#pragma once

#include <algorithm>

#include "cblock.h"


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
