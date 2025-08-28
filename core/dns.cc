#include "dns.h"
#include "mutex"

namespace No {
  namespace DNS {
    void AfterLookup(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
      LookupReq* ctx = (LookupReq*)LookupReq::from_req(req);
      Isolate* isolate = ctx->env()->GetIsolate();
      HandleScope handle_scope(isolate);
      Context::Scope context_scope(ctx->env()->GetContext());

      Local<Value> argv[] = {
        Integer::New(isolate, status),
        Null(isolate)
      };

      uint32_t n = 0;
      if (status == 0) {
        Local<Array> results = Array::New(isolate);
        for (auto p = res; p != nullptr; p = p->ai_next) {
            const char* addr;
            if (p->ai_family == AF_INET) {
              addr = reinterpret_cast<char*>(
                  &(reinterpret_cast<struct sockaddr_in*>(p->ai_addr)->sin_addr));
            } else if (p->ai_family == AF_INET6) {
              addr = reinterpret_cast<char*>(
                  &(reinterpret_cast<struct sockaddr_in6*>(p->ai_addr)->sin6_addr));
            } else {
              continue;
            }

            char ip[INET6_ADDRSTRLEN];
            if (uv_inet_ntop(p->ai_family, addr, ip, sizeof(ip)))
              continue;

            Local<String> s = NewString(isolate, ip);
            if (results->Set(ctx->env()->GetContext(), n, s).IsNothing())
              return;
            n++;
        }
        // No responses were found to return
        argv[1] = results;
        if (n == 0) {
          argv[0] = Integer::New(isolate, UV_EAI_NODATA);
        }
      }
      ctx->MakeCallback("oncomplete", 2, argv);
    }

    void Lookup(V8_ARGS) {
        V8_ISOLATE
        Local<Object> req_wrap_obj = args[0].As<Object>();
        v8::String::Utf8Value hostname(isolate, args[1]);
        LookupReq *ctx = new LookupReq(Environment::GetCurrent(args), req_wrap_obj);
        int family;

        switch (args[2].As<Int32>()->Value()) {
          case 0:
            family = AF_UNSPEC;
            break;
          case 4:
            family = AF_INET;
            break;
          case 6:
            family = AF_INET6;
            break;
          default:
            return;
        }

        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = family;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = 0;
        Local<Integer> ret = Integer::New(isolate, uv_getaddrinfo(Env::Environment::GetCurrent(isolate)->loop(), ctx->req(), AfterLookup, *hostname, NULL, &hints));
        V8_RETURN(ret);
    }

    std::mutex ares_library_mutex;

    void ChannelWrap::New(V8_ARGS) {
      int timeout = args[0].As<Int32>()->Value();
      int retries = args[1].As<Int32>()->Value();
      new ChannelWrap(Environment::GetCurrent(args), args.This(), timeout, retries);
    }

    void ares_poll_cb(uv_poll_t* watcher, int status, int events) {
        uv_poll_t* handle = (uv_poll_t*)watcher;
        ChannelTask* task = (ChannelTask*)handle->data;
        ChannelWrap* channel = task->channel;

        uv_timer_again(channel->timer_handle());

        if (status < 0) {
          /* An error happened. Just pretend that the socket is both readable and */
          /* writable. */
          // See https://c-ares.org/docs.html
          ares_process_fd(channel->cares_channel(), task->sock, task->sock);
          return;
        }
        ares_process_fd(channel->cares_channel(),
                        events & UV_READABLE ? task->sock : ARES_SOCKET_BAD,
                        events & UV_WRITABLE ? task->sock : ARES_SOCKET_BAD);
    }

    void ares_poll_close_cb(uv_handle_t* watcher) {
      uv_poll_t* handle = (uv_poll_t*)watcher;
      ChannelTask* task = (ChannelTask*)handle->data;
      delete task;
    }

    void ares_sockstate_cb(void* data, ares_socket_t sock, int read, int write) {
      ChannelWrap* channel = static_cast<ChannelWrap*>(data);
      ChannelTask* task;

      auto it = channel->task_list()->find(sock);
      task = (it == channel->task_list()->end()) ? nullptr : it->second;

      if (task == nullptr) {
        if (!read && !write) {
          return;
        }
        task = ChannelTask::Create(channel, sock);
        if (task == nullptr) {
          return;
        }
        channel->StartTimer();
        channel->task_list()->insert(std::pair<ares_socket_t, ChannelTask *>(sock, task));
      } else {
        if (!read && !write) {
          channel->task_list()->erase(sock);
          uv_close((uv_handle_t*)&task->poll_watcher, ares_poll_close_cb);
          if (channel->task_list()->empty()) {
            channel->CloseTimer();
          }
          return;
        }
      }
      uv_poll_start(&task->poll_watcher,
                    (read ? UV_READABLE : 0) | (write ? UV_WRITABLE : 0),
                    ares_poll_cb);
    }
    
    int ChannelWrap::init() {
      struct ares_options options;
      memset(&options, 0, sizeof(options));
      options.flags = ARES_FLAG_NOCHECKRESP;
      options.sock_state_cb = ares_sockstate_cb;
      options.sock_state_cb_data = this;
      options.timeout = _timeout;
      options.tries = _retries;

      int r;
      if (!_library_inited) {
        std::lock_guard lock(ares_library_mutex);
        // Multiple calls to ares_library_init() increase a reference counter
        r = ares_library_init(ARES_LIB_INIT_ALL);
        if (r != ARES_SUCCESS)
          return -1;
      }

      const int optmask =
          ARES_OPT_FLAGS | ARES_OPT_TIMEOUTMS |
          ARES_OPT_SOCK_STATE_CB | ARES_OPT_TRIES;
      r = ares_init_options(&_channel, &options, optmask);

      if (r != ARES_SUCCESS) {
        std::lock_guard lock(ares_library_mutex);
        // decrease a reference counter
        ares_library_cleanup();
        return -1;
      }

      _library_inited = true;
      return 0;
    }

    ChannelWrap::ChannelWrap(No::Env::Environment *env, Local<Object> obj, int timeout, int retries): AsyncWrap(env, obj) {
      _timeout = timeout;
      _retries = retries;
      init();
      MakeWeak();
    }

    void ChannelWrap::StartTimer() {
      if (timer_handle_ == nullptr) {
        timer_handle_ = new uv_timer_t();
        timer_handle_->data = static_cast<void*>(this);
        uv_timer_init(env()->loop(), timer_handle_);
      } else if (uv_is_active(reinterpret_cast<uv_handle_t*>(timer_handle_))) {
        return;
      }
      int timeout = _timeout;
      if (timeout == 0) timeout = 1;
      if (timeout < 0 || timeout > 1000) timeout = 1000;
      uv_timer_start(timer_handle_, [](uv_timer_t* handle) {
        ChannelWrap* channel = static_cast<ChannelWrap*>(handle->data);
        ares_process_fd(channel->cares_channel(), ARES_SOCKET_BAD, ARES_SOCKET_BAD);
      }, timeout, timeout);
    }

    void ChannelWrap::CloseTimer() {
      if (timer_handle_ == nullptr)
        return;
      uv_close((uv_handle_t *)timer_handle_, [](uv_handle_t * handle) {
        uv_timer_t * timer = (uv_timer_t *)handle;
        delete timer; 
      });
      timer_handle_ = nullptr;
    }

    ChannelWrap::~ChannelWrap() {
      ares_destroy(_channel);

      if (_library_inited) {
        std::lock_guard lock(ares_library_mutex);
        // This decreases the reference counter increased by ares_library_init().
        ares_library_cleanup();
      }

      CloseTimer();
    }


    static void addrinfo_cb(void *arg, int status, int timeouts,
                          struct ares_addrinfo *result) {
      QueryReq* ctx = (QueryReq*)arg;
      Isolate *isolate = ctx->env()->GetIsolate();
      Local<Context> context =  ctx->env()->GetContext();
      Local<Value> argv[] = {
        Integer::New(isolate, status),
        Null(isolate)
      };
      Local<Array> results = Array::New(isolate);
      int n = 0;
      if (result) {
        struct ares_addrinfo_node *node;
        for (node = result->nodes; node != NULL; node = node->ai_next) {
          char        addr_buf[64] = "";
          const void *ptr          = NULL;
          if (node->ai_family == AF_INET) {
            const struct sockaddr_in *in_addr =
              (const struct sockaddr_in *)((void *)node->ai_addr);
            ptr = &in_addr->sin_addr;
          } else if (node->ai_family == AF_INET6) {
            const struct sockaddr_in6 *in_addr =
              (const struct sockaddr_in6 *)((void *)node->ai_addr);
            ptr = &in_addr->sin6_addr;
          } else {
            continue;
          }
          ares_inet_ntop(node->ai_family, ptr, addr_buf, sizeof(addr_buf));
          Local<String> s = NewString(isolate, addr_buf);
          if (results->Set(context, n, s).IsNothing())
            return;
          n++;
        }
      }
      ares_freeaddrinfo(result);
      if (n != 0) {
        argv[1] = results;
      }
      ctx->MakeCallback("oncomplete", 2, argv);
    }

    void ChannelWrap::Resolve(V8_ARGS) {
      ChannelWrap* wrap = (ChannelWrap*)Base::BaseObject::unwrap(args.Holder());
      struct ares_addrinfo_hints hints;
      memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_UNSPEC;
      hints.ai_flags  = ARES_AI_CANONNAME;
      Local<Object> req_wrap_obj = args[0].As<Object>();
      v8::String::Utf8Value hostname(args.GetIsolate(), args[1]);
      QueryReq *ctx = new QueryReq(Environment::GetCurrent(args), req_wrap_obj);
      ares_getaddrinfo(wrap->cares_channel(), *hostname, NULL, &hints, addrinfo_cb, (void *)ctx);
    }

    ChannelTask* ChannelTask::Create(ChannelWrap* channel, ares_socket_t sock) {
      auto task = new ChannelTask();

      task->channel = channel;
      task->sock = sock;
      task->poll_watcher.data = task;
      if (uv_poll_init_socket(channel->env()->loop(),
                              &task->poll_watcher, sock) < 0) {
        delete task;
        return nullptr;
      }

      return task;
    }

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);
      
      Environment *env = Environment::GetCurrent(isolate);
      Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, ChannelWrap::New);
      SetProtoMethod(isolate, tpl, "resolve", ChannelWrap::Resolve);
      tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "Channel"), tpl);
      
      Local<FunctionTemplate> lookup = No::Util::NewFunctionTemplate(isolate, Lookup);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "lookup"), lookup);

      Local<FunctionTemplate> query_req = No::Util::NewDefaultFunctionTemplate(isolate);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "QueryReq"), query_req);
      ObjectSet(isolate, target, "dns", obj);
    }

    static void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
      registry->Register(ChannelWrap::New);
      registry->Register(ChannelWrap::Resolve);
      registry->Register(Lookup);
    }
  }
}

NODE_BINDING_EXTERNAL_REFERENCE(dns, No::DNS::RegisterExternalReferences)