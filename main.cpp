#include <algorithm>
#include <iostream>
#include <cassert>

struct cblock {
    int shared_count = 1;
    int weak_count = 0;
};

template <class T> class weak_ptr;

template <class T>
class shared_ptr {
public:
    shared_ptr() noexcept = default;

    shared_ptr(T* data)
        : data_(data) {
        if (data_) {
            cblock_ = new cblock();
        }
    }

    shared_ptr(const shared_ptr& sp) noexcept
        : data_(sp.data_)
        , cblock_(sp.cblock_) {
        if (cblock_) {
            ++cblock_->shared_count;
        }
    }

    template <class D>
    shared_ptr(const shared_ptr<D>& sp) noexcept
        : data_(sp.data_)
        , cblock_(sp.cblock_) {
        if (cblock_) {
            ++cblock_->shared_count;
        }
    }

    shared_ptr(const weak_ptr<T>& wp) noexcept
        : data_(wp.data_)
        , cblock_(wp.cblock_) {
        if (cblock_) {
            ++cblock_->shared_count;
        }
    }

    shared_ptr(shared_ptr&& sp) noexcept {
        swap(sp);
        sp.reset();
    }

    ~shared_ptr() {
        dec_shared_count();
    }

    shared_ptr& operator=(const shared_ptr& sp) noexcept {
        shared_ptr tmp(sp);
        swap(tmp);
        return *this;
    }

   shared_ptr& operator=(shared_ptr&& sp) noexcept {
       swap(sp);
       sp.reset();
       return *this;
   }

    const T& operator*() const noexcept {
        return *data_;
    }

    T& operator*() noexcept {
        return *data_;
    }

    const T* operator->() const noexcept {
        return data_;
    }

    T* operator->() noexcept {
        return data_;
    }

    explicit operator bool() const noexcept {
        return data_ != nullptr;
    }

    void reset() noexcept {
        dec_shared_count();
        data_ = nullptr;
        cblock_ = nullptr;
    }

private:
    void swap(shared_ptr& sp) noexcept {
        using std::swap;
        swap(data_, sp.data_);
        swap(cblock_, sp.cblock_);
    }

    void dec_shared_count() {
        if (!cblock_) {
            return;
        }

        --cblock_->shared_count;

        if (cblock_->shared_count <= 0) {
            delete data_;
            data_ = nullptr;
            if (cblock_->weak_count <= 0) {
                delete cblock_;
                cblock_ = nullptr;
            }
        }
    }

    T* data_ = nullptr;
    cblock* cblock_ = nullptr;

    template<class Y> friend class shared_ptr;
    template<class Y> friend class weak_ptr;
};

template <class T>
class weak_ptr {
public:
    weak_ptr() noexcept = default;

    weak_ptr(const weak_ptr& wp) noexcept
        : data_(wp.data_)
        , cblock_(wp.cblock_) {
        if (cblock_) {
            ++cblock_->weak_count;
        }
    }

    weak_ptr(const shared_ptr<T>& sp) noexcept
        : data_(sp.data_)
        , cblock_(sp.cblock_) {
        if (cblock_) {
            ++cblock_->weak_count;
        }
    }

    weak_ptr(weak_ptr&& wp) noexcept {
        swap(wp);
        wp.reset();
    }

    ~weak_ptr() {
        dec_weak_count();
    }

    weak_ptr& operator=(const weak_ptr& wp) noexcept {
        weak_ptr tmp(wp);
        swap(tmp);
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& wp) noexcept {
        swap(wp);
        wp.reset();
        return *this;
    }

    shared_ptr<T> lock() const {
        return (data_ && cblock_->shared_count > 0)
            ? shared_ptr<T>(*this)
            : shared_ptr<T>(nullptr);
    }

    void reset() noexcept {
        dec_weak_count();
        data_ = nullptr;
        cblock_ = nullptr;
    }

private:
    void swap(weak_ptr& wp) noexcept {
        using std::swap;
        swap(data_, wp.data_);
        swap(cblock_, wp.cblock_);
    }

    void dec_weak_count() {
        if (!cblock_) {
            return;
        }

        --cblock_->weak_count;

        if (cblock_->shared_count <= 0 && cblock_->weak_count <= 0) {
            delete cblock_;
            cblock_ = nullptr;
        }
    }

    T* data_ = nullptr;
    cblock* cblock_ = nullptr;

    template<class Y> friend class shared_ptr;
};


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
