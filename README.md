s2client-api
============

[![Build Status](https://travis-ci.org/Blizzard/s2client-api.png?branch=master)](https://travis-ci.org/Blizzard/s2client-api)

The StarCraft II API provides access to in-game state observation and unit control. The API is a wrapper around protobuf defined protocol over a websocket connection.

While it's possible to write directly to the protocol, this library provides a C++ and class-based abstraction. You can see a simple example below.

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

You can find a detailed tutorial on what this code does in docs/tutorial1.md.

Documentation
-------------

You can find API in documentation on our [github pages site](http://blizzard.github.io/s2client-api). The documentation is generated from code automatically, using [Doxygen](http://www.stack.nl/~dimitri/doxygen/)

To editor and generate the documentation yourself.

1. Download and install [doxygen](http://www.stack.nl/~dimitri/doxygen/download.html#srcbin)
2. Fork the repository and clone it locally
3. Checkout the master branch 'git checkout origin/master -b my-documentation-update
4. Make changes to the code documentation or docs/ files
5. From the root of the project run: doxygen Doxyfile
6. Review your documenation changes by opening docs/html/index.html in your browser
7. Push your changes to your fork and send us a pull request

Building
--------

This library uses [CMake](https://cmake.org/download/) to generate project files. It builds with [Visual Studio](https://www.visualstudio.com/downloads/) on Windows and Makefiles on the other platforms. It relies on the following contrib packages. 

1. Civetweb
2. Protobuf
3. (optional) SDL
4. ipv6-parse

Follow the instructions for submodules and building in [docs/building.md](docs/building.md).

Coding Standard
---------------

[Coding Standard](https://google.github.io/styleguide/cppguide.html)

We do our best to conform to the Google C++ Style Guide with the exception that we use four space tabs instead
of two space tabs.

Additional Downloads
----------------

### Maps and Replays

This repository only comes with a few maps for testing.

Additional maps and replays can be found [here](https://github.com/Blizzard/s2client-proto#downloads).

### Precompiled Libs

If you are using Visual Studio 2017 and just want precompiled library files you can download the following package:

[Precompiled Libs](http://blzdistsc2-a.akamaihd.net/SC2API_Binary_vs2017.zip)

### Other Libraries

Additional community built AI libraries can be found [here](https://github.com/Blizzard/s2client-proto#community).

If you are new to programming, the [CommandCenter](https://github.com/davechurchill/CommandCenter) framework may be a good starting point.
