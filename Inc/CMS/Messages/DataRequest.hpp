#pragma once

#include <cstdint>

#include "CMS/Types/Flags.hpp"
#include "CMS/Types/Identifiers.hpp"
#include "CMS/Types/Values.hpp"

namespace CMS::Messages {

struct DataRequest {
    Types::DataRequestFlags tx_config;
    Types::TxCycle_t tx_cycle;
};

struct ModuleState {
    Types::GeneralModuleStateFlags general_state;
    Types::ModuleVoltage module_voltage;
    Types::Temperature max_temperature;
    Types::Temperature min_temperature;
    Types::ErrorCode error_code;
    uint8_t live_count;
};

struct CellState {
    Types::CellVoltage max_voltage;
    Types::CellVoltage min_voltage;
    Types::CellVoltage avg_voltage;
    uint8_t max_voltage_cell;
    uint8_t min_voltage_cell;
};

struct StateDetails {
    uint16_t _reserved_;
    Types::WarningFlags warnings;
    Types::ErrorFlags errors;
    Types::SystemErrorFlags system_errors;
};

struct CellVoltages {
    uint8_t message_number;  // 0 - 15
    uint8_t norm;            // = 1
    Types::CellVoltage voltages[3];
};

};  // namespace CMS::Messages