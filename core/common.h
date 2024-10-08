#ifndef MACRO_H
#define MACRO_H

#include "v8.h"
#include "uv.h"
#include <stdio.h>

#define V8_ARGS const FunctionCallbackInfo<Value> &args
#define V8_ISOLATE Isolate * isolate = args.GetIsolate();
#define V8_CONTEXT Local<Context> context = args.GetIsolate()->GetCurrentContext();
#define V8_RETURN(ret) args.GetReturnValue().Set(ret);
#define DEFIND_JS_FUNC(FUNC) static void FUNC(V8_ARGS);

#endif