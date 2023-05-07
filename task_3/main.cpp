#include "inheritance.hpp"

// базовый класс
VIRTUAL_CLASS( Base )
	int a;
END( Base )
// методы
DECLARE_METHOD( Base, Both )
DECLARE_METHOD( Base, OnlyBase )

// класс-наследник
VIRTUAL_CLASS_DERIVED( Derived, Base )
	int b;
END_DERIVE( Derived, Base )
// методы
DECLARE_METHOD( Derived, Both )
DECLARE_METHOD( Derived, OnlyDerived )


int main()
{
	Base base;
	base.a = 0;

	Derived derived;
    derived.b = 1;
	
	// полиморфизм
    Base* reallyDerived = reinterpret_cast<Base*>(&derived);

	VIRTUAL_CALL(&base, Both); // печатает “Base::Both a = 0”
	VIRTUAL_CALL(reallyDerived, Both); // печатает “Derived::Both b = 1”
	VIRTUAL_CALL(reallyDerived, OnlyBase);  // печатает “Base::OnlyBase”
	VIRTUAL_CALL(reallyDerived, OnlyDerived);
    VIRTUAL_CALL(&base, OnlyDerived);
}
