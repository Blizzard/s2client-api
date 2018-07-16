#pragma once
#include "sc2api/sc2_agent.h"
#include "zealot_state.h"
//#include "../build/QLLib/QLLib/QL.h"
#include "qllib/Stav.h"
#include "qllib/QL.h"

using namespace std;
class ZealotBot : public sc2::Agent
{
    double ALPHA;
    double GAMMA;
    double EPSILON;
    uint32_t restarts_;
    const double pi;
    float dmg;
    float hp;
    float shield;
    bool jeVypis;
    int lastAction;
    float reward;
    float global_reward;
    int step;
    ZealotState* zstav_;
    QL* ql_;
    Stav* state_;
    void GetState() const;
    void Triangulate(float speed, float degree, float& x, float& y);
public:
    ZealotBot();
    void Vypis(std::string);
    void OnGameStart() override final;
    void OnStep() override final;
    void OnGameEnd() override final;
    void StrategiaUstup(const sc2::Unit*, const sc2::Units);
    void StrategiaZmenTarget(const sc2::Unit*);
    void StrategiaUtoc(const sc2::Unit*);
    void HladajNepriatela(const sc2::Unit*);
    void UlozNaucene();
    //virtual void OnUnitDestroyed(const sc2::Unit* unit) override;
    string ReportNaKonciHry();
    //vector<int>* DajStav();
    ~ZealotBot();

};
