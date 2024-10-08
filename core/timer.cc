#include "timer.h"           

namespace No {
    namespace Timer {
      void TimerWrap::New(const FunctionCallbackInfo<Value>& args) {
        new TimerWrap(Environment::GetCurrent(args), args.This());
      }

      void OnTimeout(uv_timer_t *handle) {
        TimerWrap* wrap = (TimerWrap*)handle->data;
        wrap->MakeCallback("ontimeout", 0, nullptr);
      }

      void TimerWrap::Start(V8_ARGS) {
        TimerWrap* wrap = (TimerWrap*)Base::BaseObject::unwrap(args.Holder());
        uint32_t timeout;
        if (!args[0]->Uint32Value(wrap->env()->GetContext()).To(&timeout)) return;
        uint32_t repeat;
        if (!args[1]->Uint32Value(wrap->env()->GetContext()).To(&repeat)) return;
        int err = uv_timer_start(&wrap->handle_, OnTimeout, timeout, repeat);
        args.GetReturnValue().Set(err);
      }

      void TimerWrap::Stop(V8_ARGS) {
        TimerWrap* wrap = (TimerWrap*)Base::BaseObject::unwrap(args.Holder());
        int err = uv_timer_stop(&wrap->handle_);
        args.GetReturnValue().Set(err);
      }

      void Init(Isolate* isolate, Local<Object> target) {
        Local<Object> obj = Object::New(isolate);

        Local<FunctionTemplate> tpl = No::Util::NewFunctionTemplate(isolate, TimerWrap::New);
        SetProtoMethod(isolate, tpl, "start", TimerWrap::Start);
        SetProtoMethod(isolate, tpl, "stop", TimerWrap::Stop);
        tpl->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
        SetFunction(isolate->GetCurrentContext(), obj, NewString(isolate, "Timer"), tpl);

        ObjectSet(isolate, target, "timer", obj);
      }
    }
}