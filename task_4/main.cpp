#include "rtti.hpp"
#include <iostream>


RTTI_CLASS_BASE(A)
    int a = 1;
    std::string WhoAmI() {
        return "I'm A";
    }
END_CLASS
RTTI_CLASS_BASE(B)
    int b = 2;
    std::string WhoAmI() {
        return "I'm B";
    }
END_CLASS
RTTI_CLASS_DERIVED(C, B)
    int c = 3;
    std::string WhoAmI() {
        return "I'm C";
    }
END_CLASS
RTTI_CLASS_DERIVED(D, C)
    int d = 4;
    std::string WhoAmI() {
        return "I'm D";
    }
END_CLASS
RTTI_CLASS_DERIVED(E, A, D)
    int e = 5;
    std::string WhoAmI() {
        return "I'm E";
    }
END_CLASS


// Другое дерево зависимостей
RTTI_CLASS_BASE(Other)
    int other = 0;
    std::string WhoAmI() {
        return "I'm Other";
    }
END_CLASS


// Для проверки кастов
#define CastSuccessInfo(ptr)\
    if (ptr) {\
        std::cout << "Successfully custed to " << #ptr << ": " << (ptr)->WhoAmI() << std::endl;\
    } else {\
        std::cout << #ptr << " is nullptr" << std::endl;\
    }


int main() {
    E e_var;
    // Все аргументы доступны
    std::cout << e_var.a << ' ' << e_var.b << ' ' << e_var.c << ' ' << e_var.d << ' ' << e_var.e << std::endl;
    E* e_ptr = &e_var;

    // Каст к одному из предков 
    C* c_ptr;
    DYNAMIC_CAST(e_ptr, C, c_ptr);
    CastSuccessInfo(c_ptr);

    // Каст вниз, но не к исходному классу
    D* d_ptr;
    DYNAMIC_CAST(c_ptr, D, d_ptr);
    CastSuccessInfo(d_ptr);

    // Каст к исходному классу
    DYNAMIC_CAST(d_ptr, E, e_ptr);
    CastSuccessInfo(e_ptr);


    // К классу не из дерева зависимостей не кастится
    Other* other_ptr;
    DYNAMIC_CAST(e_ptr, Other, other_ptr);
    CastSuccessInfo(other_ptr);


    // К классу ниже изначального каст невозможен...
    D d_var;
    DYNAMIC_CAST(&d_var, E, e_ptr); 
    CastSuccessInfo(e_ptr);  //nullptr

    // ... даже если сначала скастить к одному из классов-родителей
    B* b_ptr;
    DYNAMIC_CAST(&d_var, B, b_ptr);
    DYNAMIC_CAST(b_ptr, E, e_ptr);  
    CastSuccessInfo(e_ptr);  // nullptr

    auto e_type_id1 = TYPEID(e_var);
    std::cout << "E TYPEID. Name: " << e_type_id1.name() << ", hash: " << e_type_id1.hash_code() << std::endl;

    E e_var_other;
    DYNAMIC_CAST(&e_var_other, D, d_ptr)
    auto e_type_id2 = TYPEID(*d_ptr);
    std::cout << "E type_ids are equal: " << (e_type_id1 == e_type_id2) << std::endl;

    auto d_type_id = TYPEID(d_var);
    std::cout << "E and D type ids are not equal: " << (e_type_id1 == d_type_id) << std::endl;

}