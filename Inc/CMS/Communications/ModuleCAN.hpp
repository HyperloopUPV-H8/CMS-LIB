#pragma once

#include <array>
#include <functional>

#include "CMS/Data/System.hpp"
#include "CMS/Messages/Encoding.hpp"

namespace CMS::Communications {

class ModuleCAN {
    // Returns true if data was read
    using ReceiveFunction = std::function<bool(Messages::CanPacket &)>;

    // Returns true if data was successfully sent
    using TransmitFunction = std::function<bool(Messages::CanPacket &&)>;

    ReceiveFunction receive;
    TransmitFunction transmit;

    static Messages::CanPacket last_packet;

    struct ModuleCommunicationState {
        bool sending_cell_voltages{false};
        Types::TxCycle_t configured_tx;
    };

    std::array<std::array<ModuleCommunicationState, NUMBER_OF_MODULES + 1>,
               NUMBER_OF_STRINGS + 1>
        communication_state{};

    bool request_data(uint8_t string, uint8_t module,
                      Types::DataRequestFlags request,
                      Types::TxCycle_t tx_cycle);

    bool request_id(uint8_t string, uint8_t module);

    bool request_version(uint8_t string, uint8_t module);

    bool request_restart(uint8_t string, uint8_t module,
                         Types::RestartMode mode);

    bool update_module_state(uint8_t string, uint8_t module,
                             Messages::ModuleState &&response);

    bool update_cell_state(uint8_t string, uint8_t module,
                           Messages::CellState &&response);

    bool update_state_details(uint8_t string, uint8_t module,
                              Messages::StateDetails &&response);

    bool update_cell_voltages(uint8_t string, uint8_t module,
                              Messages::CellVoltages &&response);

    bool update_id(uint8_t string, uint8_t module, Messages::GetID &&response);

    bool update_version(uint8_t string, uint8_t module,
                        Messages::GetVersion &&response);

   public:
    inline static Data::System<NUMBER_OF_STRINGS + 1, NUMBER_OF_MODULES + 1>
        system{};

    ModuleCAN(ReceiveFunction receive, TransmitFunction transmit)
        : receive(receive), transmit(transmit) {}

    bool update();

    bool start_module(uint8_t string, uint8_t module,
                      Types::TxCycle_t tx_cycle = Types::milliseconds(100));

    bool restart_module(uint8_t string, uint8_t module);

    bool change_module_tx_cycle(uint8_t string, uint8_t module,
                                Types::TxCycle_t new_cycle);
};

};  // namespace CMS::Communications