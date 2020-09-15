#ifndef VARIANT_H
#define VARIANT_H

#include <cstdlib>
#include <cassert>

enum class Type
{
    String,
    Number,
    Array,
};

class Variant
{
private:
    Type m_type;
    void* m_raw { nullptr };

public:
    template<typename T>
    explicit Variant(T t, Type type) noexcept
        : m_type(type) {
        m_raw = reinterpret_cast<void*>(new T(t));
        assert(m_raw);
    }

    ~Variant() noexcept {
        std::free(m_raw);
    }

    Variant(const Variant&) = delete;
    Variant& operator=(const Variant&) = delete;

    Type type() const noexcept { return m_type; }

    bool is(Type t) const noexcept {
        return m_type == t;
    }

    template<typename T>
    T& as() noexcept {
        return reinterpret_cast<T*>(m_raw);
    }

    template<typename T>
    const T& as() const noexcept {
        return reinterpret_cast<T*>(m_raw);
    }
};

#endif // VARIANT_H
