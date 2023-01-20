#pragma once

#include <exception>
#include <string>


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

    struct QueryException : std::exception
    {
        QueryException(const char *why)
        : std::exception(why)
        {
            
        }
    };

} // namespace chestnut::ecs
