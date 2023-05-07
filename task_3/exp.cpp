#include <iostream>

#define makef(a, b) void a ## _ ## b() {\
    std::cout << 1 << std::endl;\
    }


makef(c, d)

int main() {
    c_d();
}