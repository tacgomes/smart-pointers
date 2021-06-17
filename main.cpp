#include <cassert>

#include "shared_ptr.h"
#include "weak_ptr.h"

void test1() {
    shared_ptr<int> sp1(new int(1234));
    weak_ptr<int> wp(sp1);
    {
        auto sp2 = wp.lock();
        assert(sp2);
    }
    sp1.reset();
    assert(!wp.lock());
}

void test2() {
    shared_ptr<int> sp1(new int(10));
    shared_ptr<int> sp2(new int(20));
    shared_ptr<int> sp3(sp1);
    shared_ptr<int> sp4(std::move(sp1));
    shared_ptr<int> sp5;
    sp5 = sp2;
    sp5 = std::move(sp2);
}

void test3() {
    shared_ptr<int> sp1(new int(10));
    weak_ptr<int> wp1(sp1);
    weak_ptr<int> wp2(wp1);
    weak_ptr<int> wp3(std::move(wp2));
    wp3 = wp1;
    wp3 = std::move(wp1);
    wp1.reset();
    assert(!wp1.lock());
}

void test4() {
    class Base {};
    class Derived : public Base {};
    shared_ptr<Derived> sp1(new Derived);
    shared_ptr<Base> sp2 (sp1);
}

int main() {
    test1();
    test2();
    test3();
    test4();
}
