#pragma once

#include <all.hpp>

#include "commands/cat.hpp"
#include "commands/cd.hpp"
#include "commands/clear.hpp"
#include "commands/color.hpp"
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
#include "commands/memory.hpp"
#include "commands/mkdir.hpp"
#include "commands/mv.hpp"
#include "commands/ps.hpp"
#include "commands/resume.hpp"
#include "commands/rm.hpp"
#include "commands/suspend.hpp"
#include "commands/volume.hpp"

void initialize(liteshell::Client *client)
{
    client->add_command<CatCommand>()
        ->add_command<CdCommand>()
        ->add_command<ClearCommand>()
        ->add_command<ColorCommand>()
        ->add_command<DateCommand>()
        ->add_command<EchoCommand>()
        ->add_command<EcholnCommand>()
        ->add_command<EnvCommand>()
        ->add_command<EvalCommand>()
        ->add_command<ExitCommand>()
        ->add_command<ForCommand>()
        ->add_command<HelpCommand>()
        ->add_command<IfCommand>()
        ->add_command<JumpCommand>()
        ->add_command<KillCommand>()
        ->add_command<LsCommand>()
        ->add_command<MemoryCommand>()
        ->add_command<MkdirCommand>()
        ->add_command<MvCommand>()
        ->add_command<PsCommand>()
        ->add_command<ResumeCommand>()
        ->add_command<RmCommand>()
        ->add_command<SuspendCommand>()
        ->add_command<VolumeCommand>();
}
