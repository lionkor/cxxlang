#ifndef VARIANT_H
#define VARIANT_H

#include <cstdlib>
#include <cassert>
#include <cstdint>

enum class Type : uint8_t
{
    String,
    Number,
    Array,
    None
};

class Variant final
{
private:
    Type m_type;
    void* m_raw { nullptr };
    size_t m_size { 0 };

    Variant()
        : m_type(Type::None), m_raw(nullptr), m_size(0) {
    }

public:
    template<typename T>
    Variant(T t, Type type) noexcept
        : m_type(type)
        , m_size(sizeof(T)) {
        m_raw = reinterpret_cast<void*>(new T(t));
        assert(m_raw);
    }

    // forces empty variants to be constructed explicitly
    static Variant empty() { return Variant(); }

    ~Variant() noexcept {
        std::free(m_raw);
    }

    Variant(const Variant& other) = delete;
    Variant& operator=(const Variant&) = delete;

    Type type() const noexcept { return m_type; }

    bool is(Type t) const noexcept {
        return m_type == t;
    }

    template<typename T>
    bool convertible_to() const {
        if (dynamic_cast<T*>(m_raw)) {
            return true;
        } else {
            return false;
        }
    }

    template<typename T>
    T& as() noexcept {
        return *reinterpret_cast<T*>(m_raw);
    }

    template<typename T>
    const T& as() const noexcept {
        return *reinterpret_cast<T*>(m_raw);
    }
};

#endif // VARIANT_H
