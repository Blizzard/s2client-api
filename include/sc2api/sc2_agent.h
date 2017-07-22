/* \file sc2_agent.h
   \brief The base class for user defined bots.

The Agent class provides a user with most access into SC2 state and unit manipulation.
A user will issue actions to units via the ActionInterface, examine game state with the
ObservationInterface, issue blocking commands (such as pathing queries) via the UnitQueryInterface
and override library provided events in ClientEvents.
*/

#pragma once

#include "sc2api/sc2_client.h"

namespace sc2 {

class AgentControlImp;
class ActionInterface;
class ActionFeatureLayerInterface;
class AgentControlInterface;

//! The base class for user defined bots.
class Agent : public Client {
public:
    Agent();
    ~Agent() override;

    //! Interface for issuing actions to units. Actions should be batched via the UnitCommand functions
    //! then eventually dispatched with SendActions. If you are stepping the simulation yourself the Step
    //! will automatically call SendActions. If your bot is running in real time you must call SendActions yourself.
    //!< \return The raw (basic) action interface.
    ActionInterface* Actions();

    //! Interface for issuing actions in feature layers.
    //!< \return The feature layer action interface.
    ActionFeatureLayerInterface* ActionsFeatureLayer();

    //! The AgentControlInterface is only currently used for restarting a game.
    //! For internal use.
    //!< \return The agent control interface.
    AgentControlInterface* AgentControl();

private:
    AgentControlImp* agent_control_imp_;
};

}
