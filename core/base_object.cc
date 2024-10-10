#include "base_object.h"

namespace No {
  namespace Base {

    void BaseObject::WeakCallback(const WeakCallbackInfo<BaseObject>& data) {
        BaseObject* obj = data.GetParameter();
        delete obj;
    }

    void BaseObject::Release() {
        delete this;
    }

    BaseObject::BaseObject(Environment* env, Local<Object> object)
        : _env(env), _object(env->GetIsolate(), object) {
        object->SetAlignedPointerInInternalField(BaseObject::Slot, static_cast<void*>(this));
    }

    BaseObject::~BaseObject() {
      object()->SetAlignedPointerInInternalField(BaseObject::Slot, nullptr);
      _object.Reset();
    }

    void BaseObject::MakeWeak() {
       _object.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);
    }
    
    Environment* BaseObject::env() const {
      return _env;
    }

    Local<Object> BaseObject::object() {
      return PersistentToLocal::Strong(_object);
    }

    Global<Object>& BaseObject::persistent_object() {
      return _object;
    }

    void * BaseObject::unwrap(Local<Object> object) {
      return object->GetAlignedPointerFromInternalField(BaseObject::Slot);
    }
  }
}