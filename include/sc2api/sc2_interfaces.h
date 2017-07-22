/*! \file sc2_interfaces.h
    \brief A set of public facing interfaces used to query game state.

Each function in this class is pure virtual but is not intended for a user of the API to overwrite.
An implementation file that utilizes this interface will be responsible for its definition and should
be of little concern to the consumer.
*/

#pragma once

#include "sc2api/sc2_common.h"
#include "sc2api/sc2_action.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_data.h"

#include <vector>

// Forward declarations to avoid including proto headers everywhere.
namespace SC2APIProtocol {
    class Observation;
}

namespace sc2 {

enum class UNIT_TYPEID;
enum class ABILITY_ID;

class ObservationInterface;
struct Score;
struct GameInfo;

enum class Visibility {
    Hidden = 0,
    Fogged = 1,
    Visible = 2,
    FullHidden = 3
};

//! Used to filter out units when querying. You can use this filter to get all full health units, for example.
//!< \param unit The unit in question to filter.
//!< \param observation The interface for querying game state to determine whether the unit should be filtered or not.
//!< \return Whether or not to filter the unit in or out of the list. true will add the unit, false will leave it out of the list.
//!< \sa GetUnits()
typedef std::function<bool(const Unit& unit)> Filter;

//! The ObservationInterface reflects the current state of the game. Guaranteed to be valid when OnGameStart or OnStep is called.
class ObservationInterface {
public:
    //! Gets a unique ID that represents the player.
    //!< \return The player ID.
    virtual uint32_t GetPlayerID() const = 0;

    //! Get the current game loop for this observation.
    //!< \return The game loop.
    virtual uint32_t GetGameLoop() const = 0;

    //! Get a list of all known units in the game.
    //!< \return List of all ally and visible enemy and neutral units.
    virtual const Units& GetUnits() const = 0;

    //! Get all units belonging to a certain alliance and meet the conditions provided by the filter. The unit structure is const data only.
    //! Therefore editing that data will not change any in game state. See the ActionInterface for changing Unit state.
    //!< \param alliance The faction the units belong to.
    //!< \param filter A functor or lambda used to filter out any unneeded units in the list.
    //!< \return A list of units that meet the conditions provided by alliance and filter.
    virtual Units GetUnits(Unit::Alliance alliance, Filter filter = {}) const = 0;

    //! Get all units belonging to self that meet the conditions provided by the filter. The unit structure is const data only.
    //! Therefore editing that data will not change any in game state. See the ActionInterface for changing Unit state.
    //!< \param filter A functor or lambda used to filter out any unneeded units in the list.
    //!< \return A list of units that meet the conditions provided by the filter.
    virtual Units GetUnits(Filter filter) const = 0;

    //! Get the unit state as represented by the last call to GetObservation.
    //!< \param tag Unique tag of the unit.
    //!< \return Pointer to the Unit object.
    virtual const Unit* GetUnit(Tag tag) const = 0;

    //! Get the unit state as represented by the the call to GetObservation from the previous frame. This is useful for calculating deltas between Unit states.
    //! It is unsafe to assume GetUnit and GetPreviousUnit will both necessarily return a Unit given the same tag. The Unit may have been created or
    //! destroyed in the last step.
    //!< \param tag Unique tag of the unit.
    //!< \return Pointer to the Unit object.
    virtual const Unit* GetPreviousUnit(Tag tag) const = 0;

    //! Gets a list of units added in the previous step. You could also hook into the OnUnitCreated event for this data on a per Unit basis.
    //!< \sa OnUnitCreated()
    virtual const Units& GetUnitsAdded() const = 0;

    //! Gets a list of units added in the previous step. You could also hook into the OnUnitCreated event for this data on a per Unit basis.
    //! Ally units disappear because they are destroyed. Enemy units may disappear because they move out of visibility, or because they are destroyed.
    //!< \return A list of units that were removed from the observation.
    //!< \sa OnUnitCreated()
    virtual const Units& GetUnitsRemoved() const = 0;

    //! Gets a list of actions performed as abilities applied to units. For use with the raw option.
    //!< \return List of raw actions.
    virtual const RawActions& GetRawActions() const = 0;

    //! Gets a list of actions performed as abilities applied to the current selection. For use with the feature layer or rendered options.
    //!< \return List of actions.
    virtual const SpatialActions& GetFeatureLayerActions() const = 0;

    //! Gets all power sources associated with the current player.
    //!< \return List of power sources.
    virtual const std::vector<PowerSource>& GetPowerSources() const = 0;

    //! Gets all upgrades.
    //!< \return List of upgrades.
    virtual const std::vector<UpgradeID>& GetUpgrades() const = 0;

    //! Gets the detailed current set of scores.
    //!< \return The current score structure.
    virtual const Score& GetScore() const = 0;

    //! Gets metadata of abilities. Array can be indexed directly by AbilityID.
    //!< \param force_refresh forces a full query from the game, may otherwise cache data from a previous call.
    //!< \return Data about all abilities possible for the current game session.
    virtual const Abilities& GetAbilityData(bool force_refresh = false) const = 0;

    //! Gets metadata of units. Array can be indexed directly by UnitID.
    //!< \param force_refresh forces a full query from the game, may otherwise cache data from a previous call.
    //!< \return Data about all units possible for the current game session.
    virtual const UnitTypes& GetUnitTypeData(bool force_refresh = false) const = 0;

    //! Gets metadata of upgrades. Array can be indexed directly by UpgradeID.
    //!< \param force_refresh forces a full query from the game, may otherwise cache data from a previous call.
    //!< \return Data about all upgrades possible for the current game session.
    virtual const Upgrades& GetUpgradeData(bool force_refresh = false) const = 0;

    //! Gets metadata of buffs. Array can be indexed directly by BuffID.
    //!< \param force_refresh forces a full query from the game, may otherwise cache data from a previous call.
    //!< \return Data about all buffs possible for the current game session.
    virtual const Buffs& GetBuffData(bool force_refresh = false) const = 0;

    //! Gets the GameInfo struct for the current map.
    //!< \return The current GameInfo struct.
    virtual const GameInfo& GetGameInfo() const = 0;

    //! The mineral count of the player.
    //!< \return The mineral count.
    virtual int32_t GetMinerals() const = 0;

    //! The vespene count of the player.
    //!< \return The vespene count.
    virtual int32_t GetVespene() const = 0;

    //! The total supply cap given the players max possible supply.
    //!< \return Food cap.
    //!< \sa GetFoodUsed() GetFoodArmy() GetFoodWorkers()
    virtual int32_t GetFoodCap() const = 0;

    //! The total supply used by the player as defined: GetFoodArmy() + GetFoodWorkers().
    //!< \return Food used.
    //!< \sa GetFoodArmy() GetFoodWorkers()
    virtual int32_t GetFoodUsed() const = 0;

    //! The total supply consumed by army units alone.
    //!< \sa GetFoodUsed() GetFoodWorkers()
    //!< \return Food used by army units.
    virtual int32_t GetFoodArmy() const = 0;

    //! The total supply consumed by workers units alone.
    //!< \sa GetFoodArmy() GetFoodUsed()
    //!< \return Food used by worker units.
    virtual int32_t GetFoodWorkers() const = 0;

    //! The number of workers that currently have no orders.
    //!< \return Count of idle workers.
    virtual int32_t GetIdleWorkerCount() const = 0;

    //! The number of army units.
    //!< \return Count of army units.
    virtual int32_t GetArmyCount() const = 0;

    //! Number of warp gates owned by the player. This value should only be nonzero for Protoss.
    //!< \return Count of warp gates.
    virtual int32_t GetWarpGateCount() const = 0;

    //! Position of the center of the camera.
    //!< \return Camera position.
    virtual Point2D GetCameraPos() const = 0;

    //! Gets the initial start location of the player.
    //!< \return Player start position.
    virtual Point3D GetStartLocation() const = 0;

    //! Returns 'true' if the given point has creep.
    //!< \param point Position to sample.
    //!< \return Creep.
    virtual bool HasCreep(const Point2D& point) const = 0;

    //! Returns visibility value of the given point for the current player.
    //!< \param point Position to sample.
    //!< \return Visibility.
    virtual Visibility GetVisibility(const Point2D& point) const = 0;

    //! Returns 'true' if the given point on the terrain is pathable. This does not
    // include pathing blockers like structures. For more accurate pathing results
    // use QueryInterface::PathingDistance.
    //!< \param point Position to sample.
    //!< \return Pathable.
    virtual bool IsPathable(const Point2D& point) const = 0;

    //! Returns 'true' if the given point on the terrain is buildable. This does not
    // include blockers like other structures. For more accurate building placement
    // results use QueryInterface::Placement.
    //!< \param point Position to sample.
    //!< \return Placable.
    virtual bool IsPlacable(const Point2D& point) const = 0;

    //! Returns terrain height of the given point.
    //!< \param point Position to sample.
    //!< \return Height.
    virtual float TerrainHeight(const Point2D& point) const = 0;

    //! A pointer to the low-level protocol data for the current observation. While it's possible to extract most in-game data from this pointer
    // it is highly discouraged. It should only be used for extracting feature layers because it would be inefficient to copy these each frame.
    //!< \return A const pointer to the Observation.
    //!< \sa Observation GetObservation()
    virtual const SC2APIProtocol::Observation* GetRawObservation() const = 0;
};


//! The QueryInterface provides additional data not contained in the observation.
//!
//! Performance note:
//!  - Always try and batch things up. These queries are effectively synchronous and will block until returned.
class QueryInterface {
public:
    //! Returns a list of abilities represented as a uint32_t see the ABILITY_ID enum for their corresponding, named, representations.
    //!< \param tag Tag of unit.
    //!< \param ignore_resource_requirements Ignores food, mineral and gas costs, as well as cooldowns.
    //!< \return Abilities for the unit.
    virtual AvailableAbilities GetAbilitiesForUnit(Tag tag, bool ignore_resource_requirements = false) = 0;
    //! Issues multiple available abilities queries.
    //! Batch version.
    //!< \param tag Tags of units.
    //!< \param ignore_resource_requirements Ignores food, mineral and gas costs, as well as cooldowns.
    //!< \return Abilities for the units.
    virtual std::vector<AvailableAbilities> GetAbilitiesForUnits(const std::vector<Tag>& tags, bool ignore_resource_requirements = false) = 0;

    //! Returns pathing distance between two locations. Takes into account unit movement properties (e.g. Flying).
    //!< \param start Starting point.
    //!< \param end End point.
    //!< \return Distance between the two points.
    virtual float PathingDistance(const Point2D& start, const Point2D& end) = 0;
    //! Returns pathing distance between a unit and a target location. Takes into account unit movement properties (e.g. Flying).
    //! Batch version.
    //!< \param start Starting points.
    //!< \param end End points.
    //!< \return Distances between the two points.
    virtual float PathingDistance(const Tag& start_unit_tag, const Point2D& end) = 0;

    struct PathingQuery {
        Tag start_unit_tag_ = NullTag;
        Point2D start_;
        Point2D end_;
    };
    //! Issues multiple pathing queries.
    virtual std::vector<float> PathingDistance(const std::vector<PathingQuery>& queries) = 0;

    //! Returns whether a building can be placed at a location.
    //! The placing unit field is optional. This is only used for cases where the placing unit plays a role in the
    //! placement grid test (e.g. A flying barracks building an add-on requires room for both the barracks and add-on).
    //!< \param ability Ability for building or moving a structure.
    //!< \param end target_pos Position to attempt placement on.
    //!< \param placing_unit_tag_ (Optional) The unit that is moving, if moving a structure.
    //!< \return If placement is possible.
    virtual bool Placement(const AbilityID& ability, const Point2D& target_pos, Tag placing_unit_tag_ = NullTag) = 0;

    struct PlacementQuery {
        PlacementQuery() = default;
        PlacementQuery(AbilityID ability_id, Point2D target) :
            ability(ability_id),
            target_pos(target) {};

        AbilityID ability;
        Point2D target_pos;
        Tag placing_unit_tag = 0LL;     // Optional. Used for testing placement with add-ons.
    };
    //! A batch version of the above Placement query. Takes an array of abilities, positions and
    //! optional unit tags and returns a matching array of bools indicating if placement is possible.
    //!< \param queries Placement queries.
    //!< \return Array of bools indicating if placement is possible.
    virtual std::vector<bool> Placement(const std::vector<PlacementQuery>& queries) = 0;
};


//! The ActionInterface issues actions to units in a game. Not available in replays.
//! Guaranteed to be valid when the OnStep event is called.
class ActionInterface {
public:
    /*!\fn virtual void UnitCommand(Tag unit_tag, uint32_t ability)
     * Batches a UnitCommand that will be dispatched when SendActions() is called. UnitCommand has many overloaded functions, you can call it with
     * most combinations of Unit types (the Unit object or tag), ability types (the enum or uint32_t) and targets (a 2D position or tag).
     * \param unit_tag The unique id that represents the unit.
     * \param ability The unique id that represents the ability, see ABILITY_ID for ids.
     * \sa ABILITY_ID Unit Point2D SendActions()
     */

    //! Issues a command to a unit. Self targeting.
    //!< \param unit_tag The unit to send the command to.
    //!< \param ability The ability id of the command.
    virtual void UnitCommand(Tag unit_tag, AbilityID ability) = 0;

    //! Issues a command to a unit. Targets a point.
    //!< \param unit_tag The unit to send the command to.
    //!< \param ability The ability id of the command.
    //!< \param point The 2D world position to target.
    virtual void UnitCommand(Tag unit_tag, AbilityID ability, const Point2D& point) = 0;

    //! Issues a command to a unit. Targets another unit.
    //!< \param unit_tag The unit to send the command to.
    //!< \param ability The ability id of the command.
    //!< \param target_tag The unit that is a target of the unit getting the command.
    virtual void UnitCommand(Tag unit_tag, AbilityID ability, Tag target_tag) = 0;

    //! Issues a command to multiple units (prefer this where possible). Same as UnitCommand(Tag, AbilityID).
    virtual void UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability) = 0;

    //! Issues a command to multiple units (prefer this where possible). Same as UnitCommand(Tag, AbilityID, Point2D).
    virtual void UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability, const Point2D& point) = 0;

    //! Issues a command to multiple units (prefer this where possible). Same as UnitCommand(Tag, AbilityID, Tag).
    virtual void UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability, Tag target_tag) = 0;

    //! Returns a list of unit tags that have sent commands out in the last call to SendActions. This will be used to determine
    //! if a unit actually has a command when the observation is received.
    //!< \return Array of units that have sent commands.
    virtual const std::vector<Tag>& Commands() const = 0;

    //! Enables or disables autocast of an ability on a unit.
    //!< \param unit_tag The unit to toggle the ability on.
    //!< \param ability The ability to be toggled.
    virtual void ToggleAutocast(Tag unit_tag, AbilityID ability) = 0;
    //! Enables or disables autocast of an ability on a list of units.
    //!< \param unit_tags The units to toggle the ability on.
    //!< \param ability The ability to be toggled.
    virtual void ToggleAutocast(const std::vector<Tag>& unit_tags, AbilityID ability) = 0;

    //! This function sends out all batched unit commands. You DO NOT need to call this function in non real time simulations since
    //! it is automatically called when stepping the simulation forward. You only need to call this function in a real time simulation.
    //! For example, if you wanted to move 20 marines to some position on the map you'd want to batch all of those unit commands and
    //! send them at once.
    virtual void SendActions() = 0;
};

//! The ActionFeatureLayerInterface emulates UI actions in feature layer. Not available in replays.
//! Guaranteed to be valid when the OnStep event is called.
class ActionFeatureLayerInterface {
public:
    //! Issues a command to whatever is selected. Self targeting.
    //!< \param ability The ability id of the command.
    virtual void UnitCommand(AbilityID ability) = 0;

    //! Issues a command to whatever is selected. Uses a point as a target for the command.
    //!< \param ability The ability id of the command.
    //!< \param point The 2D world position to target.
    //!< \param minimap Target in the minimap instead of the map.
    virtual void UnitCommand(AbilityID ability, const Point2DI& point, bool minimap = false) = 0;

    //! Moves the camera to be centered around a position. Coordinate is position on minimap feature layer.
    virtual void CameraMove(const Point2DI& center) = 0;

    //! Selection of a point, equivalent to clicking the mouse on a unit.
    //!< \param center The feature layer 'pixel' being clicked on.
    //!< \param selection_type Any modifier keys, for example if 'shift-click' is desired.
    virtual void Select(const Point2DI& center, PointSelectionType selection_type) = 0;

    //! Selection of an area, equivalent to click-dragging the mouse over an area of the screen.
    //!< \param p0 The feature layer pixel where the first click occurs (mouse button down).
    //!< \param p1 The feature layer pixel where the drag release occurs (mouse button up).
    //!< \param add_to_selection Will add newly selected units to an existing selection.
    virtual void Select(const Point2DI& p0, const Point2DI& p1, bool add_to_selection = false) = 0;

    //! This function sends out all batched selection and unit commands. You DO NOT need to call this function in non real time simulations since
    //! it is automatically called when stepping the simulation forward. You only need to call this function in a real time simulation.
    virtual void SendActions() = 0;
};


//! DebugInterface draws debug text, lines and shapes. Available at any time after the game starts.
//! Guaranteed to be valid when the OnStep event is called.
//! All debug actions are queued and dispatched when SendDebug is called. All drawn primitives
//! continue to draw without resending until another SendDebug is called.
class DebugInterface {
public:
    // Debug drawing primitives.

    //! Outputs text at the top, left of the screen.
    //!< \param out The string of text to display.
    //!< \param color (Optional) Color of the text.
    virtual void DebugTextOut(const std::string& out, Color color = Colors::White) = 0;
    //! Outputs text at any 2D point on the screen. Coordinate ranges are 0..1 in X and Y.
    //!< \param out The string of text to display.
    //!< \param pt_virtual_2D The screen position to draw text at.
    //!< \param color (Optional) Color of the text.
    virtual void DebugTextOut(const std::string& out, const Point2D& pt_virtual_2D, Color color = Colors::White) = 0;
    //! Outputs text at any 3D point in the game world. Map coordinates are used.
    //!< \param out The string of text to display.
    //!< \param pt3D The world position to draw text at.
    //!< \param color (Optional) Color of the text.
    virtual void DebugTextOut(const std::string& out, const Point3D& pt3D, Color color = Colors::White) = 0;
    //! Outputs a line between two 3D points in the game world. Map coordinates are used.
    //!< \param p0 The starting position of the line.
    //!< \param p1 The ending position of the line.
    //!< \param color (Optional) Color of the line.
    virtual void DebugLineOut(const Point3D& p0, const Point3D& p1, Color color = Colors::White) = 0;
    //! Outputs a box specified as two 3D points in the game world. Map coordinates are used.
    //!< \param p0 One corner of the box.
    //!< \param p1 The far corner of the box.
    //!< \param color (Optional) Color of the lines.
    virtual void DebugBoxOut(const Point3D& p_min, const Point3D& p_max, Color color = Colors::White) = 0;
    //! Outputs a sphere specified as a 3D point in the game world and a radius. Map coordinates are used.
    //!< \param p Center of the sphere.
    //!< \param r Radius of the sphere.
    //!< \param color (Optional) Color of the lines.
    virtual void DebugSphereOut(const Point3D& p, float r, Color color = Colors::White) = 0;

    // Cheats.

    //! Creates a unit at the given position.
    //!< \param unit_type Type of unit to create.
    //!< \param p Position to create the unit at.
    //!< \param player_id Player the unit should belong to.
    //!< \param count Number of units to create.
    virtual void DebugCreateUnit(UnitTypeID unit_type, const Point2D& p, uint32_t player_id = 1, uint32_t count = 1) = 0;

    //! Destroy a unit.
    //!< \param tag Unit to destroy.
    virtual void DebugKillUnit(Tag tag) = 0;

    //! Makes the entire map visible, i.e., removes the fog-of-war.
    virtual void DebugShowMap() = 0;
    //! Enables commands to be issued to enemy units.
    virtual void DebugEnemyControl() = 0;
    //! Disables the food check.
    virtual void DebugIgnoreFood() = 0;
    //! Disables resource checks.
    virtual void DebugIgnoreResourceCost() = 0;
    //! Gives a bunch of minerals and gas.
    virtual void DebugGiveAllResources() = 0;
    //! Makes the units of a player indestructible.
    virtual void DebugGodMode() = 0;
    //! Ignores mineral costs.
    virtual void DebugIgnoreMineral() = 0;
    //! Cooldowns become instant.
    virtual void DebugNoCooldowns() = 0;
    //! All tech becomes available.
    virtual void DebugGiveAllTech() = 0;
    //! All upgrades are available.
    virtual void DebugGiveAllUpgrades() = 0;
    //! Structures and units are built much faster.
    virtual void DebugFastBuild() = 0;
    //! Sets the scripted "curriculum" score.
    virtual void DebugSetScore(float score) = 0;
    //! Ends a game.
    //!< \param victory If true, this player is victorious. If false, this player surrenders.
    virtual void DebugEndGame(bool victory = false) = 0;
    //! Sets the energy level on a unit.
    //!< \param value The new energy level.
    //!< \param tag The unit.
    virtual void DebugSetEnergy(float value, Tag tag) = 0;
    //! Sets the life on a unit.
    //!< \param value The new life.
    //!< \param tag The unit.
    virtual void DebugSetLife(float value, Tag tag) = 0;
    //! Sets shields on a unit.
    //!< \param value The new shields.
    //!< \param tag The unit.
    virtual void DebugSetShields(float value, Tag tag) = 0;

    //! Sets the position of the camera.
    //!< \param pos The camera position in world space.
    virtual void DebugMoveCamera(const Point2D& pos) = 0;

      enum AppTest {
        hang = 1,
        crash = 2,
        exit = 3
      };
    //! Cause the game to fail; useful to test library behavior.
    //!< \param app_test State to put the game into.
    //!< \param delay_ms Time to elapse before invoking the game state.
      virtual void DebugTestApp(AppTest app_test, int delay_ms = 0) = 0;

    //! Dispatch all queued debug commands. No debug commands will be sent until this is called.
    //! This will also clear or set new debug primitives like text and lines.
    virtual void SendDebug() = 0;
};

}
