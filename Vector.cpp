#ifndef VECTOR_CPP
#define VECTOR_CPP

#include "Vector.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

// Default constructor
template <typename T>
Vector<T>::Vector() : begin_(nullptr), end_(nullptr), end_cap_(nullptr) {}

// Vector(n)
template <typename T>
Vector<T>::Vector(std::size_t n) : Vector() {
    if (n > 0) {
        begin_ = new T[n]();
        end_ = begin_ + n;
        end_cap_ = begin_ + n;
    }
}

// Vector(n, value)
template <typename T>
Vector<T>::Vector(std::size_t n, const T& value) : Vector() {
    if (n > 0) {
        begin_ = new T[n];
        end_ = begin_ + n;
        end_cap_ = begin_ + n;
        for (std::size_t i = 0; i < n; ++i) {
            begin_[i] = value;
        }
    }
}

// Initializer list constructor
template <typename T>
Vector<T>::Vector(std::initializer_list<T> init) : Vector() {
    if (init.size() > 0) {
        std::size_t n = init.size();
        begin_ = new T[n];
        end_ = begin_ + n;
        end_cap_ = begin_ + n;
        std::size_t i = 0;
        for (const auto& item : init) {
            begin_[i++] = item;
        }
    }
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& other) : Vector() {
    std::size_t n = other.size();
    if (n > 0) {
        begin_ = new T[n];
        end_ = begin_ + n;
        end_cap_ = begin_ + n;
        for (std::size_t i = 0; i < n; ++i) {
            begin_[i] = other.begin_[i];
        }
    }
}

// Copy assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
    if (this != &other) {
        std::size_t n = other.size();
        T* new_begin = nullptr;
        if (n > 0) {
            new_begin = new T[n];
            for (std::size_t i = 0; i < n; ++i) {
                new_begin[i] = other.begin_[i];
            }
        }
        delete[] begin_;
        begin_ = new_begin;
        end_ = new_begin + n;
        end_cap_ = new_begin + n;
    }
    return *this;
}

// Destructor
template <typename T>
Vector<T>::~Vector() {
    delete[] begin_;
    begin_ = nullptr;
    end_ = nullptr;
    end_cap_ = nullptr;
}

// Element access: operator[]
template <typename T>
T& Vector<T>::operator[](std::size_t i) {
    return begin_[i];
}

template <typename T>
const T& Vector<T>::operator[](std::size_t i) const {
    return begin_[i];
}

// Element access: at
template <typename T>
T& Vector<T>::at(std::size_t i) {
    if (i >= size()) {
        throw std::out_of_range("Vector::at: index out of range");
    }
    return begin_[i];
}

// front / back
template <typename T>
T& Vector<T>::front() {
    if (empty()) {
        throw std::out_of_range("Vector::front: vector is empty");
    }
    return *begin_;
}

template <typename T>
T& Vector<T>::back() {
    if (empty()) {
        throw std::out_of_range("Vector::back: vector is empty");
    }
    return *(end_ - 1);
}

// data
template <typename T>
T* Vector<T>::data() {
    return begin_;
}

// Iterators
template <typename T>
T* Vector<T>::begin() {
    return begin_;
}

template <typename T>
const T* Vector<T>::begin() const {
    return begin_;
}

template <typename T>
T* Vector<T>::end() {
    return end_;
}

template <typename T>
const T* Vector<T>::end() const {
    return end_;
}

// Capacity helpers
template <typename T>
std::size_t Vector<T>::size() const {
    return static_cast<std::size_t>(end_ - begin_);
}

template <typename T>
std::size_t Vector<T>::capacity() const {
    return static_cast<std::size_t>(end_cap_ - begin_);
}

template <typename T>
bool Vector<T>::empty() const {
    return begin_ == end_;
}

// Internal reallocation helper
template <typename T>
void Vector<T>::reallocate(std::size_t new_cap) {
    if (new_cap == 0) {
        delete[] begin_;
        begin_ = nullptr;
        end_ = nullptr;
        end_cap_ = nullptr;
        return;
    }

    T* new_begin = new T[new_cap];
    std::size_t cur_size = size();
    std::size_t count = std::min(cur_size, new_cap);

    for (std::size_t i = 0; i < count; ++i) {
        new_begin[i] = begin_[i];
    }

    delete[] begin_;
    begin_ = new_begin;
    end_ = new_begin + count;
    end_cap_ = new_begin + new_cap;
}

// reserve
template <typename T>
void Vector<T>::reserve(std::size_t n) {
    if (n > capacity()) {
        reallocate(n);
    }
}

// shrink_to_fit
template <typename T>
void Vector<T>::shrink_to_fit() {
    if (capacity() != size()) {
        reallocate(size());
    }
}

// push_back
template <typename T>
void Vector<T>::push_back(const T& value) {
    if (end_ == end_cap_) {
        std::size_t cur_cap = capacity();
        std::size_t factor_cap = static_cast<std::size_t>(cur_cap * growth_factor);
        std::size_t new_cap = std::max(cur_cap + 1, factor_cap);

        T val_copy = value;
        reallocate(new_cap);
        *end_ = val_copy;
        ++end_;
    } else {
        *end_ = value;
        ++end_;
    }
}

// pop_back
template <typename T>
void Vector<T>::pop_back() {
    if (empty()) {
        throw std::out_of_range("Vector::pop_back: vector is empty");
    }
    --end_;
}

// clear
template <typename T>
void Vector<T>::clear() {
    end_ = begin_;
}

// resize
template <typename T>
void Vector<T>::resize(std::size_t n) {
    std::size_t cur_size = size();
    if (n < cur_size) {
        end_ = begin_ + n;
    } else if (n > cur_size) {
        if (n > capacity()) {
            reallocate(n);
        }
        while (size() < n) {
            *end_ = T();
            ++end_;
        }
    }
}

// insert
template <typename T>
void Vector<T>::insert(std::size_t index, const T& value) {
    if (index > size()) {
        throw std::out_of_range("Vector::insert: index out of range");
    }

    T val_copy = value;
    if (end_ == end_cap_) {
        std::size_t cur_cap = capacity();
        std::size_t factor_cap = static_cast<std::size_t>(cur_cap * growth_factor);
        std::size_t new_cap = std::max(cur_cap + 1, factor_cap);
        reallocate(new_cap);
    }

    for (std::size_t i = size(); i > index; --i) {
        begin_[i] = begin_[i - 1];
    }
    begin_[index] = val_copy;
    ++end_;
}

// erase
template <typename T>
void Vector<T>::erase(std::size_t index) {
    if (index >= size()) {
        throw std::out_of_range("Vector::erase: index out of range");
    }

    for (std::size_t i = index; i + 1 < size(); ++i) {
        begin_[i] = begin_[i + 1];
    }
    --end_;
}

// Equality operators
template <typename T>
bool operator==(const Vector<T>& a, const Vector<T>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (!(a[i] == b[i])) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool operator!=(const Vector<T>& a, const Vector<T>& b) {
    return !(a == b);
}

#endif