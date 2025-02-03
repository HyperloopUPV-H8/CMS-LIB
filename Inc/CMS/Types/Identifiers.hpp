#pragma once

#include <cstdint>

namespace CMS::Types {

enum class ProtocolNumber : uint8_t {
    DataRequest = 0,
    ModuleStateResponse = 1,
    CellStateResponse = 2,
    StateDetailsResponse = 3,
    CellsVoltageResponse = 4,
    Reserved = 5,
    ServiceRequest = 6,
    ServiceResponse = 7,
};

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

// 0bPPPSSSSMMMM (P: protocol, S: string, M: module), 11 bit ID
struct CanID {
    ProtocolNumber protocol;
    uint8_t string;
    uint8_t module;

    CanID(uint32_t id)
        : protocol(static_cast<ProtocolNumber>((id >> 8) & 0b00000111)),
          string(static_cast<uint8_t>((id >> 4) & 0b00001111)),
          module(static_cast<uint8_t>(id & 0b00001111)) {}

    CanID(ProtocolNumber protocol, uint8_t string, uint8_t module)
        : protocol(protocol), string(string), module(module) {}

    uint32_t encode() {
        return (static_cast<uint32_t>(static_cast<uint8_t>(protocol) &
                                      0b00000111)
                << 8) |
               (static_cast<uint32_t>(string & 0b00001111) << 4) |
               static_cast<uint32_t>(module & 0b00001111);
    }
};

};  // namespace CMS::Types
