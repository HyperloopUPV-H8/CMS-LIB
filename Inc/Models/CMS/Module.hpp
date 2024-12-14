#pragma once
#include <cstdint>

#include "CMS/Messages.hpp"

namespace CMS {

class Module {
   public:
    float module_voltage{};

    std::array<float, 48> cell_voltages{};
    float max_cell_voltage{}, min_cell_voltage{}, mean_cell_voltage{};

    float max_temperature, min_temperature;

    GeneralModuleState general_state{GeneralModuleState::NoState};
    ErrorCode last_error_code{ErrorCode::NoError};
    CommandError command_error{
        .error{ErrorCode::NoError},
        .command{ServiceCommand::NoCommand},
    };
    WarningFlags warnings{WarningFlags::NoWarning};
    ErrorFlags errors{ErrorFlags::NoError};
    SystemErrorFlags system_errors{SystemErrorFlags::NoError};

    uint8_t live_count{};

    uint8_t module_id{};
    uint32_t serial_number{};

    uint8_t version_high{}, version_low{};
    uint16_t version_build_number{};
    char version_variant{};
    uint8_t version_type_modules{};

    Module() : module_voltage(99) {}

    void update_module_state(Messages::ModuleState module_state) {
        general_state = module_state.general_state;
        module_voltage = static_cast<float>(module_state.module_voltage) / 10;
        if (module_state.max_temperature == 0) {
            max_temperature = 0;
        } else {
            max_temperature =
                static_cast<float>(module_state.max_temperature) - 273.5;
        }
        if (module_state.min_temperature == 0) {
            min_temperature = 0;
        } else {
            min_temperature =
                static_cast<float>(module_state.min_temperature) - 273.5;
        }
        last_error_code = module_state.error_code;
        // TODO: check if this increments to see if data is updated

        live_count = module_state.live_count;
    }

    void update_cell_state(Messages::CellState cell_state) {
        max_cell_voltage =
            static_cast<float>(static_cast<uint32_t>(cell_state.max_voltage)
                               << 14) /
            5;
        cell_voltages[cell_state.max_voltage_cell] =
            static_cast<float>(static_cast<uint32_t>(cell_state.max_voltage)
                               << 14) /
            5;
        min_cell_voltage =
            static_cast<float>(static_cast<uint32_t>(cell_state.min_voltage)
                               << 14) /
            5;
        cell_voltages[cell_state.min_voltage_cell] =
            static_cast<float>(static_cast<uint32_t>(cell_state.min_voltage)
                               << 14) /
            5;
        mean_cell_voltage =
            static_cast<float>(static_cast<uint32_t>(cell_state.mean_voltage)
                               << 14) /
            5;
    }

    void update_state_details(Messages::StateDetails state_details) {
        warnings = state_details.warnings;
        errors = state_details.errors;
        system_errors = state_details.system_errors;
    }

    void update_cells_voltage(Messages::CellsVoltage cells_voltage) {
        // TODO: check cells_voltage.norm = 1 and 0 <=
        // cells_voltage.message_number <= 15

        uint8_t base = cells_voltage.message_number * 3;
        cell_voltages[base + 0] =
            static_cast<float>(static_cast<uint32_t>(cells_voltage.voltages[0])
                               << 14) /
            5;
        cell_voltages[base + 1] =
            static_cast<float>(static_cast<uint32_t>(cells_voltage.voltages[1])
                               << 14) /
            5;
        cell_voltages[base + 2] =
            static_cast<float>(static_cast<uint32_t>(cells_voltage.voltages[2])
                               << 14) /
            5;
    }

    void update_id(Messages::GetID update) {
        module_id = update.module_id;
        serial_number = update.serial_number;
    }

    void update_version(Messages::GetVersion update) {
        version_high = update.version_high;
        version_low = update.version_low;
        version_build_number = update.build_number;
        version_variant = update.variant;
        version_type_modules = update.type_modules;
    }

    void update_command_error(ServiceCommand command, ErrorCode error) {
        command_error.error = error;
        command_error.command = command;
    }
};

}  // namespace CMS
