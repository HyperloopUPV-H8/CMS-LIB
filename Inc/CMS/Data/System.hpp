#pragma once

#include <array>

#include "CMS/Data/Module.hpp"
#include "CMS/Types/Values.hpp"

namespace CMS::Data {

template <uint8_t NumberOfStrings, uint8_t NumberOfModules>
struct System {
    using String = std::array<Module, NumberOfModules>;

    std::array<String, NumberOfStrings> strings{};

    float total_voltage_volts{0.0};
};

};  // namespace CMS::Data