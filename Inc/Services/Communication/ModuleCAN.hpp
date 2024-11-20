#pragma once

#include "CMS/Module.hpp"
#include "CMS/String.hpp"
#include "CMS/Types.hpp"

namespace CMS {
// Protocol between the CMSs and the board
template <uint8_t StringAmount, uint8_t ModulesPerString>
class ModuleCAN {
   public:
    static CMS::String<ModulesPerString>
        strings[StringAmount + 1];  // Simplify indexing

    // Read the next message and update the data with the new information
    static bool decode(Messages::Packet packet);

    // Request data from a specific module on a specific string
    static bool encode_data_request(uint8_t string, uint8_t module,
                                    CMS::DataRequestFlags tx_config,
                                    CMS::TxCycle tx_cycle,
                                    Messages::Packet &packet);
    // Broadcast to all modules in a single string a data request
    static bool encode_data_request(uint8_t string,
                                    CMS::DataRequestFlags tx_config,
                                    CMS::TxCycle tx_cycle,
                                    Messages::Packet &packet) {
        return encode_data_request(string, 0, tx_config, tx_cycle, packet);
    }
    // Broadcast to all modules in all strings a data request
    static bool encode_data_request(CMS::DataRequestFlags tx_config,
                                    CMS::TxCycle tx_cycle,
                                    Messages::Packet &packet) {
        return encode_data_request(0, 0, tx_config, tx_cycle, packet);
    }

    // Request service data of specific module on a specific string
    static bool encode_service_request(uint8_t string, uint8_t module,
                                       CMS::ServiceCommand command,
                                       Messages::Packet &packet);
    // Broadcast to all modules in a single string a service request
    static bool encode_service_request(uint8_t string,
                                       CMS::ServiceCommand command,
                                       Messages::Packet &packet) {
        return encode_service_request(string, 0, command, packet);
    }
    // Broadcast to all modules in all strings a service request.
    // Since only the GetID command works on broadcast to all, the command
    // cannot be configured
    static bool encode_service_request(Messages::Packet &packet) {
        return encode_service_request(0, 0, CMS::ServiceCommand::GetID, packet);
    }

    // Request a restart of a specific module on a specific string
    static bool encode_restart_request(uint8_t string, uint8_t module,
                                       CMS::RestartMode mode,
                                       Messages::Packet &packet);
    // Broadcast to all modules in a single string a restart request
    static bool encode_restart_request(uint8_t string,
                                       Messages::Packet &packet) {
        return encode_restart_request(string, 0,
                                      CMS::RestartMode::NormalRestart, packet);
    }
    // Broadcast to all modules in all strings a restart request
    static bool encode_restart_request(Messages::Packet &packet) {
        return encode_restart_request(0, 0, CMS::RestartMode::NormalRestart,
                                      packet);
    }
};

}  // namespace CMS
