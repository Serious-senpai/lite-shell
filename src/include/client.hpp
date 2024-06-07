#pragma once

#include "base.hpp"
#include "environment.hpp"
#include "fuzzy_search.hpp"
#include "stream.hpp"
#include "subprocess.hpp"
#include "wrapper.hpp"

#define LITE_SHELL_SCRIPT_EXTENSION ".ff"
#define LITE_SHELL_BUFFER_SIZE 4096

namespace liteshell
{
    /**
     * @brief The command shell application.
     *
     * Represents a command shell client. The application should hold only one instance of this class.
     *
     * This class is responsible for managing commands, subprocesses and errorlevel. It also provides a method
     * to run the shell indefinitely.
     */
    class Client
    {
    private:
        static std::shared_ptr<Client> _instance;

        const std::vector<std::string> _extensions = {".exe", LITE_SHELL_SCRIPT_EXTENSION};

        std::vector<ProcessInfoWrapper *> _subprocesses;

        std::vector<CommandWrapper<BaseCommand>> _wrappers;
        std::map<std::string, std::size_t> _commands;

        const std::unique_ptr<Environment> _environment;
        const std::unique_ptr<InputStream> _stream;

        CommandWrapper<BaseCommand> _get_command(const std::string &name) const;
        CommandWrapper<BaseCommand> _get_command(const Context &context) const;

        /**
         * @brief Find an executable that `token` points to.
         *
         * The function will first look in the current working directory, then in the directories specified in `resolve_order`.
         *
         * @param token The token to resolve. This token may contain path separators.
         * @return The path to the executable if found, `std::nullopt` otherwise.
         */
        std::optional<std::string> resolve(const std::string &token) const;

        void process_batch_file(const std::string &path) const;

        /**
         * @brief An error handler that process exceptions thrown during command execution.
         *
         * @param e The exception object that was thrown.
         */
        void on_error(std::exception &e) const;

    public:
        /**
         * @brief Get the `Client` instance.
         *
         * @return A shared pointer to the `Client` instance
         */
        static std::shared_ptr<Client> get_instance();

        /**
         * @brief Construct a new `Client` object. Use `Client::get_instance()` instead.
         *
         * @see `Client::get_instance()`
         */
        Client();

        /** @brief Destructor for this object */
        ~Client();

        /**
         * @brief Get a pointer to the shell environment containing the variables.
         *
         * @return A pointer to the shell environment
         */
        Environment *get_environment() const;

        /**
         * @brief Get a pointer to the input stream.
         *
         * @return A pointer to the input stream
         */
        InputStream *get_stream() const;

        /**
         * @brief Get all commands of the current command shell.
         *
         * @return A set containing all commands
         */
        std::set<CommandWrapper<BaseCommand>> walk_commands() const;

        /**
         * @brief Get a command from the internal list of commands.
         * This can also be used as a way to get aliases.
         *
         * @param name The name of the command to get.
         * @return The command that was requested. If not found, returns an empty optional.
         */
        std::optional<CommandWrapper<BaseCommand>> get_optional_command(const std::string &name) const;

        /**
         * @brief Get all subprocesses of the current shell.
         *
         * @return An array of pointers to `ProcessInfoWrapper` objects.
         */
        std::vector<ProcessInfoWrapper *> get_subprocesses();

        /**
         * @brief Add a command to the internal list of commands.
         *
         * @param ptr A shared pointer `std::shared_ptr<BaseCommand>` to the command to add.
         * @return A pointer to the current client to allow fluent-style chaining
         */
        Client *add_command(const std::shared_ptr<BaseCommand> &ptr);

        /**
         * @brief Add a command to the internal list of commands.
         *
         * @tparam T A subclass of `BaseCommand`
         * @return A pointer to the current client to allow fluent-style chaining
         */
        template <typename T>
        Client *add_command();

        /**
         * @brief Search for a command that matches most closely to the given name.
         * @see `utils::fuzzy_search`
         *
         * @param name The name of the command to search for.
         * @return The name of the command that was found.
         */
        std::string fuzzy_command_search(const std::string &name) const;

        /**
         * @brief Run the shell indefinitely.
         */
        void run_forever();

        /**
         * @brief Process a command message.
         *
         * @note The resolution order is as follows:
         * @note - Built-in command
         * @note - Executable/batch file
         *
         * @param message The command message to process.
         */
        void process_command(const std::string &message);

        /**
         * @brief Split the PATH environment variable into a vector of paths.
         *
         * @return A vector of paths
         */
        std::vector<std::string> get_resolve_order() const;

        /**
         * @brief Spawn a subprocess and execute `command` in it.
         *
         * @param context A context holding the command to execute.
         * @return A pointer to the wrapper object containing information about the subprocess.
         */
        ProcessInfoWrapper *spawn_subprocess(const Context &context);

        /**
         * @brief Get the current errorlevel of the shell.
         *
         * @return The current errorlevel
         */
        DWORD get_errorlevel() const;
    };
}
