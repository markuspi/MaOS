
#include "kernel/hello.h"

#include <stdint.h>

#include <stdio>

class MyClass {
public:
    MyClass(int foo) : _foo(foo){};
    void Print() { printf("MyClass: %d\n", _foo); }

private:
    uint8_t _foo;
};

void hello_cpp() {
    MyClass* x = new MyClass(2);
    x->Print();
    printf("Hello from cpp!\n");
}
