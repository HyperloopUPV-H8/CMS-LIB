#pragma once
#include <cstdint>

#include "CMS/Module.hpp"

namespace CMS {

template <uint8_t ModuleAmount>
class String {
   public:
    Module modules[ModuleAmount + 1];  // Simplify indexing
};

}  // namespace CMS
