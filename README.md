# lite-shell

[![Build docs](https://github.com/Serious-senpai/lite-shell/actions/workflows/docs.yml/badge.svg)](https://github.com/Serious-senpai/lite-shell/actions/workflows/docs.yml)
[![Lint](https://github.com/Serious-senpai/lite-shell/actions/workflows/lint.yml/badge.svg)](https://github.com/Serious-senpai/lite-shell/actions/workflows/lint.yml)
[![Test](https://github.com/Serious-senpai/lite-shell/actions/workflows/test.yml/badge.svg)](https://github.com/Serious-senpai/lite-shell/actions/workflows/test.yml)

Lightweight Windows command shell

## Build instructions
**This repository contains submodules. Therefore, it must be cloned with `git clone --recursive <...>` or `git submodule update --init --recursive`.**

A C++17 compiler (in Windows, of course) is required. To build the source files, run [scripts/build.bat](/scripts/build.bat).
This batch script will build the executables under the `build/` directory, which contains the command shell `shell.exe`.

## Run tests
The tests are written in Python and executed with [pytest](https://docs.pytest.org/). Although not necessary, it is recommended to create a virtual Python environment using `python3 -m venv .venv` first before installing the dependencies with `pip install -r requirements.txt`.

To execute the tests, simply invoke `pytest .` (or `pytest -v .` for more verbose output).

## Quickstart

> [!CAUTION]  
> Do not use the C/C++ [system](https://cplusplus.com/reference/cstdlib/system/) function since it makes this whole project becomes pointless. Otherwise, the following code is sufficient for 90% of the functionalities:
> ```cpp
> #include <cstdlib>
> #include <iostream>
>
> int main()
> {
>     while (true)
>     {
>         std::string input;
>         std::cout << "\nliteshell>";
>         std::cin >> input;
>         if (input == "exit")
>         {
>             break;
>         }
>         else
>         {
>             system(input.c_str());
>         }
>     }
> 
>     return 0;
> }
> ```

This section will guide you through adding a command to the application quickly. We will create a command `add` which calculates the sum of 2 numbers: `add 4 5` should print a value of `9`.

Note that commands within the application always run in the main process. If you want to isolate the execution in a subprocess, create a standalone executable (with `int main(int argc, const char *argv[])`) in [src/external](/src/external) instead.

First, navigate to [src/commands](/src/commands) and create a new file `add.hpp` (you can name this to whatever you want, but it should end with `*.hpp`) then paste the following code:
```cpp
#pragma once

#include <all.hpp>

class AddCommand : public liteshell::BaseCommand
{
public:
    AddCommand()
        : liteshell::BaseCommand(
              "add",
              "Add 2 integers",
              "Calculate the sum of 2 long long integers",
              {},
              liteshell::CommandConstraint(
                  "x", "The first integer", true,
                  "y", "The second integer", true)) {}

    DWORD run(const liteshell::Context &context)
    {
        auto x = std::stoll(context.get("x")), y = std::stoll(context.get("y"));
        std::cout << x + y << std::endl;

        return 0;
    }
};
```
This is the implementation of the `AddCommand` class. In fact, any commands must inherit `liteshell::BaseCommand`. The `AddCommand()` constructor calls to its super constructor with the following arguments:
```cpp
BaseCommand(const std::string &name, const std::string &description, const std::string &long_description, const std::initializer_list<std::string> &aliases, const CommandConstraint &constraint);
```

In the signature above, `name` is the name of the command (e.g. `add`), `description` is the description what will be shown when running `help` (e.g. `Add 2 integers`), `long_description` is the description that will be shown when running `help <command>` (e.g. running `help add` will print `Calculate the sum of 2 long long integers`).

The argument `alias` is a list of command aliases (the command `add` has no alias, so we are leaving this argument as an empty list). Finally, the `constraint` argument must be a `CommandConstraint` object, which states how arguments should be passed to the command and automatically generates a beautiful help message for you. In this example, a `CommandConstraint` object was created with 2 positional arguments: `x` and `y`, the string `The first integer` and `The second integer` are used to generate help message when running `help add`, the boolean values `true` indicate that both of these arguments are required.

Second, navigate to [src/initialize.hpp](/src/initialize.hpp) and add `#include "commands/add.hpp"`. In the function `void initialize(Client *client)`, add a call `client->add_command(std::make_shared<AddCommand>())`.

You can now build the shell using [scripts/build.bat](/scripts/build.bat) and test to see that the command works as expected! Try typing `help`, `help add` and `add 4 5`.
