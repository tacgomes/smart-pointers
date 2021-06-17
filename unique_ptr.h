#pragma once

#include <algorithm>
#include <cassert>

template <class T>
class unique_ptr {
public:
    unique_ptr() noexcept = default;

    explicit unique_ptr(T* data)
        : data_(data) {
    }

    unique_ptr(const unique_ptr& up) = delete;

    unique_ptr(unique_ptr&& up) noexcept
        : data_(up.data_){
        up.data_ = nullptr;
    }

    ~unique_ptr() {
        delete data_;
        data_ = nullptr;
    }

    unique_ptr& operator=(const unique_ptr& up) = delete;

    unique_ptr& operator=(unique_ptr&& up) noexcept {
        reset(up.data_);
        up.data_ = nullptr;
        return *this;
    }

    T* get() const noexcept {
        return data_;
    }

    T* operator->() const noexcept {
        return data_;
    }

    T& operator*() const noexcept {
        assert(*this);
        return *data_;
    }

    explicit operator bool() const noexcept {
        return data_ != nullptr;
    }

    T* release() noexcept {
        T* tmp = data_;
        data_ = nullptr;
        return tmp;
    }

    void reset(T* data = nullptr) noexcept {
        delete data_;
        data_ = data;
    }

private:
    T* data_ = nullptr;
};
