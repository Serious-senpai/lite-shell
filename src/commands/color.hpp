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
        if (!utils::setColor(color))
        {
            throw std::invalid_argument("Error: Unable to set color \"" + color + "\"");
        }

        std::cout << "Color changed to " << color << std::endl;
        return 0;
    }
};