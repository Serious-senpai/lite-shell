#include "units.hpp"

namespace utils
{
    std::string memory_size(long double bytes)
    {
        std::string result = format("%.2LfB", bytes);
        if (bytes > 1024)
        {
            bytes /= 1024;
            result = format("%.2LfKB", bytes);

            if (bytes > 1024)
            {
                bytes /= 1024;
                result = format("%.2LfMB", bytes);

                if (bytes > 1024)
                {
                    bytes /= 1024;
                    result = format("%.2LfGB", bytes);

                    if (bytes > 1024)
                    {
                        bytes /= 1024;
                        result = format("%.2LfTB", bytes);
                    }
                }
            }
        }

        return result;
    }
}
