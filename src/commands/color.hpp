#pragma once

#include <all.hpp>

class ColorCommand : public liteshell::BaseCommand
{
public:
    ColorCommand()
        : liteshell::BaseCommand(
              "color",
              "Change the text color in the shell",
              "Specify a color in hex format to change the text color",
              liteshell::CommandConstraint("color", "The color to set", true, false)) {}

    DWORD run(const liteshell::Context &context)
    {
        std::string color = context.get("color");
        utils::set_color(color);

        std::cout << "Color changed to " << color << std::endl;
        return 0;
    }
};
