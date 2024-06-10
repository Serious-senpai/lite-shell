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
        if (context.values.find("color") == context.values.end())
        {
            std::cerr << "Error: No color specified" << std::endl;
            return 1;
        }

        std::string color = context.values.at("color").front();
        if (!utils::setColor(color))
        {
            std::cerr << "Error: Unable to set color \"" << color << "\"" << std::endl;
            return 1;
        }

        std::cout << "Color changed to " << color << std::endl;
        return 0;
    }
};