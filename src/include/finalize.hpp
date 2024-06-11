#pragma once

#include "utils.hpp"

namespace utils
{
    /**
     * @brief Register a callback to run when this object is destroyed
     * @see https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization
     */
    class Finalize
    {
    private:
        const std::function<void()> callback;

        Finalize(const Finalize &) = delete;
        Finalize &operator=(const Finalize &) = delete;

    public:
        /** @brief Construct a new `Finalize` object with a registered callback */
        Finalize(const std::function<void()> &callback) : callback(callback) {}

        /** @brief Destructor for this object, which invokes the underlying callback */
        ~Finalize()
        {
            callback();
        }
    };
}