namespace Papyrus {
    template <class T>
    inline void CallPapyrusAction(T* obj, const char* className, const char* methodName) {
        if (!obj) {
            return;
        }
        auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        auto handlePolicy = vm->GetObjectHandlePolicy();
        auto args = RE::MakeFunctionArguments();
        RE::VMHandle handle = handlePolicy->GetHandleForObject(obj->GetFormType(), obj);
        auto callback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>();
        bool success =
            vm->DispatchMethodCall(handle, RE::BSFixedString(className), RE::BSFixedString(methodName), args, callback);
        handlePolicy->ReleaseHandle(handle);
    }

    template <class T,class... Args>
    inline void CallPapyrusAction(T* obj, const char* className, const char* methodName,
                                  Args&&... a_args) {
        if (!obj) {
            return;
        }
        auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        auto handlePolicy = vm->GetObjectHandlePolicy();
        auto args = RE::MakeFunctionArguments(std::forward<Args>(a_args)...);
        RE::VMHandle handle = handlePolicy->GetHandleForObject(obj->GetFormType(), obj);
        auto callback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>();
        bool success =
            vm->DispatchMethodCall(handle, RE::BSFixedString(className), RE::BSFixedString(methodName), args, callback);
        handlePolicy->ReleaseHandle(handle);
    }


    inline void Delete(RE::TESObjectREFR* refr) {
        CallPapyrusAction(refr, "ObjectReference", "Delete");
    }
    inline void Play(RE::TESEffectShader* refr, RE::TESObjectREFR* parm, float duration = -1.0) {
        CallPapyrusAction(refr, "EffectShader", "Play", std::forward<RE::TESObjectREFR*>(parm),
                          std::forward<float>(duration));
    }
    inline void Stop(RE::TESEffectShader* refr, RE::TESObjectREFR* parm) {
        CallPapyrusAction(refr, "EffectShader", "Stop", std::forward<RE::TESObjectREFR*>(parm));
    }
}