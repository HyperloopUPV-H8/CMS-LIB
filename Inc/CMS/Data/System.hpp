#pragma once

#include <array>

#include "CMS/Data/Module.hpp"
#include "CMS/Types/Values.hpp"

namespace CMS::Data {

template <uint8_t NumberOfStrings, uint8_t NumberOfModules>
struct System {
    constexpr static uint8_t number_of_strings{NumberOfStrings - 1};
    constexpr static uint8_t number_of_modules{NumberOfModules - 1};

    using String = std::array<Module, number_of_modules>;

    std::array<String, number_of_strings> strings{};

    float total_voltage_volts{0.0};

    std::array<std::array<std::array<float *, 48>, number_of_modules>,
               number_of_strings>
        all_cells_voltage{};
    std::array<std::array<float *, number_of_modules>, number_of_strings>
        all_module_voltage{};
    std::array<std::array<float *, number_of_modules>, number_of_strings>
        all_max_cell_voltage{};
    std::array<std::array<float *, number_of_modules>, number_of_strings>
        all_min_cell_voltage{};
    std::array<std::array<float *, number_of_modules>, number_of_strings>
        all_avg_cell_voltage{};
    std::array<std::array<float *, number_of_modules>, number_of_strings>
        all_max_temperature{};
    std::array<std::array<float *, number_of_modules>, number_of_strings>
        all_min_temperature{};

    System() {
        for (uint8_t string{1}; string < NumberOfStrings; ++string) {
            for (uint8_t module{1}; module < NumberOfModules; ++module) {
                for (uint8_t cell{0}; cell < 48; ++cell) {
                    all_cells_voltage[string - 1][module - 1][cell] =
                        &strings[string][module].cell_voltages[cell].millivolts;
                }

                all_module_voltage[string - 1][module - 1] =
                    &strings[string][module].module_voltage.volts;

                all_max_cell_voltage[string - 1][module - 1] =
                    &strings[string][module].max_cell_voltage.millivolts;

                all_min_cell_voltage[string - 1][module - 1] =
                    &strings[string][module].min_cell_voltage.millivolts;

                all_avg_cell_voltage[string - 1][module - 1] =
                    &strings[string][module].avg_cell_voltage.millivolts;

                all_max_temperature[string - 1][module - 1] =
                    &strings[string][module].max_temperature.celsius;

                all_min_temperature[string - 1][module - 1] =
                    &strings[string][module].min_temperature.celsius;
            }
        }
    }
};

};  // namespace CMS::Data