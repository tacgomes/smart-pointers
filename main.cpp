#include <cassert>

#include "unique_ptr.h"
#include "shared_ptr.h"
#include "weak_ptr.h"

void test_unique_ptr() {
    {
        unique_ptr<int> up1(new int(1));
        assert(*up1 == 1);
        assert(*up1.get() == 1);
    }

    {
        unique_ptr<int> up1(new int(1));
        unique_ptr<int> up2(std::move(up1));
    }

    {
        unique_ptr<int> up1(new int(1));
        unique_ptr<int> up2;
        up2 = std::move(up1);
    }

    {
        unique_ptr<int> up1(new int(1));
        up1 = std::move(up1);
    }

    {
        unique_ptr<int> up1(new int(1));
        up1.reset(new int(2));
        assert(*up1 == 2);
    }

    {
        unique_ptr<int> up1(new int(1));
        int* data = up1.release();
        assert(!up1);
        delete data;
    }
}

void test_shared_ptr() {
    {
        shared_ptr<int> sp1;
        assert(sp1.get() == nullptr);
        assert(!sp1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        assert(*sp1 == 1);
        assert(*sp1.get() == 1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(sp1);
        assert(*sp1 == 1);
        assert(*sp2 == 1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(std::move(sp1));
        assert(sp1.get() == nullptr);
        assert(!sp1);
        assert(*sp2 == 1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));
        shared_ptr<int> sp3;

        sp3 = sp1;
        assert(*sp1 == 1);
        assert(*sp3 == 1);

        sp3 = sp2;
        assert(*sp2 == 2);
        assert(*sp3 == 2);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));

        shared_ptr<int> sp3;

        sp3 = std::move(sp1);
        assert(sp1.get() == nullptr);
        assert(!sp1);
        assert(*sp3 == 1);

        sp3 = std::move(sp2);
        assert(sp2.get() == nullptr);
        assert(!sp2);
        assert(*sp3 == 2);
    }

    {
        class Base {};
        class Derived : public Base {};
        shared_ptr<Derived> sp1(new Derived);
        shared_ptr<Base> sp2 (sp1);
    }
}

void test_weak_ptr() {
    {
        weak_ptr<int> wp1;
        assert(!wp1.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        assert(wp1.lock());
        wp1.reset();
        assert(!wp1.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(wp1);
        assert(wp1.lock());
        assert(wp2.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(std::move(wp1));
        assert(!wp1.lock());
        assert(wp2.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(sp2);
        weak_ptr<int> wp3;
        wp3 = wp1;
        assert(wp1.lock());
        assert(wp3.lock());
        wp3 = wp2;
        assert(wp2.lock());
        assert(wp3.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(sp1);
        weak_ptr<int> wp3;
        wp3 = std::move(wp1);
        assert(!wp1.lock());
        assert(wp3.lock());
        wp3 = std::move(wp2);
        assert(!wp2.lock());
        assert(wp3.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp(sp1);
        {
            auto sp2 = wp.lock();
            assert(sp2);
        }
        sp1.reset();
        assert(!wp.lock());
    }
}

int main() {
    test_unique_ptr();
    test_shared_ptr();
    test_weak_ptr();
}
