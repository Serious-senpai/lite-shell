#pragma once

#include <all.hpp>

#include "commands/cat.hpp"
#include "commands/cd.hpp"
#include "commands/clear.hpp"
#include "commands/date.hpp"
#include "commands/echo.hpp"
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
#include "commands/resume.hpp"
#include "commands/suspend.hpp"

void initialize(liteshell::Client *client)
{
    client->add_command(std::make_shared<CatCommand>())
        ->add_command(std::make_shared<CdCommand>())
        ->add_command(std::make_shared<ClearCommand>())
        ->add_command(std::make_shared<DateCommand>())
        ->add_command(std::make_shared<EchoCommand>())
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
        ->add_command(std::make_shared<PsCommand>())
        ->add_command(std::make_shared<ResumeCommand>())
        ->add_command(std::make_shared<SuspendCommand>());
}
