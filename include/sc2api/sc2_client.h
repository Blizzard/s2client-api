/*! \file sc2_client.h
    \brief The base class for Agent and ReplayObserver.

The Client and ClientEvents provides most of the functionality a user would be interested in for examining
game state and scripting bots. A user should prefer to inherit from either Agent or ReplayObserver, those classes
both inherit from Client.
*/

#pragma once

#include <string>
#include <vector>
#include <cassert>

#include "sc2api/sc2_typeenums.h"

namespace sc2 {

class ControlImp;
class Unit;
class ObservationInterface;
class QueryInterface;
class DebugInterface;
class ControlInterface;
class ControlImp;

/*! Errors that the api can encounter, if the OnError event in ClientEvents is overwritten it will contain a list of errors encountered. */
enum class ClientError {
    ErrorSC2,
    InvalidAbilityRemap, /*! An ability was improperly mapped to an ability id that doesn't exist. */
    InvalidResponse,     /*! The response does not contain a field that was expected. */
    NoAbilitiesForTag,   /*! The unit does not have any abilities. */
    ResponseNotConsumed, /*! A request was made without consuming the response from the previous request, that puts this library in an illegal state. */
    ResponseMismatch,    /*! The response received from SC2 does not match the request. */
    ConnectionClosed,    /*! The websocket connection has prematurely closed, this could mean starcraft crashed or a websocket timeout has occurred. */
    SC2UnknownStatus,
    SC2AppFailure,       /*! SC2 has either crashed or been forcibly terminated by this library because it was not responding to requests. */
    SC2ProtocolError,    /*! The response from SC2 contains errors, most likely meaning the API was not used in a correct way. */
    SC2ProtocolTimeout,  /*! A request was made and a response was not received in the amount of time given by the timeout. */
};

//! A set of common events a user can override in their derived bot or replay observer class.
class ClientEvents {
public:
    //! Called when a game is started after a load. Fast restarting will not call this.
    virtual void OnGameFullStart() {}

    //! Called when a game is started or restarted.
    virtual void OnGameStart() {}

    //! Called when a game has ended.
    virtual void OnGameEnd() {}

    //! This event will only get called when stepping. It will not get called in a real time game.
    //! In a real time game the user will be responsible for calling GetObservation() via the ObservationInterface.
    virtual void OnStep() {}

    //! Called whenever one of the player's units has been destroyed.
    //!< \param unit The destroyed unit.
    virtual void OnUnitDestroyed(const Unit&) {}

    //! Called when a Unit has been created by the player.
    //!< \param unit The created unit.
    virtual void OnUnitCreated(const Unit&) {}

    //! Called when a unit becomes idle, this will only occur as an event so will only be called when the unit becomes
    //! idle and not a second time. Being idle is defined by having orders in the previous step and not currently having
    //! orders or if it did not exist in the previous step and now does, a unit being created, for instance, will call both
    //! OnUnitCreated and OnUnitIdle if it does not have a rally set.
    //!< \param unit The idle unit.
    virtual void OnUnitIdle(const Unit&) {}

    //! Called when an upgrade is finished, warp gate, ground weapons, baneling speed, etc.
    //!< \param upgrade The completed upgrade.
    virtual void OnUpgradeCompleted(UpgradeID) {}

    //! Called when the unit in the previous step had a build progress less than 1.0 but is greater than or equal to 1.0 in the current step.
    //!< \param unit The constructed unit.
    virtual void OnBuildingConstructionComplete(const Unit&) {}

    //! Called when a nydus is placed.
    virtual void OnNydusDetected() {}

    //! Called when a nuclear launch is detected.
    virtual void OnNuclearLaunchDetected() {}

    //! Called when an enemy unit enters vision from out of fog of war.
    //!< \param unit The unit entering vision.
    virtual void OnUnitEnterVision(const Unit&) {}

    //! Called for various errors the library can encounter. See ClientError enum for possible errors.
    virtual void OnError(const std::vector<ClientError>& /*client_errors*/, const std::vector<std::string>& /*protocol_errors*/ = {}) {}
};

//! The base class for Agent and ReplayObserver.
class Client : public ClientEvents {
public:
    Client();
    virtual ~Client();

    //! The ObservationInterface is used to query game state.
    const ObservationInterface* Observation() const;

    //! The UnitQuery interface is used to issue commands to units.
    QueryInterface* Query();

    //! The DebugInterface allows a derived class to print text, draw primitive shapes and spawn/destroy units.
    DebugInterface* Debug();

    //! The ControlInterface is only meant to be used by the coordinator as it provides functionality for connecting
    //! to Starcraft2, setting up a websocket connection and issuing blocking commands via SC2's protocol.
    ControlInterface* Control();
    const ControlInterface* Control() const;

    void Reset();

private:
    //! Pointer to the control interface.
    ControlImp* control_imp_;
};

//! Helper function used to discover whether a unit is carrying minerals or not. You could use this function in GetUnits
//! to get all units carrying minerals: Units units = GetUnits(Alliance::Self, IsCarryingMinerals);
//!< \param unit The unit.
//!< \param observation Not needed for this function, only exists for Filter parameters, leave nullptr (its default).
//!< \return Returns true if the unit is carrying minerals, false otherwise.
bool IsCarryingMinerals(const Unit& unit);

//! Helper function used to discover whether a unit is carrying vespene or not. You could use this function in GetUnits
//! to get all units carrying vespene: Units units = GetUnits(Alliance::Self, IsCarryingVespene);
//!< \param unit The unit.
//!< \param observation Not needed for this function, only exists for Filter parameters, leave nullptr (its default).
//!< \return Returns true if the unit is carrying vespene, false otherwise.
bool IsCarryingVespene(const Unit& unit);


}
