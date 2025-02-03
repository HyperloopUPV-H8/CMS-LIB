#pragma once

#include <cstdint>
#include <span>

#include "CMS/Messages/DataRequest.hpp"
#include "CMS/Messages/ServiceCommand.hpp"

namespace CMS::Messages {

using PacketPayload = std::array<uint8_t, 64>;

struct CanPacket {
    uint32_t id;
    uint8_t length;
    PacketPayload payload;
};

CanPacket encode_data_request(uint8_t string, uint8_t module,
                              DataRequest request);

CanPacket encode_get_id_command(uint8_t string, uint8_t module);

CanPacket encode_get_version_command(uint8_t string, uint8_t module);

CanPacket encode_restart_command(uint8_t string, uint8_t module,
                                 Types::RestartMode mode);

GetID decode_get_id_response(const PacketPayload& payload);

GetVersion decode_get_version_response(const PacketPayload& payload);

ModuleState decode_module_state_response(const PacketPayload& payload);

CellState decode_cell_state_response(const PacketPayload& payload);

StateDetails decode_state_details_response(const PacketPayload& payload);

CellVoltages decode_cell_voltages_response(const PacketPayload& payload);

Types::ServiceType decode_service_type(const PacketPayload& payload);

Types::ServiceCommand decode_service_command(const PacketPayload& payload);

Types::ErrorCode decode_service_error(const PacketPayload& payload);

};  // namespace CMS::Messages