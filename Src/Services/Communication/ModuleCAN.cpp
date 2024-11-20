#include "Services/Communication/ModuleCAN.hpp"

#include "CMS/Messages.hpp"

template <uint8_t S, uint8_t M>
bool CMS::ModuleCAN<S, M>::decode(CMS::Messages::Packet packet) {
    CMS::IDParts id_parts{CMS::get_id_parts(packet.id)};

    if (id_parts.string > S || id_parts.module > M || data_length != 8) {
        return false;
    }

    switch (id_parts.protocol) {
        case CMS::ProtocolNumber::ModuleStateResponse:
            CMS::Messages::ModuleState module_state{
                .general_state{static_cast<CMS::GeneralModuleState>(
                    (static_cast<uint16_t>(packet.data[1]) << 8) |
                    packet.data[0])},
                .module_voltage{static_cast<CMS::ModuleVoltage>(
                    (static_cast<uint16_t>(packet.data[3]) << 8) |
                    packet.data[2])},
                .max_temperature{static_cast<CMS::Temperature>(packet.data[4])},
                .min_temperature{static_cast<CMS::Temperature>(packet.data[5])},
                .error_code{static_cast<CMS::ErrorCode>(packet.data[6])},
                .live_count{packet.data[7]},
            };

            strings[id_parts.string]
                .modules[id_parts.module]
                .update_module_state(module_state);
            break;

        case CMS::ProtocolNumber::CellStateResponse:
            CMS::Messages::CellState cell_state{
                .max_voltage{static_cast<CMS::CellVoltage>(
                    (static_cast<uint16_t>(packet.data[1]) << 8) |
                    packet.data[0])},
                .min_voltage{static_cast<CMS::CellVoltage>(
                    (static_cast<uint16_t>(packet.data[3]) << 8) |
                    packet.data[2])},
                .mean_voltage{static_cast<CMS::CellVoltage>(
                    (static_cast<uint16_t>(packet.data[5]) << 8) |
                    packet.data[4])},
                .max_voltage_cell{packet.data[6]},
                .min_voltage_cell{packet.data[7]},
            };

            strings[id_parts.string].modules[id_parts.module].update_cell_state(
                cell_state);
            break;

        case CMS::ProtocolNumber::StateDetailsResponse:
            CMS::Messages::StateDetails state_details{
                ._reserved_{0},
                .warnings{static_cast<CMS::WarningFlags>(
                    (static_cast<uint16_t>(packet.data[3]) << 8) |
                    packet.data[2])},
                .errors{static_cast<CMS::ErrorFlags>(
                    (static_cast<uint16_t>(packet.data[5]) << 8) |
                    packet.data[4])},
                .system_errors{static_cast<CMS::SystemErrorFlags>(
                    (static_cast<uint16_t>(packet.data[7]) << 8) |
                    packet.data[6])},
            };

            strings[id_parts.string]
                .modules[id_parts.module]
                .update_state_details(state_details);
            break;

        case CMS::ProtocolNumber::CellsVoltageResponse:
            CMS::Messages::CellsVoltage cells_voltage{
                .message_number{packet.data[0]},
                .norm{packet.data[1]},
                .voltages{
                    static_cast<CMS::CellVoltage>(
                        (static_cast<uint16_t>(packet.data[3]) << 8) |
                        packet.data[2]),
                    static_cast<CMS::CellVoltage>(
                        (static_cast<uint16_t>(packet.data[5]) << 8) |
                        packet.data[4]),
                    static_cast<CMS::CellVoltage>(
                        (static_cast<uint16_t>(packet.data[7]) << 8) |
                        packet.data[6]),
                }};

            strings[id_parts.string]
                .modules[id_parts.module]
                .update_cells_voltage(cells_voltage);
            break;

        case CMS::ProtocolNumber::ServiceResponse:
            CMS::ServiceType service_type{
                static_cast<CMS::ServiceType>(packet.data[0])};
            CMS::ServiceCommand command{
                static_cast<CMS::ServiceCommand>(packet.data[1])};

            CMS::ErrorCode error{static_cast<CMS::ErrorCode>(packet.data[7])};

            if (service_type == CMS::ServiceType::ErrorReport) {
                strings[id_parts.string]
                    .modules[id_parts.module]
                    .update_command_error(command, error);

            } else if (service_type == CMS::ServiceType::Regular) {
                switch (command) {
                    case CMS::ServiceCommand::GetID:
                        CMS::Messages::GetID id_update{
                            .module_id{packet.data[2]},
                            .serial_number{static_cast<uint32_t>(
                                (static_cast<uint32_t>(packet.data[6]) << 24) |
                                (static_cast<uint32_t>(packet.data[5]) << 16) |
                                (static_cast<uint16_t>(packet.data[4]) << 8) |
                                packet.data[3])}};

                        strings[id_parts.string]
                            .modules[id_parts.module]
                            .update_id(id_update);
                        break;

                    case CMS::ServiceCommand::GetVersion:
                        CMS::Messages::GetVersion version_update{
                            .version_high{packet.data[2]},
                            .version_low{packet.data[3]},
                            .build_number{static_cast<uint16_t>(
                                (static_cast<uint16_t>(packet.data[5]) << 8) |
                                packet.data[4])},
                            .variant{static_cast<char>(packet.data[6])},
                            .type_modules{packet.data[7]},
                        };

                        strings[id_parts.string]
                            .modules[id_parts.module]
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

template <uint8_t S, uint8_t M>
bool CMS::ModuleCAN<S, M>::encode_data_request(uint8_t string, uint8_t module,
                                               CMS::DataRequestFlags tx_config,
                                               CMS::TxCycle tx_cycle,
                                               CMS::Messages::Packet &packet) {
    if (string > S || module > M) return false;

    uint16_t raw_tx_config = static_cast<uint16_t>(tx_config);
    uint16_t raw_tx_cycle = static_cast<uint16_t>(tx_cycle);

    packet.id = CMS::get_id(CMS::ProtocolNumber::DataRequest, string, module);
    packet.lenght = 4;
    packet.data = {
        static_cast<char>(raw_tx_config),
        static_cast<char>(raw_tx_config >> 8),
        static_cast<char>(raw_tx_cycle),
        static_cast<char>(raw_tx_cycle >> 8),
    };

    return true;
}

template <uint8_t S, uint8_t M>
bool CMS::ModuleCAN<S, M>::encode_service_request(
    uint8_t string, uint8_t module, CMS::ServiceCommand command,
    CMS::Messages::Packet &packet) {
    if (command == CMS::ServiceCommand::RestartModule ||
        command == CMS::ServiceCommand::NoCommand || string > S || module > M) {
        return false;
    }

    packet.id =
        CMS::get_id(CMS::ProtocolNumber::ServiceRequest, string, module);
    packet.length = 8;
    packet.data = {
        static_cast<char>(CMS::ServiceType::Regular),
        static_cast<char>(command),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };  // none of the commands change the message arguments

    return true;
}

template <uint8_t S, uint8_t M>
bool CMS::ModuleCAN<S, M>::encode_restart_request(
    uint8_t string, uint8_t module, CMS::RestartMode mode,
    CMS::Messages::Packet &packet) {
    packet.id =
        CMS::get_id(CMS::ProtocolNumber::ServiceRequest, string, module);
    packet.length = 8;
    packet.data = {
        static_cast<char>(CMS::ServiceType::Regular),
        static_cast<char>(CMS::ServiceCommand::RestartModule),
        static_cast<char>(mode),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    return true;
}
