#pragma once

#include <cstdint>

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

namespace CMS::Types {

enum class DataRequestFlags : uint16_t {
    EnableAutoTx = 1 << 0,
    SendModuleState = 1 << 1,
    SendCellState = 1 << 2,
    SendDetailedState = 1 << 3,
    SendCellsVoltage = 1 << 4,
};
ENUM_CLASS_FLAGS(DataRequestFlags, uint16_t)

enum class GeneralModuleStateFlags : uint16_t {
    NoState = 0,
    ActiveCellMeasurement = 1 << 0,
    ActiveCellBalancing = 1 << 1,
    ActiveWarning = 1 << 2,
    ActiveError = 1 << 3
};
ENUM_CLASS_FLAGS(GeneralModuleStateFlags, uint16_t)

enum class WarningFlags : uint16_t {
    NoWarning = 0,
    ModuleVoltage = 1 << 0,
    CellVoltage = 1 << 1,
    CellTemperature = 1 << 2,
    CellVoltageDelta = 1 << 3,

    // Bits 4, 5 and 6 undefined

    CellCalibration = 1 << 7
};
ENUM_CLASS_FLAGS(WarningFlags, uint16_t)

enum class ErrorFlags : uint16_t {
    NoError = 0,
    ModuleVoltage = 1 << 0,
    CellVoltage = 1 << 1,
    CellTemperature = 1 << 2,
    CellVoltageDelta = 1 << 3,

    // Bits 4 and 5 undefined

    ImpossibleVoltage = 1 << 6,

    // Bit 7 undefined

    MissingSerialNumberOrPCBNumber = 1 << 8,
};
ENUM_CLASS_FLAGS(ErrorFlags, uint16_t)

enum class SystemErrorFlags : uint16_t {
    NoError = 0,
    InternalWatchdogTriggered = 1 << 0,
    ExternalWatchdogTriggered = 1 << 1,

    // Bits 2, 3, 4, 5, 6, 7, 8 and 9 undefined

    InitializationParameter = 1 << 10,
    InitializationEEPROM = 1 << 11,
    ChecksumEEPROM = 1 << 12,
    AccessEEPROM = 1 << 13,
};
ENUM_CLASS_FLAGS(SystemErrorFlags, uint16_t)

}  // namespace CMS::Types