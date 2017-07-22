/*! \file sc2_replay_observer.h
    \brief A client running a replay.
*/
#pragma once

#include "sc2_client.h"

#include <string>

namespace sc2 {

class ReplayControlImp;
class ReplayControlInterface;
class ControlInterface;
struct ReplayInfo;

//! A client for running a replay.
class ReplayObserver : public Client {
public:
    ReplayObserver();
    ~ReplayObserver() override;

    //! Obtains the replay control interface.
    //!< \return The replay control interface.
    ReplayControlInterface* ReplayControl();

    //! Determines if the replay should be filtered out.
    //! \param replay_info Replay information used to decide if the replay should be filtered.
    //!< \return If 'true', the replay will be rejected and not analyzed.
    virtual bool IgnoreReplay(const ReplayInfo& replay_info, uint32_t& player_id);

    void SetControl(ControlInterface* control);

private:
    ReplayControlImp* replay_control_imp_;
};

}
