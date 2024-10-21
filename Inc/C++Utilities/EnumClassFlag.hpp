#pragma once

// Define bitwise operators for an enum class, allowing usage as bitmasks.
#define ENUM_CLASS_FLAGS(Enum, type)                            \
    inline constexpr Enum operator|(Enum Lhs, Enum Rhs) {       \
        return static_cast<Enum>(static_cast<type>(Lhs) |       \
                                 static_cast<type>(Rhs));       \
    }                                                           \
    inline constexpr Enum operator&(Enum Lhs, Enum Rhs) {       \
        return static_cast<Enum>(static_cast<type>(Lhs) &       \
                                 static_cast<type>(Rhs));       \
    }                                                           \
    inline constexpr Enum operator^(Enum Lhs, Enum Rhs) {       \
        return static_cast<Enum>(static_cast<type>(Lhs) ^       \
                                 static_cast<type>(Rhs));       \
    }                                                           \
    inline constexpr Enum operator~(Enum E) {                   \
        return static_cast<Enum>(~static_cast<type>(E));        \
    }                                                           \
    inline Enum& operator|=(Enum& Lhs, Enum Rhs) {              \
        return Lhs = static_cast<Enum>(static_cast<type>(Lhs) | \
                                       static_cast<type>(Rhs)); \
    }                                                           \
    inline Enum& operator&=(Enum& Lhs, Enum Rhs) {              \
        return Lhs = static_cast<Enum>(static_cast<type>(Lhs) & \
                                       static_cast<type>(Rhs)); \
    }                                                           \
    inline Enum& operator^=(Enum& Lhs, Enum Rhs) {              \
        return Lhs = static_cast<Enum>(static_cast<type>(Lhs) ^ \
                                       static_cast<type>(Rhs)); \
    }
