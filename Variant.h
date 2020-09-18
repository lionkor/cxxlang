#ifndef VARIANT_H
#define VARIANT_H

#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>

enum class Type : uint8_t
{
    String,
    Number,
    Array,
    Bool,
    None,
};

static inline std::string type_to_string(Type t) {
    switch (t) {
    case Type::String:
        return "String";
        break;
    case Type::Number:
        return "Number";
        break;
    case Type::Array:
        return "Array";
        break;
    case Type::Bool:
        return "Bool";
        break;
    case Type::None:
        return "None";
        break;
    default:
        return "Unknown ???";
    }
}

class Variant final
{
private:
    Type m_type;
    std::shared_ptr<uint8_t> m_raw { nullptr };
    size_t m_size { 0 };

public:
    Variant()
        : m_type(Type::None), m_raw(nullptr), m_size(0) {
    }

    template<typename T>
    Variant(T t, Type type) noexcept
        : m_type(type)
        , m_size(sizeof(T)) {
        m_raw = std::shared_ptr<uint8_t>(reinterpret_cast<uint8_t*>(new T(t)));
        assert(m_raw.get());
    }

    // forces empty variants to be constructed explicitly
    static Variant empty() { return Variant(); }

    ~Variant() noexcept {
    }

    Type type() const noexcept { return m_type; }

    bool is(Type t) const noexcept {
        return m_type == t;
    }

    template<typename T>
    bool convertible_to() const {
        if (dynamic_cast<T*>(m_raw.get())) {
            return true;
        } else {
            return false;
        }
    }

    template<typename T>
    T& as() noexcept {
        return *reinterpret_cast<T*>(m_raw.get());
    }

    template<typename T>
    const T& as() const noexcept {
        return *reinterpret_cast<T*>(m_raw.get());
    }

    std::string print() const {
        switch (m_type) {
        case Type::String:
            return "String(\"" + as<std::string>() + "\")";
            break;
        case Type::Number:
            return "Number(" + std::to_string(as<double>()) + ")";
            break;
        case Type::Bool:
            return std::string("Bool(") + (as<bool>() ? "true" : "false") + ")";
            break;
        case Type::Array:
            return "Array";
            break;
        case Type::None:
            return "None";
            break;
        default:
            return "Unknown";
        }
    }
};

#endif // VARIANT_H
