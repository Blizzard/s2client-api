#include "qlbot/bot_zealot.h"
#include "qllib/Stav.h"
#include "qllib/QInitZealot.h"

#include "sc2api/sc2_score.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"
#include "sc2lib/sc2_utils.h"

#include "sc2utils/sc2_manage_process.h"
#include "../../tests/feature_layers_shared.h"

#include <iostream>
#include <limits>
#include <cfloat>


ZealotBot::ZealotBot() :
    restarts_(0), dmg(0), hp(0), shield(0), jeVypis(true), lastAction(0), reward(0), global_reward(0), step(100), pi(atan(1) * 4)
{
    //TODO: upravit bota pre hyperparametre epsilon a alfa (vyber nahodneho stavu a learning rate)
    GAMMA = 0.9;
    ALPHA = 0.05;
    EPSILON = 0.75;
    zstav_ = new ZealotState();
    state_ = new Stav(new vector<int>(2, 0));///TODO NATVRDO nasraaaaaat com to tu ide
    ql_ = new QL(state_, 2,3, new QInitZealot());
    ql_->SetHyperparemeters(ALPHA, GAMMA, EPSILON);
}

void ZealotBot::Vypis(std::string sprava)
{
    if (jeVypis)
    {
        std::cout << sprava << std::endl;
    }
}

void ZealotBot::OnGameStart()
{

    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
    GetState();
    dmg = Observation()->GetScore().score_details.total_damage_dealt.life;
    dmg += Observation()->GetScore().score_details.total_damage_dealt.shields;
    dmg += Observation()->GetScore().score_details.total_damage_dealt.energy;
    reward = dmg;
    global_reward = reward;
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    //cout << Observation()->GetUnitTypeData().at(static_cast<int>(sc2::UNIT_TYPEID::PROTOSS_ZEALOT)).weapons.at(0).damage_ << endl;
    //cout << Observation()->GetUnitTypeData().at(static_cast<int>(sc2::UNIT_TYPEID::PROTOSS_ZEALOT)).movement_speed << endl;
    //cout << Observation()->GetUnitTypeData().at(static_cast<int>(sc2::UNIT_TYPEID::TERRAN_MARINE)).movement_speed << endl;

    if (units.empty())
    {
        return;
    }
    hp = units[0]->health;
    shield = units[0]->shield;
    Debug()->DebugCreateUnit(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, units[0]->pos, Observation()->GetPlayerID());
    Debug()->SendDebug();
}

void ZealotBot::OnStep()
{
    uint32_t game_loop = Observation()->GetGameLoop();
    //Observation()->GetGameInfo().start_locations;
    //Observation()->GetScore().score_details.killed_minerals;
    //OnUnitDestroyed();
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    if (units.empty())
    {
        return;
    }
    Debug()->DebugMoveCamera(units[0]->pos);
    Debug()->SendDebug();
    if (game_loop % step == 0)
    {
        float pomocna = Observation()->GetScore().score_details.total_damage_dealt.life;
        pomocna += Observation()->GetScore().score_details.total_damage_dealt.shields;
        pomocna += Observation()->GetScore().score_details.total_damage_dealt.energy;
        reward = pomocna - dmg;
        global_reward += reward;
        /*
         * Mozna odmena dobuducna (hp, shield)
         *
        if (hp != units[0]->health)
        {
            reward += hp - units[0]->health;
        }
        if (shield != units[0]->shield)
        {
            reward += shield - units[0]->shield;
        }
        */
        ql_->Learn(reward, new Stav(zstav_->to_array()), lastAction, false);//TODO Pozor momentalne to je spravene na tu minihru s dierou ktoru ma obchadzat (natvrdo bohuzial) takze velkosti stavou nesedia

        if (units.empty())
        {
            //this->Vypis(" Nemame jednotky.");
            return;//ak nemame vojakov step sa nedeje (padlo)
        }
        GetState();
        srand(time(NULL));
        int akcia = ql_->ChooseAction(false, this->state_);///TODO tu mu posli stav
        sc2::Units jednotkyNepriatelov = Observation()->GetUnits(sc2::Unit::Enemy);
        if (akcia == 0)
        {
            //Vypis("  Strategia USTUP");
            step = 30;
            this->StrategiaUstup(units[0], jednotkyNepriatelov);
        }
        else if (akcia == 1)
        {
            //Vypis("  Strategia ZMEN TARGET");
            step = 100;
            this->StrategiaZmenTarget(units[0]);
        }
        else
        {
            //Vypis("  Strategia UTOC");
            step = 100;
            this->StrategiaUtoc(units[0]);
        }
        lastAction = akcia;
    }

    /*
    if (game_loop % 100 == 0) {
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    for (auto& it_unit : units) {

    sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo());
    Actions()->UnitCommand(it_unit, sc2::ABILITY_ID::SMART, target);
    }
    }*/
}

void ZealotBot::UlozNaucene()
{
    this->ql_->Save("saveQL.csv");
}
/*
void ZealotBot::OnUnitDestroyed(const sc2::Unit* unit)
{

    auto raw = Observation()->GetRawObservation();

    if (unit->alliance == sc2::Unit::Alliance::Self)
    {
        cout << "Zomrela moja jednotka" << endl;
    }
    if (unit->alliance == sc2::Unit::Enemy)
    {
        cout << "Zomrela superova jednotka" << endl;
        sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
        for (auto unt : units)
        {
            auto range = Observation()->GetUnitTypeData().at(unt->unit_type).weapons.at(0).range;
            if (!unt->orders.empty() && unt->orders[0].ability_id == sc2::ABILITY_ID::ATTACK && unt->orders[0].progress == 0 && unt->orders[0].target_unit_tag == unit->tag)
            {
                cout << range << endl;
                cout << Distance2D(unit->pos, unt->pos) << endl;
                cout << range + unit->radius + unt->radius << endl;
                cout << "Ja som last hitol " << unt->tag <<  endl;
                //Debug()->DebugSphereOut(unt->pos, 1);
                //Debug()->SendDebug();
            }
        }
    }

}*/

void ZealotBot::OnGameEnd()
{
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    for (auto unit : units)
    {
        auto range = Observation()->GetUnitTypeData().at(unit->unit_type).weapons.at(0).range;
        if (!unit->orders.empty() && unit->orders[0].ability_id == sc2::ABILITY_ID::ATTACK && unit->orders[0].progress == 0 && Distance2D(unit->pos, unit->orders[0].target_pos) <= range + unit->radius * 2)
        {
            cout << "Ja som last hitol " << unit->tag << endl;
            //Debug()->DebugSphereOut(unit->pos, 1);
            //Debug()->SendDebug();
        }
    }
    ++restarts_;
    if (restarts_ % 10 == 0)
    {
        this->UlozNaucene();
        //Priebezne uklada to je asi len docasne alebo sa potom zvacsi interval
        //ak by padlo a podobne
        cout << "Ukladam." << endl;
    }
    //cout << Observation()->GetUnitTypeData().at(static_cast<int>(sc2::UNIT_TYPEID::PROTOSS_ZEALOT)).l;

    auto vysledky = Observation()->GetResults();
    for (auto player_result : vysledky)
    {
        if (player_result.player_id == Observation()->GetPlayerID())
        {
            if (player_result.result == 0)
            {
                reward += 1000;
                global_reward += reward;
                cout << "Vyhral som. " << this->ReportNaKonciHry()<< endl;
                ql_->Learn(reward, new Stav(zstav_->to_array()), lastAction, false);
                break;
            }
            cout << "Prehral som. " << endl << this->ReportNaKonciHry() << endl;
            break;
        }
    }
    std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}


/*
*	Pokusi sa vybrat najvhodnejsiu cestu ktorou by sa dostal od protivnikou
*/
void ZealotBot::StrategiaUstup(const sc2::Unit*  unit, const sc2::Units enemies)
{
    //docasne to proste pojde dakde random :D
    sc2::Point2D *scan[8];
    float value[8] = { 0 };
    float currentX, currentY;
    currentX = unit->pos.x;
    currentY = unit->pos.y;
    auto speed = Observation()->GetUnitTypeData().at(unit->unit_type).movement_speed;

    float x, y;
    Triangulate(speed, 45, x, y);
    //cout << x << " " << y << endl;
    scan[0] = new sc2::Point2D(currentX + speed, currentY);
    scan[1] = new sc2::Point2D(currentX + x, currentY + y);
    scan[2] = new sc2::Point2D(currentX, currentY + speed);
    scan[3] = new sc2::Point2D(currentX - x, currentY + y);
    scan[4] = new sc2::Point2D(currentX - speed, currentY);
    scan[5] = new sc2::Point2D(currentX - x, currentY - y);
    scan[6] = new sc2::Point2D(currentX, currentY - speed);
    scan[7] = new sc2::Point2D(currentX + x, currentY - y);
    if (enemies.empty())
    {
        HladajNepriatela(unit);
        return;
    }
    int i = 0;
    for (auto point2_d : scan)
    {
        for (auto& enemy : enemies)
        {
            if (enemy->is_alive)
            {
                auto enemy_speed = Observation()->GetUnitTypeData().at(enemy->unit_type).movement_speed;
                value[i] += enemy_speed + Distance2D(*point2_d, enemy->pos);
            }

        }
        i++;
    }
    float bestValue = FLT_MIN;
    int bestIndex = -1;
    i = 0;
    for (auto point2_d : scan)
    {
        if (value[i] > bestValue && Observation()->IsPathable(*point2_d))
        {
            bestIndex = i;
            bestValue = value[i];
        }
        i++;
    }
    Actions()->UnitCommand(unit, sc2::ABILITY_ID::MOVE, *scan[bestIndex], false);
}

string ZealotBot::ReportNaKonciHry()
{
    sc2::Units units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    float friendly_hp = 0;
    for (auto unit : units)
    {
        friendly_hp += unit->health;
    }
    sc2::Units enemy_units = Observation()->GetUnits(sc2::Unit::Enemy);
    float enemy_hp = 0;
    for (auto unit : enemy_units)
    {
        enemy_hp += unit->health;
    }
    float dmg = Observation()->GetScore().score_details.total_damage_dealt.life;
    dmg += Observation()->GetScore().score_details.total_damage_dealt.shields;
    dmg += Observation()->GetScore().score_details.total_damage_dealt.energy;
    string ret_string = "Replikacia: " + to_string(restarts_) + " Pocet uceni: " + to_string(ql_->DajPocetUceni()) + "\n";
    ret_string += "Ziskana odmena: " + to_string(global_reward) + "\n";
    ret_string += "HP zealotov: " + to_string(friendly_hp) + "\n";
    ret_string += "HP marinakov: " + to_string(enemy_hp) + "\n";
    ret_string += "Rozdiel HP: " + to_string(friendly_hp - enemy_hp) + "\n";
    ret_string += "Udeleny dmg: " + to_string(dmg) + "\n";
    return ret_string;
}


/*
*	Najde vhodny target na utocenie a zacne nanho utocit
*	-> predpoklad je ze najvhodnejsi target je najblizsi target
*/
void ZealotBot::StrategiaZmenTarget(const sc2::Unit*  unit)
{
    sc2::Tag tagTargetu = 0;//malo by to byt nieco ako identifikator pre jednotky ak som to nepochopil zle
    if (unit->orders.size() > 0) {
        if (unit->orders[0].ability_id == sc2::ABILITY_ID::ATTACK)
        {
            //ak na niekoho utoci musime brat ohlad na koho aby sme nezmenili na toho isteho
            tagTargetu = unit->orders[0].target_unit_tag;
        }
    }
    sc2::Units jednotkyNepriatelov = Observation()->GetUnits(sc2::Unit::Enemy);
    const sc2::Unit* najblizsia1 = nullptr;
    int vzdialenost1 = std::numeric_limits<int>::max();
    const sc2::Unit* najblizsia2 = nullptr;
    int vzdialenost2 = std::numeric_limits<int>::max();

    if (jednotkyNepriatelov.empty()) {
        this->HladajNepriatela(unit);
        return;
    }
    //vyberie 2 najblizsie jednotky
    for (auto& jednotkaNepriatela : jednotkyNepriatelov)
    {
        const int pomVzdialenost = Distance2D(jednotkaNepriatela->pos, unit->pos);
        if (pomVzdialenost < vzdialenost1) {
            vzdialenost1 = pomVzdialenost;
            najblizsia1 = jednotkaNepriatela;
        }
        else if (pomVzdialenost < vzdialenost2) {
            vzdialenost2 = pomVzdialenost;
            najblizsia2 = jednotkaNepriatela;
        }
    }

    if (tagTargetu == 0) {
        //naposledy neutocil na nikoho takze berie prveho co ma prisebe
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, najblizsia1, false);
        return;
    }
    if (najblizsia2 == nullptr)
    {
        //2. najblizsi neexistuje teda utocime na 1. aj ked sa nanho utocilo aj pred tym
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::ATTACK, najblizsia1, false);
        return;
    }
    if (najblizsia1->tag == tagTargetu)
    {
        // ak momentalne nahana najblizsieho
        // zacne nahanat 2. najblizsieho
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, najblizsia2);
        return;
    }
    else
    {
        //inak zacne nahanat najblizsieho
        Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, najblizsia1);
        return;
    }

}

/*
*	Pokial na niekoho utoci bude utocit aj nadalej
*	To znamena ze bude za nim behat a bit inak musi najst
*	target na ktory bude utocit a vola sa zmen target
*/
void ZealotBot::StrategiaUtoc(const sc2::Unit*  unit)
{
    //je posledny order na indexe 0? ... isto chceme pouzivat ATTACK? je ich tam vacej
    if (unit->orders.size()>0) {
        if (unit->orders[0].ability_id == sc2::ABILITY_ID::ATTACK)
        {
            return;//uz na niekoho utoci netreba rusat
        }
    }
    //ak na nikoho neutovi mozme vybrat novy target a zautocit nanho
    this->StrategiaZmenTarget(unit);
}


ZealotBot::~ZealotBot()
{
    delete ql_;
    ql_ = nullptr;
    delete state_;
    state_ = nullptr;
    delete zstav_;
    zstav_ = nullptr;
}

void ZealotBot::HladajNepriatela(const sc2::Unit* unit)
{
    sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo());
    Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, target);
}

void ZealotBot::GetState() const
{
    float totalHealth = 0.0;
    float maxHealth = 0.0;
    float totalShield = 0.0;
    float maxShield = 0.0;
    auto units = Observation()->GetUnits(sc2::Unit::Alliance::Self);
    for (auto unit : units)
    {
        totalHealth += unit->health;
        maxHealth += unit->health_max;
        totalShield += unit->shield;
        maxShield += unit->shield_max;
    }
    zstav_->set_hp(totalHealth / maxHealth);
    zstav_->set_shield(totalShield / maxShield);
    auto enemy_units = Observation()->GetUnits(sc2::Unit::Alliance::Enemy);
    float distance1 = FLT_MAX;
    float distance2 = FLT_MAX;
    sc2::Unit* u1 = nullptr;
    sc2::Unit* u2 = nullptr;

    for(auto& unit : units)
    {
        for (auto& enemy_unit : enemy_units)
        {
            float current_distance = Distance2D(enemy_unit->pos, unit->pos);
            if (current_distance < distance1 && current_distance < distance2)
            {
                if (distance1 < distance2)
                {
                    u2 = const_cast<sc2::Unit*>(enemy_unit);
                    distance2 = current_distance;
                }
                else
                {
                    u1 = const_cast<sc2::Unit*>(enemy_unit);
                    distance1 = current_distance;
                }
                continue;
            }
            if (current_distance < distance2)
            {
                u2 = const_cast<sc2::Unit*>(enemy_unit);
                distance2 = current_distance;
                continue;
            }
            if (current_distance < distance1)
            {
                u1 = const_cast<sc2::Unit*>(enemy_unit);
                distance1 = current_distance;
            }
        }
    }
    if (distance1 > distance2)
    {
        float pomocna = distance2;
        sc2::Unit* pom = u2;
        distance2 = distance1;
        u2 = u1;
        distance1 = pomocna;
        u1 = pom;
    }
    zstav_->set_pocet_nepriatelov(enemy_units.size());
    if (u1 == nullptr)
    {
        zstav_->set_najblizsi_prvy_hp(1.0);
        zstav_->set_najblizsi_druhy_hp(1.0);
        zstav_->set_najblizsi_prvy_vzd(10.0);
        zstav_->set_najblizsi_druhy_vzd(10.0);
        zstav_->set_ciel(3);
    }
    else if (u2 == nullptr)
    {
        zstav_->set_najblizsi_prvy_hp((u1->health + u1->shield) / (u1->health_max + u1->shield_max));
        zstav_->set_najblizsi_druhy_hp(1.0);
        zstav_->set_najblizsi_prvy_vzd(distance1);
        zstav_->set_najblizsi_druhy_vzd(10.0);
        if (units[0]->engaged_target_tag == u1->tag)
        {
            zstav_->set_ciel(1);
        } else
        {
            zstav_->set_ciel(3);
        }
    }
    else
    {
        zstav_->set_najblizsi_prvy_hp((u1->health + u1->shield) / (u1->health_max + u1->shield_max));
        zstav_->set_najblizsi_druhy_hp((u2->health + u2->shield) / (u2->health_max + u2->shield_max));
        zstav_->set_najblizsi_prvy_vzd(distance1);
        zstav_->set_najblizsi_druhy_vzd(distance2);
        if (units[0]->engaged_target_tag == u1->tag)
        {
            zstav_->set_ciel(1);
        } else if (units[0]->engaged_target_tag == u2->tag)
        {
            zstav_->set_ciel(2);
        } else
        {
            zstav_->set_ciel(3);
        }

    }
}

void ZealotBot::Triangulate(float speed, float degree, float& x, float& y)
{
    x = cos(degree * pi / 180) * speed;
    y = sin(degree * pi / 180) * speed;
}

//vector<int>* ZealotBot::DajStav()
//{
//    return zstav_->to_array();
//}
