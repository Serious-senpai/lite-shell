#include "wrapper.hpp"

namespace liteshell
{
    template <typename T>
    CommandWrapper<T>::CommandWrapper(const std::shared_ptr<T> &command) : command(command)
    {
        static_assert(std::is_base_of_v<BaseCommand, T>, "CommandWrapper can only be used for BaseCommand subclasses");
    }

    template <typename T>
    DWORD CommandWrapper<T>::run(const Context &context)
    {
        return command->run(context);
    }

    /** @brief Comparator to use within `std::set` and `std::map` */
    template <typename T>
    bool operator<(const CommandWrapper<T> &lhs, const CommandWrapper<T> &rhs)
    {
        return lhs.command->name < rhs.command->name;
    }

    /** @brief Comparator to use within `std::set` and `std::map` */
    template <typename T>
    bool operator==(const CommandWrapper<T> &lhs, const CommandWrapper<T> &rhs)
    {
        return lhs.command->name == rhs.command->name;
    }
}
