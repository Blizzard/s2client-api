Tutorial 1: Getting Started
===========================

This first tutorial will cover the following concepts. The source code for this tutorial
will be available, in full, at the bottom of this document. We will go over the following concepts.

1. Creating a simple bot class.
2. Starting a bot game with the SC2 Coordinator class.
3. Running the game simulation.

Creating A Simple Bot
---------------------

Open the file examples/tutorial.cc, this is where you'll be adding your code. It is already
setup with the necessary include paths and links the api.

In order to hook into and control game state using this library you'll want to inherit
from the Agent class and overwrite the events you are interested in.

You can see a list of all events available to you in sc2_client.h, ClientEvents. We will start
simple, our bot will start by printing hello world to console when the game starts so we will
only overwrite the OnGameStart event. Add the following code to tutorial.cc.

```C++
#include <sc2api/sc2_api.h>

#include <iostream>

using namespace sc2;

class Bot : public Agent {
public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }
};
```

> sc2_api.h is a grab bag of the common headers you need to create a bot. In this series of 
> tutorials you will end up using all the headers included but in your own projects you
> may choose to include the headers independently.


Starting The Game
-----------------

Now that we have our bot the next thing to do is start a game with the bot.

The sc2_coordinator is included from sc2_api.h, you will use it to start and manage a game.

> The coordinator acts as a managing class for any running games and replays.
> Some of its responsibilities include starting the Starcraft 2 executable
> with the proper game settings, setting up multiplayer and stepping your bots.

The coordinator requires some preliminary information to properly start and connect
to the Starcraft executable.

Replace the `main` function with the following code:

```C++
int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &bot),
        CreateComputer(Race::Zerg)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    return 0;
}
```

> If you have started the retail binary LoadSettings should automatically load necessary settings.
> Otherwise you can provide the path to the game on the command line.
> Invoke the binary with --help to see more detailed instructions.

In a future tutorial we will cover how you can specify settings as arguments via command
line.

At this point you should be able to build and run. The game should start on BelShirVestigeLE with
you controlling the Terran against an AI zerg. You should see 'Hello World' print in the
console when the game starts and then it will instantly shut down. 

By default the game is set to single step when using the library so you must step in
manually.

Running The Simulation
----------------------

Now you are ready to run the Starcraft 2 simulation. Overwrite the OnStep function in your bot. 
For now we will simply print the game loop.

Add to your bot the following function.

```C++
virtual void OnStep() final {
    std::cout << Observation()->GetGameLoop() << std::endl;
}

```

And the following to the bottom of the main function.

```C++
while (coordinator.Update()) {
}
```

Your bots OnStep function will be called each time the coordinator steps the simulation forward.
The Observation() function returns an interface that contains functions to examine
the current game state. We are simply printing the game loop in this example.

Exercises
---------

As a couple simple exercises try to do the following -

1. Print the number of minerals/vespene your bot currently has. (Hint: Use the ObservationInterface).
2. Overwrite and utilize another event from ClientEvents. You can control Terran while the game
is simulating, so you could, for example, overwrite OnUnitCreated() and print the tag when
an scv is made.

Full Source Code
----------------

```C++
#include <sc2api/sc2_api.h>

#include <iostream>

using namespace sc2;

class Bot : public Agent {
public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    virtual void OnStep() final {
        std::cout << Observation()->GetGameLoop() << std::endl;
    }
};

int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(Race::Terran, &bot),
        CreateComputer(Race::Zerg)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()) {
    }

    return 0;
}
```
