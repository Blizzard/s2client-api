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

All the examples, at a minimum, require the path to Starcraft in order to run.

On Windows and Mac, running the game manually once will automatically configure this path.
On Linux, you will need to provide the path on the command line.

All other options can be configured either on the command line or in code on the "sc2::Coordinator" object.

You can view available command line arguments with the "--help" option.

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
```


Running The Unit Tests
----------------------

This repo comes with a set of units tests. They are located in the "all_tests" project inside of the "tests" folder.

Simply building and running this project will run through all of the tests. Any errors will be output to standard error.


Finding The Documentation
-------------------------

You can find documentation for the api in docs/html, open index.html to view it in your browser.

Each of the examples are also documented. All examples are implemented in a single .cc file with the same name as the project. These files contain comments at the top of each of these file describing the functionality of each of the examples.


Building Your Own Bot
---------------------

The examples folder comes with a tutorial.cc that is specifically stubbed out for you to follow our tutorial documents.

More details instructions are provided in tutorial1.md through tutorialN.md. If you'd like hands on experience creating
a bot please follow those!
