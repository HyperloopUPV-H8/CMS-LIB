#pragma once

#include <cstdint>

namespace CMS::Messages {

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

};  // namespace CMS::Messages