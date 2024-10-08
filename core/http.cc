#include "http.h"

namespace No {
    namespace HTTP {
        Parser::Parser(Environment* env, Local<Object> object): AsyncWrap(env, object) {
            parser_callback callback = {
                this,
                [](on_headers_complete_info info, parser_callback callback) {
                    Parser * parser = (Parser *)callback.data;
                    Isolate *isolate = parser->env()->GetIsolate();
                    Local<Context> context = parser->env()->GetContext();
                    
                    Local<Object> obj = Object::New(isolate);
                    obj->Set(context, NewString(isolate, "major_version"), Integer::New(isolate, info.major_version));
                    obj->Set(context, NewString(isolate, "minor_version"), Integer::New(isolate, info.minor_version));
                    obj->Set(context, NewString(isolate, "keepalive"), Integer::New(isolate, info.keepalive));
                    obj->Set(context, NewString(isolate, "upgrade"), Integer::New(isolate, info.upgrade));
                    obj->Set(context, NewString(isolate, "url"), NewString(isolate, info.url.c_str()));
                    Local<Object> headers = Object::New(isolate);
                    for (int i = 0; i < info.keys.size(); i++) {
                        headers->Set(context, NewString(isolate, info.keys[i].c_str()), NewString(isolate, info.values[i].c_str()));
                    }
                    obj->Set(context, NewString(isolate, "headers"), headers);
                    
                    Local<Value> argv[] = {
                        obj
                    };
                    parser->MakeCallback("onHeaderComplete", 1, argv);
                },

                [](on_body_info info, parser_callback callback) {
                    Parser * parser = (Parser *)callback.data;
                    Isolate *isolate = parser->env()->GetIsolate();
                    Local<Value> argv[] = {
                        NewString(isolate, info.body.c_str())
                    };
                    parser->MakeCallback("onBody", 1, argv);
                },

                [](on_body_complete_info info, parser_callback callback) {
                    Parser * parser = (Parser *)callback.data;
                    Isolate *isolate = parser->env()->GetIsolate();
                    Local<Value> argv[] = {
                        NewString(isolate, info.body.c_str())
                    };
                    parser->MakeCallback("onBodyComplete", 1, argv);
                },
            };
            httpparser = std::make_unique<HTTPParser>(HTTP_REQUEST, callback);
        }

        void Parser::New(const FunctionCallbackInfo<Value>& args) {
            Environment* env = Environment::GetCurrent(args);
            new Parser(env, args.This());
        }

        void Parser::Parse(const FunctionCallbackInfo<Value>& args) {
            Parser * parser = (Parser *)unwrap(args.This());
            Local<Uint8Array> uint8Array = args[0].As<Uint8Array>();
            Local<ArrayBuffer> arrayBuffer = uint8Array->Buffer();
            std::shared_ptr<BackingStore> backing = arrayBuffer->GetBackingStore();
            const char * data = (const char * )backing->Data();
            parser->httpparser->parse(data, strlen(data));
        }

        void Init(Isolate* isolate, Local<Object> target) {
            Local<FunctionTemplate> parser = FunctionTemplate::New(isolate, Parser::New);
            parser->InstanceTemplate()->SetInternalFieldCount(No::Base::BaseObject::kInternalFieldCount);
            parser->SetClassName(NewString(isolate, "HTTPParser"));
            parser->PrototypeTemplate()->Set(NewString(isolate, "parse"), FunctionTemplate::New(isolate, Parser::Parse));
            ObjectSet(isolate, target, "HTTPParser", parser->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
        }
    }
}