#pragma once

#include <client.hpp>

#include "commands/args.hpp"
#include "commands/cd.hpp"
#include "commands/echo.hpp"
#include "commands/exit.hpp"
#include "commands/help.hpp"
#include "commands/ls.hpp"
#include "commands/sleep.hpp"
#include "commands/type.hpp"

void initialize(Client *client)
{
    client->add_command(std::make_shared<ArgsCommand>())
        ->add_command(std::make_shared<CdCommand>())
        ->add_command(std::make_shared<EchoCommand>())
        ->add_command(std::make_shared<ExitCommand>())
        ->add_command(std::make_shared<HelpCommand>())
        ->add_command(std::make_shared<LsCommand>())
        ->add_command(std::make_shared<SleepCommand>())
        ->add_command(std::make_shared<TypeCommand>());
}