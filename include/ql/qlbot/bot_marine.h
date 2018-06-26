#pragma once
#include "sc2api/sc2_agent.h"

class MarineBot : public sc2::Agent {
public:
    uint32_t restarts_;
    MarineBot();
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    void StrategiaUstup(const sc2::Unit*);
    void StrategiaDopredu(const sc2::Unit*);
    void StrategiaUtoc(const sc2::Unit*);
    float GetClosestEnemy(const sc2::Unit*, sc2::Unit*&);
};
