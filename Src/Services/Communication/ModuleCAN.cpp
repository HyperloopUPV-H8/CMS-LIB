#include "Services/Communication/ModuleCAN.hpp"

#include "CMS/Messages.hpp"
#include "Communication/FDCAN/FDCAN.hpp"

template <uint8_t StringAmount, uint8_t ModulesPerString>
bool ModuleCAN<StringAmount, ModulesPerString>::read_next() {
    static FDCAN::Packet last_packet{};
    static CMS::IDParts last_id{};

    if (!FDCAN::read(fdcan_instance, &last_packet)) {
        return false;
    }
    last_id = CMS::get_id_parts(last_packet.identifier);

    if (last_id.string > StringAmount || last_id.module > ModulesPerString ||
        last_packet.data_length != FDCAN::DLC::BYTES_8) {
        return false;
    }

    switch (last_id.protocol) {
        case CMS::ProtocolNumber::ModuleStateResponse:
            CMS::Messages::ModuleState module_state{
                .general_state{static_cast<CMS::GeneralModuleState>(
                    (static_cast<uint16_t>(last_packet.rx_data[1]) << 8) |
                    last_packet.rx_data[2])},
                .module_voltage{static_cast<CMS::ModuleVoltage>(
                    (static_cast<uint16_t>(last_packet.rx_data[3]) << 8) |
                    last_packet.rx_data[2])},
                .max_temperature{
                    static_cast<CMS::Temperature>(last_packet.rx_data[4])},
                .min_temperature{
                    static_cast<CMS::Temperature>(last_packet.rx_data[5])},
                .error_code{
                    static_cast<CMS::ErrorCode>(last_packet.rx_data[6])},
                .live_count{last_packet.rx_data[7]},
            };

            strings[last_id.string].modules[last_id.module].update_module_state(
                module_state);
            break;

        case CMS::ProtocolNumber::CellStateResponse:
            CMS::Messages::CellState cell_state{
                .max_voltage{static_cast<CMS::CellVoltage>(
                    (static_cast<uint16_t>(last_packet.rx_data[1]) << 8) |
                    last_packet.rx_data[0])},
                .min_voltage{static_cast<CMS::CellVoltage>(
                    (static_cast<uint16_t>(last_packet.rx_data[3]) << 8) |
                    last_packet.rx_data[2])},
                .mean_voltage{static_cast<CMS::CellVoltage>(
                    (static_cast<uint16_t>(last_packet.rx_data[5]) << 8) |
                    last_packet.rx_data[4])},
                .max_voltage_cell{last_packet.rx_data[6]},
                .min_voltage_cell{last_packet.rx_data[7]},
            };

            strings[last_id.string].modules[last_id.module].update_cell_state(
                cell_state);
            break;

        case CMS::ProtocolNumber::StateDetailsResponse:
            CMS::Messages::StateDetails state_details{
                ._reserved_{0},
                .warnings{static_cast<CMS::WarningFlags>(
                    (static_cast<uint16_t>(last_packet.rx_data[3]) << 8) |
                    last_packet.rx_data[2])},
                .errors{static_cast<CMS::ErrorFlags>(
                    (static_cast<uint16_t>(last_packet.rx_data[5]) << 8) |
                    last_packet.rx_data[4])},
                .system_errors{static_cast<CMS::SystemErrorFlags>(
                    (static_cast<uint16_t>(last_packet.rx_data[7]) << 8) |
                    last_packet.rx_data[6])},
            };

            strings[last_id.string]
                .modules[last_id.module]
                .update_state_details(state_details);
            break;

        case CMS::ProtocolNumber::CellsVoltageResponse:
            CMS::Messages::CellsVoltage cells_voltage{
                .message_number{last_packet.rx_data[0]},
                .norm{last_packet.rx_data[1]},
                .voltages{
                    static_cast<CMS::CellVoltage>(
                        (static_cast<uint16_t>(last_packet.rx_data[3]) << 8) |
                        last_packet.rx_data[2]),
                    static_cast<CMS::CellVoltage>(
                        (static_cast<uint16_t>(last_packet.rx_data[5]) << 8) |
                        last_packet.rx_data[4]),
                    static_cast<CMS::CellVoltage>(
                        (static_cast<uint16_t>(last_packet.rx_data[7]) << 8) |
                        last_packet.rx_data[6]),
                }};

            strings[last_id.string]
                .modules[last_id.module]
                .update_cells_voltage(cells_voltage);
            break;

        case CMS::ProtocolNumber::ServiceResponse:
            CMS::ServiceType service_type{
                static_cast<CMS::ServiceType>(last_packet.rx_data[0])};
            CMS::ServiceCommand command{
                static_cast<CMS::ServiceCommand>(last_packet.rx_data[1])};

            CMS::ErrorCode error{
                static_cast<CMS::ErrorCode>(last_packet.rx_data[7])};

            if (service_type == CMS::ServiceType::ErrorReport) {
                strings[last_id.string]
                    .modules[last_id.module]
                    .update_command_error(command, error);

            } else if (service_type == CMS::ServiceType::Regular) {
                switch (command) {
                    case CMS::ServiceCommand::GetID:
                        CMS::Messages::GetID id_update{
                            .module_id{last_packet.rx_data[2]},
                            .serial_number{static_cast<uint32_t>(
                                (static_cast<uint32_t>(last_packet.rx_data[6])
                                 << 24) |
                                (static_cast<uint32_t>(last_packet.rx_data[5])
                                 << 16) |
                                (static_cast<uint16_t>(last_packet.rx_data[4])
                                 << 8) |
                                last_packet.rx_data[3])}};

                        strings[last_id.string]
                            .modules[last_id.module]
                            .update_id(id_update);
                        break;

                    case CMS::ServiceCommand::GetVersion:
                        CMS::Messages::GetVersion version_update{
                            .version_high{last_packet.rx_data[2]},
                            .version_low{last_packet.rx_data[3]},
                            .build_number{static_cast<uint16_t>(
                                (static_cast<uint16_t>(last_packet.rx_data[5])
                                 << 8) |
                                last_packet.rx_data[4])},
                            .variant{static_cast<char>(last_packet.rx_data[6])},
                            .type_modules{last_packet.rx_data[7]},
                        };

                        strings[last_id.string]
                            .modules[last_id.module]
                            .update_version(version_update);
                        break;

                    default:
                        return false;
                }
            }

            return false;
        default:
            return false;
    }

    return true;
}

template <uint8_t StringAmount, uint8_t ModulesPerString>
bool ModuleCAN<StringAmount, ModulesPerString>::request_data(
    uint8_t string, uint8_t module, CMS::DataRequestFlags tx_config,
    CMS::TxCycle tx_cycle) {
    if (string > StringAmount || module > ModulesPerString) {
        return false;
    }

    uint16_t raw_tx_config = static_cast<uint16_t>(tx_config);
    uint16_t raw_tx_cycle = static_cast<uint16_t>(tx_cycle);

    const char message[4] = {
        static_cast<char>(raw_tx_config),
        static_cast<char>(raw_tx_config >> 8),
        static_cast<char>(raw_tx_cycle),
        static_cast<char>(raw_tx_cycle >> 8),
    };

    // Protocol Number 0
    uint32_t id{CMS::get_id(CMS::ProtocolNumber::DataRequest, string, module)};

    return FDCAN::transmit(fdcan_instance, id, message, FDCAN::DLC::BYTES_4);
}

template <uint8_t StringAmount, uint8_t ModulesPerString>
bool ModuleCAN<StringAmount, ModulesPerString>::request_service(
    uint8_t string, uint8_t module, CMS::ServiceCommand command) {
    if (command == CMS::ServiceCommand::RestartModule ||
        command == CMS::ServiceCommand::NoCommand || string > StringAmount ||
        module > ModulesPerString) {
        return false;
    }

    const char message[8] = {
        static_cast<char>(CMS::ServiceType::Regular),
        static_cast<char>(command),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };  // none of the commands change the message arguments

    uint32_t id{
        CMS::get_id(CMS::ProtocolNumber::ServiceRequest, string, module)};

    return FDCAN::transmit(fdcan_instance, id, message, FDCAN::DLC::BYTES_8);
}

template <uint8_t StringAmount, uint8_t ModulesPerString>
bool ModuleCAN<StringAmount, ModulesPerString>::request_restart(
    uint8_t string, uint8_t module, CMS::RestartMode mode) {
    const char message[8] = {
        static_cast<char>(CMS::ServiceType::Regular),
        static_cast<char>(CMS::ServiceCommand::RestartModule),
        static_cast<char>(mode),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    uint32_t id{
        CMS::get_id(CMS::ProtocolNumber::ServiceRequest, string, module)};

    return FDCAN::transmit(fdcan_instance, id, message, FDCAN::DLC::BYTES_8);
}