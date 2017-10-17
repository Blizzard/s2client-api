#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <string>
#include <random>
#include <cmath>

static uint32_t c_text_size = 8;

static std::string GetAbilityText(sc2::AbilityID ability_id) {
    std::string str;
    str += sc2::AbilityTypeToName(ability_id);
    str += " (";
    str += std::to_string(uint32_t(ability_id));
    str += ")";
    return str;
}

class EchoActionsBot : public sc2::Agent {
public:
    void OnGameStart() override {
        last_echoed_gameloop_ = 0;
    }

    void EchoAction(const sc2::RawActions& actions, sc2::DebugInterface* debug, const sc2::Abilities&) {
        if (actions.size() < 1) {
            debug->DebugTextOut(last_action_text_);
            return;
        }
        last_action_text_ = "";

        // For now, just show  the first action.
        const sc2::ActionRaw& action = actions[0];

        // Show the index of the ability being used.
        last_action_text_ = GetAbilityText(action.ability_id);

        // Add targeting information.
        switch (action.target_type) {
            case sc2::ActionRaw::TargetUnitTag:
                last_action_text_ += "\nTargeting Unit: " + std::to_string(action.target_tag);
                break;

            case sc2::ActionRaw::TargetPosition:
                last_action_text_ += "\nTargeting Pos: " + std::to_string(action.target_point.x) + ", " + std::to_string(action.target_point.y);
                break;

            case sc2::ActionRaw::TargetNone:
            default:
                last_action_text_ += "\nTargeting self";
        }

        debug->DebugTextOut(last_action_text_);
    }

    void OnStep() final {
        Control()->GetObservation();

        const sc2::ObservationInterface* obs = Observation();
        sc2::QueryInterface* query = Query();
        sc2::DebugInterface* debug = Debug();

        if (Control()->GetLastStatus() != SC2APIProtocol::Status::in_game)
            return;

        if (last_echoed_gameloop_ == obs->GetGameLoop())
            return;
        last_echoed_gameloop_ = obs->GetGameLoop();

        const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
        if (unit_types.size() < 1) {
            std::cout << "Unit types requested but not yet available." << std::endl;
            return;
        }
        const sc2::Abilities& abilities = obs->GetAbilityData();
        if (abilities.size() < 1) {
            std::cout << "Abilities requested but not yet available." << std::endl;
            return;
        }

        // Find a selected unit. {
        const sc2::Unit* unit = nullptr;
        for (const auto& try_unit : obs->GetUnits()) {
            if (try_unit->is_selected && try_unit->alliance == sc2::Unit::Self) {
                unit = try_unit;
                break;
            }
        }
        if (!unit)
            return;

        // Actions.
        EchoAction(obs->GetRawActions(), debug, abilities);

        // Show names for the selected unit.
        std::string debug_txt;
        {
            debug_txt = UnitTypeToName(unit->unit_type);
            if (debug_txt.length() < 1) {
                debug_txt = "(Unknown name)";
                assert(0);
            }
            debug_txt += " (" + std::to_string(unit->unit_type) + ")";
        }

        sc2::AvailableAbilities available_abilities = query->GetAbilitiesForUnit(unit);
        if (available_abilities.abilities.size() < 1) {
            std::cout << "No abilities available for this unit" << std::endl;
        }
        else {
            for (const sc2::AvailableAbility& available_ability : available_abilities.abilities) {
                assert(uint32_t(available_ability.ability_id) < abilities.size());
                const sc2::AbilityData& ability = abilities[available_ability.ability_id];
                assert(available_ability.ability_id == ability.ability_id);

                debug_txt += GetAbilityText(ability.ability_id) + "\n";
            }
        }
        debug->DebugTextOut(debug_txt, unit->pos, sc2::Colors::Green, c_text_size);

        // Show the direction of the unit.
        sc2::Point3D p1; // Use this to show target distance.
        {
            const float length = 5.0f;
            sc2::Point3D p0 = unit->pos;
            p0.z += 0.1f; // Raise the line off the ground a bit so it renders more clearly.
            p1 = unit->pos;
            assert(unit->facing >= 0.0f && unit->facing < 6.29f);
            p1.x += length * std::cos(unit->facing);
            p1.y += length * std::sin(unit->facing);
            debug->DebugLineOut(p0, p1, sc2::Colors::Yellow);
        }

        // Box around the unit.
        {
            sc2::Point3D p_min = unit->pos;
            p_min.x -= 2.0f;
            p_min.y -= 2.0f;
            p_min.z -= 2.0f;
            sc2::Point3D p_max = unit->pos;
            p_max.x += 2.0f;
            p_max.y += 2.0f;
            p_max.z += 2.0f;
            debug->DebugBoxOut(p_min, p_max, sc2::Colors::Blue);
        }

        // Sphere around the unit.
        {
            sc2::Point3D p = unit->pos;
            p.z += 0.1f; // Raise the line off the ground a bit so it renders more clearly.
            debug->DebugSphereOut(p, 1.25f, sc2::Colors::Purple);
        }

        // Pathing query to get the target.
        bool has_target = false;
        sc2::Point3D target;
        std::string target_info;
        for (const sc2::UnitOrder& unit_order : unit->orders)
        {
            // TODO: Need to determine if there is a target point, no target point, or the target is a unit/snapshot.
            target.x = unit_order.target_pos.x;
            target.y = unit_order.target_pos.y;
            target.z = p1.z;
            has_target = true;

            target_info = "Target:\n";
            if (unit_order.target_unit_tag != sc2::NullTag) {
                target_info += "Tag: " + std::to_string(unit_order.target_unit_tag) + "\n";
            }
            if (unit_order.progress != 0.0f && unit_order.progress != 1.0f) {
                target_info += "Progress: " + std::to_string(unit_order.progress) + "\n";
            }

            // Perform the pathing query.
            {
                float distance = query->PathingDistance(unit->pos, target);
                target_info += "\nPathing dist: " + std::to_string(distance);
            }

            break;
        }

        if (has_target)
        {
            sc2::Point3D p = target;
            p.z += 0.1f; // Raise the line off the ground a bit so it renders more clearly.
            debug->DebugSphereOut(target, 1.25f, sc2::Colors::Blue);
            debug->DebugTextOut(target_info, p1, sc2::Colors::Yellow, c_text_size);
        }

        debug->SendDebug();
    }

private:
    uint32_t last_echoed_gameloop_;
    std::string last_action_text_;
};

//*************************************************************************************************
int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    coordinator.SetRealtime(true);

    // Add the custom bot, it will control the players.
    EchoActionsBot bot;

    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Zerg)
    });

    // Start the game.
    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    // Step forward the game simulation.
    while (coordinator.Update()) {
        sc2::SleepFor(10);
    }

    return 0;
}
