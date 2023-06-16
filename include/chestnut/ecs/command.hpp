#pragma once

#include <cstddef>

namespace chestnut::ecs
{
    class CEntityWorld;

    class ICommand
    {
    public:
        virtual ~ICommand() = default;

        virtual void excecute(CEntityWorld& world) = 0;
        virtual size_t size() const = 0;
    };

} // namespace chestnut::ecs
