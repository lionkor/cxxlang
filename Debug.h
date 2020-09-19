#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#if DBG
#define dbg_fn_noarg() std::cout << "CALL: " \
                                 << "(L" << __LINE__ << "): " << raw << "::" << __FUNCTION__ << std::endl
#define dbg_fn() std::cout << "CALL: " \
                           << "(L" << __LINE__ << "): " << raw << "::" << __FUNCTION__ << " " << b << std::endl
#else
#define dbg_fn_noarg()
#define dbg_fn()
#endif // DBG

template<typename T>
class Debug
{
private:
    T raw;

public:
    explicit Debug()
        : raw() {
    }

    explicit Debug(T t)
        : raw(t) {
    }

    // Comparison

    bool operator==(const Debug& b) const {
        dbg_fn();
        return raw == b.raw;
    }
    bool operator!=(const Debug& b) const {
        dbg_fn();
        return raw != b.raw;
    }
    bool operator<(const Debug& b) const {
        dbg_fn();
        return raw < b.raw;
    }
    bool operator>(const Debug& b) const {
        dbg_fn();
        return raw > b.raw;
    }
    bool operator<=(const Debug& b) const {
        dbg_fn();
        return raw <= b.raw;
    }
    bool operator>=(const Debug& b) const {
        dbg_fn();
        return raw >= b.raw;
    }


    // Logical

    bool operator!() const {
        dbg_fn_noarg();
        return !raw;
    }
    bool operator&&(const Debug& b) const {
        dbg_fn();
        return raw && b.raw;
    }
    bool operator||(const Debug& b) const {
        dbg_fn();
        return raw || b.raw;
    }

    // Arithmetic

    Debug operator+() const {
        dbg_fn_noarg();
        return +raw;
    }
    Debug operator-() const {
        dbg_fn_noarg();
        return -raw;
    }
    Debug operator+(const Debug& b) const {
        dbg_fn();
        return raw + b.raw;
    }
    Debug operator-(const Debug& b) const {
        dbg_fn();
        return raw - b.raw;
    }
    Debug operator*(const Debug& b) const {
        dbg_fn();
        return raw * b.raw;
    }
    Debug operator/(const Debug& b) const {
        dbg_fn();
        return raw / b.raw;
    }
    Debug operator%(const Debug& b) const {
        dbg_fn();
        return raw % b.raw;
    }
    Debug operator~() const {
        dbg_fn_noarg();
        return ~raw;
    }
    Debug operator&(const Debug& b) const {
        dbg_fn();
        return raw & b.raw;
    }
    Debug operator|(const Debug& b) const {
        dbg_fn();
        return raw | b.raw;
    }
    Debug operator^(const Debug& b) const {
        dbg_fn();
        return raw ^ b.raw;
    }
    Debug operator<<(const Debug& b) const {
        dbg_fn();
        return raw << b.raw;
    }
    Debug operator>>(const Debug& b) const {
        dbg_fn();
        return raw >> b.raw;
    }

    // Incr / Decr

    Debug& operator++() {
        dbg_fn_noarg();
        ++raw;
        return *this;
    }
    Debug& operator--() {
        dbg_fn_noarg();
        --raw;
        return *this;
    }
    Debug operator++(int) {
        dbg_fn_noarg();
        return Debug(raw++);
    }
    Debug operator--(int) {
        dbg_fn_noarg();
        return Debug(raw--);
    }

    // Assignment

    Debug& operator=(const T& b) {
        dbg_fn();
        raw = b;
        return *this;
    }

    Debug& operator=(const Debug& b) {
        dbg_fn();
        raw = b.raw;
        return *this;
    }
    Debug& operator+=(const Debug& b) {
        dbg_fn();
        raw += b.raw;
        return *this;
    }
    Debug& operator-=(const Debug& b) {
        dbg_fn();
        raw -= b.raw;
        return *this;
    }
    Debug& operator*=(const Debug& b) {
        dbg_fn();
        raw *= b.raw;
        return *this;
    }
    Debug& operator/=(const Debug& b) {
        dbg_fn();
        raw /= b.raw;
        return *this;
    }
    Debug& operator%=(const Debug& b) {
        dbg_fn();
        raw %= b.raw;
        return *this;
    }
    Debug& operator&=(const Debug& b) {
        dbg_fn();
        raw &= b.raw;
        return *this;
    }
    Debug& operator|=(const Debug& b) {
        dbg_fn();
        raw |= b.raw;
        return *this;
    }
    Debug& operator^=(const Debug& b) {
        dbg_fn();
        raw ^= b.raw;
        return *this;
    }
    Debug& operator<<=(const Debug& b) {
        dbg_fn();
        raw <<= b.raw;
        return *this;
    }
    Debug& operator>>=(const Debug& b) {
        dbg_fn();
        raw >>= b.raw;
        return *this;
    }

    T& get_raw() { return raw; }
    const T& get_raw() const { return raw; }

    operator T&() {
        return raw;
    }

    operator const T&() const {
        return raw;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Debug<T>& val) {
    return os << val.get_raw();
}

#endif // DEBUG_H
