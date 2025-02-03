#pragma once

#include <array>

#include "CMS/Config.hpp"
#include "CMS/Types/Flags.hpp"
#include "CMS/Types/Identifiers.hpp"
#include "CMS/Types/Values.hpp"

namespace CMS::Data {

struct Module {
    Types::GeneralModuleStateFlags general_state{
        Types::GeneralModuleStateFlags::NoState};
    Types::ModuleVoltage module_voltage{};
    Types::Temperature max_temperature{}, min_temperature{};
    Types::ErrorCode last_error_code{Types::ErrorCode::NoError};
    uint8_t live_count{};

    std::array<Types::CellVoltage, 48> cell_voltages{};
    Types::CellVoltage max_cell_voltage{}, min_cell_voltage{},
        avg_cell_voltage{};

    Types::ErrorFlags error_flags{Types::ErrorFlags::NoError};
    Types::WarningFlags warning_flags{Types::WarningFlags::NoWarning};
    Types::SystemErrorFlags system_error_flags{
        Types::SystemErrorFlags::NoError};

    uint8_t module_id{0};
    uint32_t serial_number{0};

    struct Version {
        uint8_t high{0};
        uint8_t low{0};
        uint16_t build_number{0};
        char variant{'D'};
        uint8_t type_modules{0};
    };

    Version version{};

    Module() {}
};

};  // namespace CMS::Data