#pragma once

#include <cmath>
#include <cstdint>

namespace CMS::Types {

using TxCycle_t = uint16_t;
using CellVoltage_t = uint16_t;
using ModuleVoltage_t = uint16_t;
using Temperature_t = int8_t;

// min: 50ms, max: 10s, 1 inc = 10ms
constexpr TxCycle_t milliseconds(uint16_t ms) {
    if (ms <= 50) {
        return 5;
    } else if (ms >= 10000) {
        return 1000;
    } else {
        return ms / 10;
    }
}

struct CellVoltage {
    float millivolts{0.0};

    CellVoltage() = default;
    CellVoltage(CellVoltage_t raw) : millivolts(convert(raw)) {}

    CellVoltage& operator=(const float& converted) {
        millivolts = converted;
        return *this;
    }

    CellVoltage& operator=(const CellVoltage_t& raw) {
        millivolts = convert(raw);
        return *this;
    }

   private:
    // 1 inc = 5 / (2 ^ 14) V = 5000 / (2 ^ 14) mV
    inline float convert(const CellVoltage_t& raw) {
        return 5000.0 * std::ldexp(static_cast<float>(raw), -14);
    }
};

struct ModuleVoltage {
    float volts{0.0};

    ModuleVoltage() = default;
    ModuleVoltage(ModuleVoltage_t raw) : volts(convert(raw)) {}

    ModuleVoltage& operator=(const float& converted) {
        volts = converted;
        return *this;
    }

    ModuleVoltage& operator=(const ModuleVoltage_t& raw) {
        volts = convert(raw);
        return *this;
    }

   private:
    // 1 inc = 100 mV = 0.1 V
    inline float convert(const ModuleVoltage_t& raw) {
        return static_cast<float>(raw) / 10.0;
    }
};

struct Temperature {
    float celsius{0.0};

    Temperature() = default;
    Temperature(Temperature_t raw) : celsius(convert(raw)) {}

    Temperature& operator=(const float& converted) {
        celsius = converted;
        return *this;
    }

    Temperature& operator=(const Temperature_t& raw) {
        celsius = convert(raw);
        return *this;
    }

   private:
    // 1 inc = 1K, 0 = 0ºC -> 1 inc = 1ºC
    inline float convert(const Temperature_t& raw) {
        return static_cast<float>(raw);
    }
};

}  // namespace CMS::Types