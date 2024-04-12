# lite-shell
Lightweight Windows command shell

## Build instructions
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

> [!WARNING]  
> This section is outdated.

This section will guide you through adding a command to the application quickly. We will create a command `add` which calculates the sum of 2 numbers: `add 4 5` should print a value of `9`.

Note that commands within the application always run in the main process. If you want to isolate the execution in a subprocess, create a standalone executable (with `int main(int argc, const char *argv[])`) in [src/external](/src/external) instead.

First, navigate to [src/commands](/src/commands) and create a new file `add.hpp` (you can name this to whatever you want, but it should end with `*.hpp`) then paste the following code:
```cpp
#pragma once

class AddCommand : public BaseCommand
{
public:
    AddCommand() : BaseCommand("add", "Add 2 numbers", "", {}, CommandConstraint(3, 3)) {}

    DWORD run(const Context &context)
    {
        return 0;
    }
};
```
- The preprocessor directive `#pragma once` ensures that this header file will be included only once within the application code, you can read more about it [here](https://en.wikipedia.org/wiki/Pragma_once).
- All commands must have a default constructor that accepts no argument, so we define a constructor `AddCommand() : BaseCommand("add", "Add 2 numbers", "", {}, CommandConstraint(3, 3)) {}`. When calling `AddCommand()`, it will propagate to the constructor of `BaseCommand`, which initializes the following attributes:
  - Command name: `add`
  - Command help: `Add 2 numbers` - this will show up when using `help` or `help add`
  - Command long description: this will show up when using `help add`, we are currently using an empty string for this command since it's quite simple anyway.
  - Command aliases: `{}`, which denotes an empty list. If, for example, you add `sum` here (i.e. `{"sum"}`), then invoking `sum 4 5` will have the same effect as invoking `add 4 5`
  - Arguments constraint: `CommandConstraint(3, 3)` means that the command accepts *from* 3 *to* 3 positional arguments (i.e. exactly **3** positional arguments): Note that the command name is treated as a positional argument too (e.g. `add 4 5` has 3 arguments: `add`, `4` and `5`).
- The method `DWORD run(const Context &context)` is the command callback, it is called whenever this command is invoked and should return a `DWORD` value (i.e. an integer). If the command succeeds, a value of `0` is usually returned.

At this point, your IDE may complain a lot about compilation errors. Just ignore it and continue the next step.

Second, navigate to [src/initialize.hpp](/src/initialize.hpp) and add `#include "commands/add.hpp"`. In the function `void initialize(Client *client)`, add a call `client->add_command(std::make_shared<AddCommand>())`.

Notice that we haven't implemented the calculation for sum of integers yet, but now you can build the shell by running [scripts/build.bat](/scripts/build.bat) to see the changes - try typing the `help` and `help add` commands in the shell to see that we have added the command successfully.

However, at this point, invoking `add 4 5` prints nothing to the screen! The reason is because we haven't implement the logic inside `DWORD run(const Context &context)` yet. Paste the following code to the method:
```cpp
    DWORD run(const Context &context)
    {
        auto first = std::stoll(context.args[1]),
             second = std::stoll(context.args[2]);

        std::cout << first + second << std::endl;

        return 0;
    }
```
The `context.args` attribute is a [vector](https://cplusplus.com/reference/vector/vector/) of 3 elements which contains the command positional arguments (e.g. `add`, `4` and `5`). Then, we use `std::stoll` to convert them to 2 `long long` values - `first` and `second`. Finally, we print `first + second` to stdout before returning `0` to indicate that the command succeeded.

You can now build the shell again and test to see that the command works as expected.
