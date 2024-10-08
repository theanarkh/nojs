#include "dns.h"

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

    void Init(Isolate* isolate, Local<Object> target) {
      Local<Object> obj = Object::New(isolate);
      
      Local<FunctionTemplate> lookup = No::Util::NewFunctionTemplate(isolate, Lookup);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "lookup"), lookup);

      Local<FunctionTemplate> lookup_req = No::Util::NewDefaultFunctionTemplate(isolate);
      SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "LookupReq"), lookup_req);
      ObjectSet(isolate, target, "dns", obj);
    }
  }
}