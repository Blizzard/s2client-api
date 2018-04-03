## SC2API Documentation

Some quick start guides can be accessed in the main repository:

[How to Build](docs/building.md) - How to get started building this project and how to build a Visual Studio solution.

[Getting Started with SC2API](docs/getting_started.md) - How to run examples.

[Tutorial 1](docs/tutorial1.md) - Create a simple bot.

[Tutorial 2](docs/tutorial2.md) - Make your bot build SCV's and Supply Depots.

[Tutorial 3](docs/tutorial3.md) - Building marines and scouting

If you're looking for the underlying protocol to communicate with the SC2 executable service see:

[StarCraft II Client Protocol (s2client-proto)](https://github.com/Blizzard/s2client-proto)

-----

#### Important Interfaces

The two most important interfaces you will use are the [ObservationInterface](classsc2_1_1_observation_interface.html)
and the [ActionInterface](classsc2_1_1_action_interface.html) which are available to your [Agent](classsc2_1_1_agent.html). Get the ObservationInterface by calling **Observation()**, and the ActionInterface by calling **Actions()** from your Agent.

An "Agent" is your bot, capable of performing actions in a game -- a player.

An "Observation" is a snapshot of the game's state at any given point in time, which is available at any time when a game or replay is running. The Observation Interface provides access to a list of units, including your Bot's units and enemy units that are visible to your units.

An "Action" is a command given to units, for example to attack, move or build a structure.

----

#### Important Notes

- Structures are a type of unit, and are included when you call "GetUnits".
