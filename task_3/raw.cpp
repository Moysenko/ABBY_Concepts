#include <unordered_map>
#include <iostream>
#include <functional>
#include <string>


struct __VTableHolder__ {
    typedef void (*MethodType)();
    std::unordered_map<std::string_view, MethodType> vtable;
    __VTableHolder__* parent_vtable_holder;

    __VTableHolder__(__VTableHolder__* parent_ptr): parent_vtable_holder(parent_ptr) {}
};


struct A {
    static __VTableHolder__ __vtable_holder__;
    __VTableHolder__ * __vtable_ptr__ = &__vtable_holder__;

    int a;
};
__VTableHolder__ A::__vtable_holder__(nullptr);


void A_CommonMethod() {
    std::cout << "Base common method\n";
}
constexpr int __Add_A_CommonMethod__() {
    A::__vtable_holder__.vtable["CommonMethod"] = &A_CommonMethod;
    return 0;
}
int A_CommonMethod_INIT = __Add_A_CommonMethod__();

struct B: A {
    static __VTableHolder__ __vtable_holder__;
    B() {
        __vtable_ptr__ = &__vtable_holder__;
    }

    int b;
};
__VTableHolder__ B::__vtable_holder__(A::__vtable_holder__);

void B_CommonMethod() {
    std::cout << "Derived common method\n";
}
constexpr int __Add_B_CommonMethod__() {
    B::__vtable_holder__.vtable["CommonMethod"] = &B_CommonMethod;
    return 0;
}
int B_CommonMethod_INIT = __Add_B_CommonMethod__();

int main() {
    B obj;
    obj.a = 1;
    obj.b = 2;
    A* base_ptr = &obj;
    std::cout << "CAST" << std::endl;

    __VTableHolder__* current_vtable = base_ptr->__vtable_ptr__;
    __VTableHolder__::MethodType method_ptr;
    while (current_vtable != nullptr) {
        if (current_vtable->vtable.count("CommonMethod")) {
            method_ptr = current_vtable->vtable["CommonMethod"];
            break;
        }
        current_vtable = current_vtable->parent_vtable_holder;
    }
    if (method_ptr == nullptr) {
        throw std::runtime_error("Object base_ptr has no method CommonMethod");
    }
    method_ptr();

}