#include <iostream>
#include "try_catch.hpp"


struct DebugDestructor {
    std::string name;

    ~DebugDestructor() {
        std::cout << "Called destructor of " << name << "\n";
    }
};


void a() {
    std::cout << "a() called\n";
    INIT(DebugDestructor, a_local);
    a_local.name = "a() local variable";

    TRY {
        std::cout << "Entered a()::TRY\n";

        INIT(DebugDestructor, a_try);
        a_try.name = "a()::TRY local variable";

        std::cout << "a()::TRY throw 2\n";
        THROW 2;
    } CATCH (1) {
        std::cout << "Entered a()::CATCH(1)\n";
    }
    FINISH

    std::cout << "Exit a()\n";
}


int main() {
    TRY {
        std::cout << "main()::TRY\n";
        a();
        std::cout << "a() finished successfully\n";
    } CATCH (2) {
        std::cout << "Captured 2 from main\n";
    }
    FINISH
}
