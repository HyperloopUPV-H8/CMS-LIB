#pragma once
#include <cstdint>

#include "CMS/Types.hpp"

namespace CMS::Messages {

struct DataRequest {
    CMS::DataRequestFlags tx_config;
    CMS::TxCycle tx_cycle;
};

struct ModuleState {
    CMS::GeneralModuleState general_state;
    CMS::ModuleVoltage module_voltage;
    CMS::Temperature max_temperature;
    CMS::Temperature min_temperature;
    CMS::ErrorCode error_code;
    uint8_t live_count;
};

struct CellState {
    CMS::CellVoltage max_voltage;
    CMS::CellVoltage min_voltage;
    CMS::CellVoltage mean_voltage;
    uint8_t max_voltage_cell;
    uint8_t min_voltage_cell;
};

struct StateDetails {
    uint16_t _reserved_;
    CMS::WarningFlags warnings;
    CMS::ErrorFlags errors;
    CMS::SystemErrorFlags system_errors;
};

struct CellsVoltage {
    uint8_t message_number;  // 0 - 15
    uint8_t norm;            // = 1
    CMS::CellVoltage voltages[3];
};

struct GetID {
    uint8_t module_id;
    uint32_t serial_number;
};

struct GetVersion {
    uint8_t version_high;
    uint8_t version_low;
    uint16_t build_number;
    char variant;
    uint8_t type_modules;
};

}  // namespace CMS::Messages
