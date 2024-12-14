#pragma once
#include <cstdint>

#include "CMS/Module.hpp"

namespace CMS {

class String {
   public:
    std::array<Module, 3 + 1> modules{};  // Simplify indexing
};

}  // namespace CMS
