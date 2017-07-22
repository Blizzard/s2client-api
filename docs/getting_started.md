Getting Started
===============

This document assumes you have already built sc2api-client, if you have not follow the instructions in building.md.
We will go over the following.

1. Running The Examples.
2. Running The Unit Tests.
3. Finding The Documentation.
4. Building Your Own Bot.


Running The Examples
--------------------

The code for examples can be found in examples/ and provide some basic code on how to utilize various parts of the api.

All the examples, at a minimum, require the path to Starcraft in order to run. You have three options for specifying
that path. A settings file, a command line argument or in code.

Open bot_mp.cc, for example. You'll see the first lines in main are the following -

```C++
sc2::GameSettings game_settings;
if (!ParseSettings(argc, argv, game_settings)) {
    std::cout << "No settings found." << std::endl;
    return 1;
}
```

The ParseSettings function attempts to load, first, from command line, then file, the required arguments for
running. You can view the arguments it's trying to load by specifying --help to the bot_mp executable.

With a console, navigate to to the location of the executable (in build/bin if you followed building.mp) and run.

```bash
bot_mp.exe --help
Usage: bot_mp.exe -e [executable]

Options -
  -e, --executable The path to Starcraft 2.
  -s, --step_size How many steps to take per call.
  -p, --port The port to make starcraft listen on.
  -r, --realtime Whether to run Starcraft 2 in  real time or not.
  -m, --map Which map to run.
  -t, --timeout Timeout for how long the library will block for a response.
No settings found.
```

As you can see, bot_mp is trying to tell us it needs the executable to run. Run it again with a valid executable path.

For example, the following path on my machine would run the example with a debug build of sc2.

```bash
bot_mp.exe -e "C:\...\Bin\Versions\Base00000\SC2_x64.exe"
```

The other option we provide is loading the executable path from a file. Once again, in a console, navigate to the
bot_mp binary (verify there isn't a game_settings.ini file in that directory) and run the following.

```bash
bot_mp.exe
Command Line Usage: bot_mp.exe -e [executable]

For more options: bot_mp.exe --help

Running The Unit Tests
----------------------

This repo comes with unit tests. The best unit tests. Nobody knows more about unit testing than us.

[Paul add instructions here]


Finding The Documentation
-------------------------

Hopefully all the headers are documented.

We should also provide a site for documentation.


Building Your Own Bot
---------------------

The examples folder comes with a tutorial.cc that is specifically stubbed out for you to follow our tutorial documents.

More details instructions are provided in tutorial1.md through tutorialN.md. If you'd like hands on experience creating
a bot please follow those!
