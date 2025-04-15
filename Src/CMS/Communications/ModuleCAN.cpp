#include "CMS/Communications/ModuleCAN.hpp"

#include <utility>

#include "CMS/Config.hpp"

using namespace CMS::Communications;

CMS::Messages::CanPacket ModuleCAN::last_packet{};

bool ModuleCAN::update() {
    bool received_something = receive(last_packet);
    if (!received_something) return false;

    Types::CanID id(last_packet.id);

    switch (id.protocol) {
        case Types::ProtocolNumber::ModuleStateResponse:
            return update_module_state(
                id.string - 1, id.module - 1,
                std::move(Messages::decode_module_state_response(
                    last_packet.payload)));
        case Types::ProtocolNumber::CellStateResponse:
            return update_cell_state(
                id.string - 1, id.module - 1,
                std::move(
                    Messages::decode_cell_state_response(last_packet.payload)));
        case Types::ProtocolNumber::StateDetailsResponse:
            return update_state_details(
                id.string - 1, id.module - 1,
                std::move(Messages::decode_state_details_response(
                    last_packet.payload)));
        case Types::ProtocolNumber::CellsVoltageResponse:
            return update_cell_voltages(
                id.string - 1, id.module - 1,
                std::move(Messages::decode_cell_voltages_response(
                    last_packet.payload)));
        case Types::ProtocolNumber::ServiceResponse:
            switch (Messages::decode_service_type(last_packet.payload)) {
                case Types::ServiceType::Regular:
                    switch (
                        Messages::decode_service_command(last_packet.payload)) {
                        case Types::ServiceCommand::GetID:
                            return update_id(
                                id.string - 1, id.module - 1,
                                std::move(Messages::decode_get_id_response(
                                    last_packet.payload)));
                        case Types::ServiceCommand::GetVersion:
                            return update_version(
                                id.string - 1, id.module - 1,
                                std::move(Messages::decode_get_version_response(
                                    last_packet.payload)));
                            // TODO: check reset command response
                        default:
                            return false;
                    }
                case Types::ServiceType::ErrorReport:
                    system.strings[id.string - 1][id.module - 1]
                        .last_error_code =
                        Messages::decode_service_error(last_packet.payload);
                    return true;
                default:
                    return false;
            }
            break;
        default:
            return false;
    }
}

bool ModuleCAN::start_module(uint8_t string, uint8_t module,
                             Types::TxCycle_t tx_cycle) {
    bool id_success = request_id(string, module);
    bool version_success = request_version(string, module);
    bool data_success =
        request_data(string, module,
                     Types::DataRequestFlags::SendModuleState |
#if FORCE_REQUEST_CELL_VOLTAGE
                         Types::DataRequestFlags::SendCellState |
                         Types::DataRequestFlags::SendCellsVoltage |
#endif
                         Types::DataRequestFlags::SendDetailedState |
                         Types::DataRequestFlags::EnableAutoTx,
                     tx_cycle);

    communication_state[string][module].configured_tx = tx_cycle;
#if FORCE_REQUEST_CELL_VOLTAGE
    communication_state[string][module].sending_cell_voltages = data_success;
#endif

    return id_success && version_success && data_success;
}

bool ModuleCAN::restart_module(uint8_t string, uint8_t module) {
    return request_restart(string, module, Types::RestartMode::NormalRestart);
}

bool ModuleCAN::change_module_tx_cycle(uint8_t string, uint8_t module,
                                       Types::TxCycle_t new_cycle) {
    bool data_success =
        request_data(string, module,
                     Types::DataRequestFlags::SendModuleState |
#if FORCE_REQUEST_CELL_VOLTAGE
                         Types::DataRequestFlags::SendCellState |
                         Types::DataRequestFlags::SendCellsVoltage |
#endif
                         Types::DataRequestFlags::SendDetailedState |
                         Types::DataRequestFlags::EnableAutoTx,
                     new_cycle);

    communication_state[string][module].configured_tx = new_cycle;
#if FORCE_REQUEST_CELL_VOLTAGE
    communication_state[string][module].sending_cell_voltages = data_success;
#endif

    return data_success;
}

bool ModuleCAN::request_data(uint8_t string, uint8_t module,
                             Types::DataRequestFlags request,
                             Types::TxCycle_t tx_cycle) {
    return transmit(std::move(
        Messages::encode_data_request(string, module, {request, tx_cycle})));
}

bool ModuleCAN::request_id(uint8_t string, uint8_t module) {
    return transmit(std::move(Messages::encode_get_id_command(string, module)));
}

bool ModuleCAN::request_version(uint8_t string, uint8_t module) {
    return transmit(
        std::move(Messages::encode_get_version_command(string, module)));
}

bool ModuleCAN::request_restart(uint8_t string, uint8_t module,
                                Types::RestartMode mode) {
    return transmit(
        std::move(Messages::encode_restart_command(string, module, mode)));
}

bool ModuleCAN::update_module_state(uint8_t string, uint8_t module,
                                    Messages::ModuleState &&response) {
    Data::Module &data{system.strings[string][module]};

#if CHECK_LIVE_COUNT
    if (response.live_count != data.live_count++) return false;
#else
    data.live_count = response.live_count;
#endif

    data.general_state = response.general_state;
    data.last_error_code = response.error_code;

    system.total_voltage_volts -= data.module_voltage.volts;
    data.module_voltage = response.module_voltage;
    system.total_voltage_volts += data.module_voltage.volts;

    data.max_temperature = response.max_temperature;
    data.min_temperature = response.min_temperature;

#if FORCE_REQUEST_CELL_VOLTAGE
#else
    ModuleCommunicationState &state{communication_state[string][module]};
    if (data.module_voltage.volts >= MODULE_VOLTAGE_FOR_CELL_VOLTAGE &&
        !state.sending_cell_voltages) {
        state.sending_cell_voltages =
            request_data(string, module,
                         Types::DataRequestFlags::SendModuleState |
                             Types::DataRequestFlags::SendCellState |
                             Types::DataRequestFlags::SendDetailedState |
                             Types::DataRequestFlags::SendCellsVoltage |
                             Types::DataRequestFlags::EnableAutoTx,
                         state.configured_tx);
    } else if (state.sending_cell_voltages) {
        state.sending_cell_voltages =
            !request_data(string, module,
                          Types::DataRequestFlags::SendModuleState |
                              Types::DataRequestFlags::SendDetailedState |
                              Types::DataRequestFlags::EnableAutoTx,
                          state.configured_tx);
    }
#endif

    return true;
}

bool ModuleCAN::update_state_details(uint8_t string, uint8_t module,
                                     Messages::StateDetails &&response) {
    Data::Module &data{system.strings[string][module]};

    data.error_flags = response.errors;
    data.warning_flags = response.warnings;
    data.system_error_flags = response.system_errors;

    return true;
}

bool ModuleCAN::update_cell_state(uint8_t string, uint8_t module,
                                  Messages::CellState &&response) {
    Data::Module &data{system.strings[string][module]};

    data.max_cell_voltage = response.max_voltage;
    data.min_cell_voltage = response.min_voltage;
    data.avg_cell_voltage = response.avg_voltage;

    data.cell_voltages[response.max_voltage_cell] = data.max_cell_voltage;
    data.cell_voltages[response.min_voltage_cell] = data.min_cell_voltage;

    return true;
}

bool ModuleCAN::update_cell_voltages(uint8_t string, uint8_t module,
                                     Messages::CellVoltages &&response) {
#if CHECK_CELL_VOLTAGE_NORM
    if (response.norm != 1) return false;
#endif

    Data::Module &data{system.strings[string][module]};

    uint8_t offset = response.message_number * 3;
    data.cell_voltages[offset + 0] = response.voltages[0];
    data.cell_voltages[offset + 1] = response.voltages[1];
    data.cell_voltages[offset + 2] = response.voltages[2];

    return true;
}

bool ModuleCAN::update_id(uint8_t string, uint8_t module,
                          Messages::GetID &&response) {
    Data::Module &data{system.strings[string][module]};

    data.module_id = response.module_id;
    data.serial_number = response.serial_number;

    return true;
}

bool ModuleCAN::update_version(uint8_t string, uint8_t module,
                               Messages::GetVersion &&response) {
    Data::Module &data{system.strings[string][module]};

    data.version.high = response.version_high;
    data.version.low = response.version_low;
    data.version.build_number = response.build_number;
    data.version.variant = response.variant;
    data.version.type_modules = response.type_modules;

    return true;
}