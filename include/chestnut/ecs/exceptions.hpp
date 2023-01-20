#pragma once

#include <exception>


namespace chestnut::ecs
{
    struct BadStorageAccessException : std::exception
    {
        const char *what() const throw()
        {
            return "Component could not be accessed in the storage. It's invalid or nonexistent.";
        }
    };

    struct BadHandleAccessException : std::exception
    {
        const char *what() const throw()
        {
            return "Tried to access invalid component handle.";
        }
    };    

} // namespace chestnut::ecs
