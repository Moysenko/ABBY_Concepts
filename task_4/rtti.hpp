#include <functional>
#include <string>


class TypeID{
    const std::string _name;

public: 
    TypeID(std::string class_name): _name(class_name) {}

    const std::string& name() const {
        return _name;
    }

    size_t hash_code() const {
        return std::hash<std::string>{}(_name);
    }

    bool operator==(const TypeID& other) const {
        return _name == other.name();
    }
};


#define RTTI_CLASS_BASE(Class)\
    struct Class {\
        virtual std::string __ClassName__() const { return #Class; }\
        virtual bool __CanDynamicCastTo__(const std::string& to_typename) const {\
            return #Class == to_typename;\
        }


#define RTTI_CLASS_DERIVED(DerivedClass,  BaseClasses...)\
    struct DerivedClass: BaseClasses {\
        virtual std::string __ClassName__() const { return #DerivedClass; }\
        template <typename BaseClass>\
        bool __ ## DerivedClass ## _CanDynamicCastToBase__(const std::string& to_typename) const {\
            return this->BaseClass::__CanDynamicCastTo__(to_typename);\
        }\
        template <typename BaseClass, typename NextBaseClass, typename... Other>\
        bool __ ## DerivedClass ## _CanDynamicCastToBase__(const std::string& to_typename) const {\
            if (this->BaseClass::__CanDynamicCastTo__(to_typename)) {\
                return true;\
            }\
            return __ ## DerivedClass ## _CanDynamicCastToBase__<NextBaseClass, Other...>(to_typename);\
        }\
        virtual bool __CanDynamicCastTo__(const std::string& to_typename) const {\
            if (#DerivedClass == to_typename) {\
                return true;\
            }\
            return __ ## DerivedClass ## _CanDynamicCastToBase__<BaseClasses>(to_typename);\
        }


#define END_CLASS };


#define STRINGIFY(a) #a  // for pragma
#define DYNAMIC_CAST(ptr_from, ClassTo, ptr_to)\
    if (ptr_from == nullptr) {\
        ptr_to = nullptr;\
    } else if ((ptr_from)->__CanDynamicCastTo__(#ClassTo)) {\
        _Pragma(STRINGIFY(GCC diagnostic ignored "-Wreinterpret-base-class"))\
        ptr_to = reinterpret_cast<ClassTo*>(ptr_from);\
    } else {\
        ptr_to = nullptr;\
    }


#define TYPEID(object) TypeID{(object).__ClassName__()}