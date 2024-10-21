#pragma once

#include "CMS/Module.hpp"
#include "CMS/String.hpp"
#include "CMS/Types.hpp"

// Protocol between the CMSs and the board
template <uint8_t StringAmount, uint8_t ModulesPerString>
class ModuleCAN {
   private:
    const uint8_t fdcan_instance;

   public:
    CMS::String<ModulesPerString>
        strings[StringAmount + 1];  // Simplify indexing

    ModuleCAN(uint8_t instance) : fdcan_instance(instance) {};

    // Read the next message and update the data with the new information
    bool read_next();

    // Request data from a specific module on a specific string
    bool request_data(uint8_t string, uint8_t module,
                      CMS::DataRequestFlags tx_config, CMS::TxCycle tx_cycle);
    // Broadcast to all modules in a single string a data request
    bool request_data(uint8_t string, CMS::DataRequestFlags tx_config,
                      CMS::TxCycle tx_cycle) {
        return request_data(string, 0, tx_config, tx_cycle);
    }
    // Broadcast to all modules in all strings a data request
    bool request_data(CMS::DataRequestFlags tx_config, CMS::TxCycle tx_cycle) {
        return request_data(0, 0, tx_config, tx_cycle);
    }

    // Request service data of specific module on a specific string
    bool request_service(uint8_t string, uint8_t module,
                         CMS::ServiceCommand command);
    // Broadcast to all modules in a single string a service request
    bool request_service(uint8_t string, CMS::ServiceCommand command) {
        return request_service(string, 0, command);
    }
    // Broadcast to all modules in all strings a service request.
    // Since only the GetID command works on broadcast to all, the command
    // cannot be configured
    bool request_service() {
        return request_service(0, 0, CMS::ServiceCommand::GetID);
    }

    // Request a restart of a specific module on a specific string
    bool request_restart(uint8_t string, uint8_t module, CMS::RestartMode mode);
    // Broadcast to all modules in a single string a restart request
    bool request_restart(uint8_t string) {
        return request_restart(string, 0, CMS::RestartMode::NormalRestart);
    }
    // Broadcast to all modules in all strings a restart request
    bool request_restart() {
        return request_restart(0, 0, CMS::RestartMode::NormalRestart);
    }
};