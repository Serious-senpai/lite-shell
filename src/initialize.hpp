#pragma once

#include <client.hpp>

#include "commands/args.hpp"
#include "commands/cat.hpp"
#include "commands/cd.hpp"
#include "commands/date.hpp"
#include "commands/echoln.hpp"
#include "commands/env.hpp"
#include "commands/eval.hpp"
#include "commands/exit.hpp"
#include "commands/for.hpp"
#include "commands/help.hpp"
#include "commands/if.hpp"
#include "commands/jump.hpp"
#include "commands/kill.hpp"
#include "commands/ls.hpp"
#include "commands/mkdir.hpp"
#include "commands/ps.hpp"

void initialize(Client *client)
{
    client->add_command(std::make_shared<ArgsCommand>())
        ->add_command(std::make_shared<CatCommand>())
        ->add_command(std::make_shared<CdCommand>())
        ->add_command(std::make_shared<DateCommand>())
        ->add_command(std::make_shared<EcholnCommand>())
        ->add_command(std::make_shared<EnvCommand>())
        ->add_command(std::make_shared<EvalCommand>())
        ->add_command(std::make_shared<ExitCommand>())
        ->add_command(std::make_shared<ForCommand>())
        ->add_command(std::make_shared<HelpCommand>())
        ->add_command(std::make_shared<IfCommand>())
        ->add_command(std::make_shared<JumpCommand>())
        ->add_command(std::make_shared<KillCommand>())
        ->add_command(std::make_shared<LsCommand>())
        ->add_command(std::make_shared<MkdirCommand>())
        ->add_command(std::make_shared<PsCommand>());
}