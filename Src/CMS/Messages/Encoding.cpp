#include "CMS/Messages/Encoding.hpp"

#define ENCODE_UINT8(value) static_cast<uint8_t>(value)

#define ENCODE_UINT16(value)                    \
    ENCODE_UINT8(static_cast<uint16_t>(value)), \
        ENCODE_UINT8(static_cast<uint16_t>(value) >> 8)

#define ENCODE_UINT32(value)                     \
    ENCODE_UINT16(static_cast<uint32_t>(value)), \
        ENCODE_UINT16(static_cast<uint32_t>(value) >> 16)

using namespace CMS;
using namespace CMS::Messages;

CanPacket Messages::encode_data_request(uint8_t string, uint8_t module,
                                        DataRequest request) {
    return CanPacket{
        .id{Types::CanID(Types::ProtocolNumber::DataRequest, string, module)
                .encode()},
        .length{4},
        .payload{ENCODE_UINT16(request.tx_config),
                 ENCODE_UINT16(request.tx_cycle)},
    };
}

CanPacket Messages::encode_get_id_command(uint8_t string, uint8_t module) {
    return CanPacket{
        .id{Types::CanID(Types::ProtocolNumber::ServiceRequest, string + 1,
                         module + 1)
                .encode()},
        .length{8},
        .payload{ENCODE_UINT8(Types::ServiceType::Regular),
                 ENCODE_UINT8(Types::ServiceCommand::GetID), 0, 0, 0, 0, 0, 0},
    };
}

CanPacket Messages::encode_get_version_command(uint8_t string, uint8_t module) {
    return CanPacket{
        .id{Types::CanID(Types::ProtocolNumber::ServiceRequest, string + 1,
                         module + 1)
                .encode()},
        .length{8},
        .payload{ENCODE_UINT8(Types::ServiceType::Regular),
                 ENCODE_UINT8(Types::ServiceCommand::GetVersion), 0, 0, 0, 0, 0,
                 0},
    };
}

CanPacket Messages::encode_restart_command(uint8_t string, uint8_t module,
                                           Types::RestartMode mode) {
    return CanPacket{
        .id{Types::CanID(Types::ProtocolNumber::ServiceRequest, string + 1,
                         module + 1)
                .encode()},
        .length{8},
        .payload{ENCODE_UINT8(Types::ServiceType::Regular),
                 ENCODE_UINT8(Types::ServiceCommand::RestartModule),
                 ENCODE_UINT8(mode), 0, 0, 0, 0, 0},
    };
}

#define DECODE_UINT8(value) static_cast<uint8_t>(value)

#define DECODE_UINT16(value0, value1)                                          \
    static_cast<uint16_t>((static_cast<uint16_t>(DECODE_UINT8(value1)) << 8) | \
                          static_cast<uint16_t>(DECODE_UINT8(value0)))

#define DECODE_UINT32(value0, value1, value2, value3)                  \
    static_cast<uint32_t>(                                             \
        (static_cast<uint32_t>(DECODE_UINT16(value2, value3)) << 16) | \
        static_cast<uint32_t>(DECODE_UINT16(value0, value1)))

GetID Messages::decode_get_id_response(const PacketPayload& payload) {
    return GetID{
        .module_id{DECODE_UINT8(payload[2])},
        .serial_number{
            DECODE_UINT32(payload[3], payload[4], payload[5], payload[6])},
    };
}

GetVersion Messages::decode_get_version_response(const PacketPayload& payload) {
    return GetVersion{
        .version_high{DECODE_UINT8(payload[2])},
        .version_low{DECODE_UINT8(payload[3])},
        .build_number{DECODE_UINT16(payload[4], payload[5])},
        .variant{static_cast<char>(DECODE_UINT8(payload[6]))},
        .type_modules{DECODE_UINT8(payload[7])},
    };
}

ModuleState Messages::decode_module_state_response(
    const PacketPayload& payload) {
    return ModuleState{
        .general_state{static_cast<Types::GeneralModuleStateFlags>(
            DECODE_UINT16(payload[0], payload[1]))},
        .module_voltage{DECODE_UINT16(payload[2], payload[3])},
        .max_temperature{
            static_cast<Types::Temperature_t>(DECODE_UINT8(payload[4]))},
        .min_temperature{
            static_cast<Types::Temperature_t>(DECODE_UINT8(payload[5]))},
        .error_code{static_cast<Types::ErrorCode>(DECODE_UINT8(payload[6]))},
        .live_count{DECODE_UINT8(payload[7])},

    };
}

CellState Messages::decode_cell_state_response(const PacketPayload& payload) {
    return CellState{
        .max_voltage{DECODE_UINT16(payload[0], payload[1])},
        .min_voltage{DECODE_UINT16(payload[2], payload[3])},
        .avg_voltage{DECODE_UINT16(payload[4], payload[5])},
        .max_voltage_cell{DECODE_UINT8(payload[6])},
        .min_voltage_cell{DECODE_UINT8(payload[7])},
    };
}

StateDetails Messages::decode_state_details_response(
    const PacketPayload& payload) {
    return StateDetails{
        ._reserved_{0},
        .warnings{static_cast<Types::WarningFlags>(
            DECODE_UINT16(payload[2], payload[3]))},
        .errors{static_cast<Types::ErrorFlags>(
            DECODE_UINT16(payload[4], payload[5]))},
        .system_errors{static_cast<Types::SystemErrorFlags>(
            DECODE_UINT16(payload[6], payload[7]))},
    };
}

CellVoltages Messages::decode_cell_voltages_response(
    const PacketPayload& payload) {
    return CellVoltages{
        .message_number{DECODE_UINT8(payload[0])},
        .norm{DECODE_UINT8(payload[1])},
        .voltages{
            DECODE_UINT16(payload[2], payload[3]),
            DECODE_UINT16(payload[4], payload[5]),
            DECODE_UINT16(payload[6], payload[7]),
        },
    };
}

Types::ServiceType Messages::decode_service_type(const PacketPayload& payload) {
    return static_cast<Types::ServiceType>(DECODE_UINT8(payload[0]));
}

Types::ServiceCommand Messages::decode_service_command(
    const PacketPayload& payload) {
    return static_cast<Types::ServiceCommand>(DECODE_UINT8(payload[1]));
}

Types::ErrorCode Messages::decode_service_error(const PacketPayload& payload) {
    return static_cast<Types::ErrorCode>(DECODE_UINT8(payload[7]));
}