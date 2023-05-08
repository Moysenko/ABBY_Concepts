#include <unordered_map>
#include <iostream>
#include <string>


namespace {
    struct __VTableHolder__ {
        typedef void (*MethodType)();
        std::unordered_map<std::string_view, MethodType> vtable;
        __VTableHolder__* parent_vtable_holder;

        __VTableHolder__(__VTableHolder__* parent_ptr): parent_vtable_holder(parent_ptr) {}
    };
}  // namespace


#define VIRTUAL_CLASS(Class)\
    struct Class {\
        static __VTableHolder__ __vtable_holder__;\
        __VTableHolder__* __vtable_ptr__ = &__vtable_holder__;


#define END(Class)\
    };\
    __VTableHolder__ Class::__vtable_holder__(nullptr);


#define VIRTUAL_CLASS_DERIVED(DerivedClass, BaseClass)\
    struct DerivedClass: BaseClass {\
        static __VTableHolder__ __vtable_holder__;\
        DerivedClass() {\
            __vtable_ptr__ = &__vtable_holder__;\
        }


#define END_DERIVE(DerivedClass, BaseClass) \
    };\
    __VTableHolder__ DerivedClass::__vtable_holder__(BaseClass::__vtable_holder__);


#define DECLARE_METHOD(Class, Method)\
    void __ ## Class ## _ ## Method ## __() {\
        std::cout << #Class << "::" << #Method << "\n";\
    }\
    constexpr int __Add_ ## Class ## _ ## Method ## __() {\
        Class::__vtable_holder__.vtable[#Method] = &__ ## Class ## _ ## Method ## __ ;\
        return 0;\
    }\
    int __ ## Class ## _ ## Method ## _INIT__ = __Add_ ## Class ## _ ## Method ## __();


#define VIRTUAL_CALL(obj_ptr, Method)\
    {\
        __VTableHolder__* current_vtable = (obj_ptr)->__vtable_ptr__;\
        __VTableHolder__::MethodType method_ptr;\
        while (current_vtable != nullptr) {\
            if (current_vtable->vtable.count(#Method)) {\
                method_ptr = current_vtable->vtable[#Method];\
                break;\
            }\
            current_vtable = current_vtable->parent_vtable_holder;\
        }\
        if (method_ptr == nullptr) {\
            throw std::runtime_error("Unavailable virtual method");\
        }\
        method_ptr();\
    }