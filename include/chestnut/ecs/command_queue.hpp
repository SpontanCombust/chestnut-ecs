#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

#include "command.hpp"

namespace chestnut::ecs
{
    class CEntityWorld;

    class CCommandQueue
    {
    private:
        std::vector<std::byte> m_buff;

    public:
        template<typename C>
        void insert(C &&cmd)
        {
            static_assert(std::is_base_of_v<ICommand, C>, "The command class should implement ICommand interface");

            size_t cmdSize = cmd.size();

            // make room for the new command
            m_buff.resize(m_buff.size() + cmdSize);

            // pointer to the beginning of the buffer space for the new command
            std::byte *eob = m_buff.data() + m_buff.size() - cmdSize;

            // use placement new to safely move the command and its contents to the buffer
            new (eob) C(std::move(cmd));
        }

        void clear()
        {
            m_buff.clear();
        }

        void execute(CEntityWorld& world) const
        {
            if(m_buff.empty())
            {
                return;
            }

            for(size_t i = 0; i < m_buff.size();)
            {
                const std::byte *data = &m_buff[i];
                ICommand *cmd = (ICommand *)data;
                size_t cmdSize = cmd->size();

                cmd->excecute(world);

                i += cmdSize;
            }
        }
    };

} // namespace chetnut::ecs
