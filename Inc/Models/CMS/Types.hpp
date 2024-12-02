#pragma once
#include <cstdint>

#include "C++Utilities/EnumClassFlag.hpp"

namespace CMS {

enum class ProtocolNumber : uint8_t {
    DataRequest = 0,
    ModuleStateResponse = 1,
    CellStateResponse = 2,
    StateDetailsResponse = 3,
    CellsVoltageResponse = 4,

    // Protocol number 5 is reserved

    ServiceRequest = 6,
    ServiceResponse = 7,
};

enum class DataRequestFlags : uint16_t {
    EnableAutoTx = 1 << 0,
    SendModuleState = 1 << 1,
    SendCellState = 1 << 2,
    SendDetailedState = 1 << 3,
    SendCellsVoltage = 1 << 4,
};
ENUM_CLASS_FLAGS(DataRequestFlags, uint16_t)
// 1 inc = 10ms, min 50ms, max 10s
using TxCycle = uint16_t;

enum class ServiceCommand : uint8_t {
    NoCommand = 0,
    GetID = 0x1,
    GetVersion = 0x20,
    RestartModule = 0x21,
};

enum class ServiceType : uint8_t {
    Regular = 0xA,
    ErrorReport = 0x8A,
};

enum class RestartMode : uint8_t {
    NormalRestart = 0,  // Could be any value other than 0x01
    BootloaderRestart = 0x01,
};

// 1 inc = (5 >> 14)V
using CellVoltage = uint16_t;
// 1 inc = 100mv
using ModuleVoltage = uint16_t;
// 1 inc = 1K
// 0 = 0ºC
using Temperature = int8_t;

enum class GeneralModuleState : uint16_t {
    NoState = 0,
    ActiveCellMeasurement = 1 << 0,
    ActiveCellBalancing = 1 << 1,
    ActiveWarning = 1 << 2,
    ActiveError = 1 << 3
};
ENUM_CLASS_FLAGS(GeneralModuleState, uint16_t)

enum class ErrorCode : uint8_t {
    NoError = 0,
    WrongSize = 0x6,
    ValueTooHigh = 0x7,
    ValueTooSmall = 0x8,
    WriteToReadOnly = 0x9,
    PermissionDenied = 0xB,
    WriteOnWrongState = 0xE,
    InvalidData = 0xF,
    CommandNotExist = 0x20,
};

struct CommandError {
    ErrorCode error;
    ServiceCommand command;
};

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

// 0bPPPSSSSMMMM (P: protocol, S: string, M: module), 11 bit ID
struct IDParts {
    ProtocolNumber protocol;
    uint8_t string;
    uint8_t module;
};

inline uint32_t get_id(CMS::ProtocolNumber protocol, uint8_t string,
                       uint8_t module) {
    return (static_cast<uint32_t>(static_cast<uint8_t>(protocol) & 0b00000111)
            << 8) |
           (static_cast<uint32_t>(string & 0b00001111) << 4) |
           static_cast<uint32_t>(module & 0b00001111);
}

inline IDParts get_id_parts(uint32_t id) {
    return IDParts{
        .protocol{static_cast<ProtocolNumber>((id >> 8) & 0b00000111)},
        .string{static_cast<uint8_t>((id >> 4) & 0b00001111)},
        .module{static_cast<uint8_t>(id & 0b00001111)}};
}

}  // namespace CMS
