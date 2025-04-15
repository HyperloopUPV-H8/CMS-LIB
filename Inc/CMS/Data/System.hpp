#pragma once

#include <array>

#include "CMS/Data/Module.hpp"
#include "CMS/Types/Values.hpp"

namespace CMS::Data {

template <uint8_t NumberOfStrings, uint8_t NumberOfModules>
struct System {
    using String = std::array<Module, NumberOfModules>;

    std::array<String, NumberOfModules> strings{};

    float total_voltage_volts{0.0};

    std::array<std::array<std::array<float *, 48>, NumberOfModules>,
               NumberOfStrings>
        all_cells_voltage{};
    std::array<std::array<float *, NumberOfModules>, NumberOfStrings>
        all_module_voltage{};
    std::array<std::array<float *, NumberOfModules>, NumberOfStrings>
        all_max_cell_voltage{};
    std::array<std::array<float *, NumberOfModules>, NumberOfStrings>
        all_min_cell_voltage{};
    std::array<std::array<float *, NumberOfModules>, NumberOfStrings>
        all_avg_cell_voltage{};
    std::array<std::array<float *, NumberOfModules>, NumberOfStrings>
        all_max_temperature{};
    std::array<std::array<float *, NumberOfModules>, NumberOfStrings>
        all_min_temperature{};

    System() {
        for (uint8_t string{0}; string < NumberOfStrings; ++string) {
            for (uint8_t module{0}; module < NumberOfModules; ++module) {
                for (uint8_t cell{0}; cell < 48; ++cell) {
                    all_cells_voltage[string][module][cell] =
                        &strings[string][module].cell_voltages[cell].millivolts;
                }

                all_module_voltage[string][module] =
                    &strings[string][module].module_voltage.volts;

                all_max_cell_voltage[string][module] =
                    &strings[string][module].max_cell_voltage.millivolts;

                all_min_cell_voltage[string][module] =
                    &strings[string][module].min_cell_voltage.millivolts;

                all_avg_cell_voltage[string][module] =
                    &strings[string][module].avg_cell_voltage.millivolts;

                all_max_temperature[string][module] =
                    &strings[string][module].max_temperature.celsius;

                all_min_temperature[string][module] =
                    &strings[string][module].min_temperature.celsius;
            }
        }
    }
};

};  // namespace CMS::Data