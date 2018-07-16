#include "qlbot/bot_marine.h"

#include "sc2api/sc2_interfaces.h"
#include "sc2lib/sc2_lib.h"

#include <iostream>
#include <cfloat>

using namespace sc2;
using namespace std;

MarineBot::MarineBot() : restarts_(0)
{
    srand(time(nullptr));
}

void MarineBot::OnGameStart()
{
    std::cout << "Starting a new game (" << restarts_ << " restarts)" << std::endl;
}

void MarineBot::OnStep()
{
    uint32_t game_loop = Observation()->GetGameLoop();
    if (game_loop % 15 == 0)
    {
        auto units = Observation()->GetUnits(Unit::Alliance::Self);
        if (units.empty())
        {
            return;
        }
        for (auto unit : units)
        {
            Debug()->DebugMoveCamera(unit->pos);
            Debug()->SendDebug();

            Unit* closest_unit = nullptr;
            float distance = GetClosestEnemy(unit, closest_unit);
            const UnitTypes jednotky = Observation()->GetUnitTypeData(false);
            int nah = rand() % 100;
            if (distance == FLT_MAX)
            {
                //std::cout << "Nevidim zealota" << std::endl;
                /*sc2::Point2D target = sc2::FindRandomLocation(Observation()->GetGameInfo());
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, target);*/
                continue;
            }
            if (distance > 5)
            {
                //std::cout << "Nad max range" << std::endl;
                StrategiaDopredu(unit);
                /* if (nah < NadRangeUstup)
                {
                std::cout << "Ustup" << std::endl;
                StrategiaUstup(unit);
                }
                else if (nah < NadRangePostup)
                {
                std::cout << "Postup" << std::endl;
                StrategiaDopredu(unit);
                }
                else
                {
                std::cout << "Utok" << std::endl;
                StrategiaUtoc(unit);
                }*/
            }
            else if (distance == 5)
            {
                //std::cout << "Max range" << std::endl;
                StrategiaUtoc(unit);
                /*if (nah < RangeUstup)
                {
                std::cout << "Ustup" << std::endl;
                StrategiaUstup(unit);
                }
                else if (nah < RangePostup)
                {
                std::cout << "Postup" << std::endl;
                StrategiaDopredu(unit);
                }
                else
                {
                std::cout << "Utok" << std::endl;
                StrategiaUtoc(unit);
                }*/
            }
            else
            {
                //std::cout << "Pod max range" << std::endl;
                StrategiaUstup(unit);
                /*if (nah < PodRangeUstup)
                {
                std::cout << "Ustup" << std::endl;
                StrategiaUstup(unit);
                }
                else if (nah < PodRangePostup)
                {
                std::cout << "Postup" << std::endl;
                StrategiaDopredu(unit);
                }
                else
                {
                std::cout << "Utok" << std::endl;
                StrategiaUtoc(unit);
                }*/
            }
        }
        /*std::cout << std::endl;*/
    }
}

void MarineBot::OnGameEnd()
{
    ++restarts_;
    std::cout << "Game ended after: " << Observation()->GetGameLoop() << " loops " << std::endl;
}

void MarineBot::StrategiaUstup(const Unit* unit)
{
    Unit* closest_unit = nullptr;
    float distance = GetClosestEnemy(unit, closest_unit);
    const ObservationInterface* observation = Observation();
    Units units = observation->GetUnits(Unit::Ally);
    auto pomocna = Observation()->GetUnitTypeData().at((*unit).unit_type).weapons;
    if (distance == pomocna[0].range)
    {
        Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, unit->pos);
    }
    else
    {
        /*cout << "X najblizsieho nepriatela: " << closest_unit->pos.x << endl;
        cout << "Y najblizsieho nepriatela: " << closest_unit->pos.y << endl;
        cout << "X moje: " << unit->pos.x << endl;
        cout << "Y moje: " << unit->pos.y << endl;
        cout << "Koeficient: " << 5 / distance << endl;*/
        float x;
        float y;
        if (closest_unit->pos.x > unit->pos.x)
        {
            if (closest_unit->pos.y > unit->pos.y)
            {
                x = closest_unit->pos.x - abs(closest_unit->pos.x - unit->pos.x) * (pomocna[0].range / distance);
                y = closest_unit->pos.y - abs(closest_unit->pos.y - unit->pos.y) * (pomocna[0].range / distance);
            }
            else
            {
                x = closest_unit->pos.x - abs(closest_unit->pos.x - unit->pos.x) * (pomocna[0].range / distance);
                y = closest_unit->pos.y + abs(closest_unit->pos.y - unit->pos.y) * (pomocna[0].range / distance);
            }
        }
        else
        {
            if (closest_unit->pos.y > unit->pos.y)
            {
                x = closest_unit->pos.x + abs(closest_unit->pos.x - unit->pos.x) * (pomocna[0].range / distance);
                y = closest_unit->pos.y - abs(closest_unit->pos.y - unit->pos.y) * (pomocna[0].range / distance);
            }
            else
            {
                x = closest_unit->pos.x + abs(closest_unit->pos.x - unit->pos.x) * (5 / distance);
                y = closest_unit->pos.y + abs(closest_unit->pos.y - unit->pos.y) * (5 / distance);
            }
        }
        /*cout << "X kam by som mal ist: " << x << endl;
        cout << "Y kam by som mal ist: " << y << endl;
        cout << "Povodny distance " << distance << endl;*/
        Actions()->UnitCommand(unit, ABILITY_ID::MOVE, *new Point2D(x, y), false);
    }
}

void MarineBot::StrategiaDopredu(const Unit* unit)
{
    Unit* closest_unit = nullptr;
    GetClosestEnemy(unit, closest_unit);
    Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, closest_unit);
}

void MarineBot::StrategiaUtoc(const Unit* unit)
{
    if (!unit->orders.empty()) {
        if (unit->orders[0].ability_id != ABILITY_ID::ATTACK)
        {
            Actions()->UnitCommand(unit, ABILITY_ID::ATTACK, unit->pos);
        }
    }

}

float MarineBot::GetClosestEnemy(const Unit* source_unit, Unit*& closest_unit)
{
    auto enemy_units = Observation()->GetUnits(Unit::Enemy);
    float distance = FLT_MAX;
    for (auto& enemy_unit : enemy_units)
    {
        float current_distance = Distance2D(enemy_unit->pos, source_unit->pos);
        if (current_distance < distance)
        {
            closest_unit = const_cast<Unit*>(enemy_unit);
            distance = current_distance;
        }
    }
    return distance;
}
