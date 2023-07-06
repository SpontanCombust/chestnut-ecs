#pragma once

#include "command.hpp"
#include "entity_world.hpp"
#include "command_queue.hpp"

#include <tuple>
#include <type_traits>

namespace chestnut::ecs
{
    template<class ...Cs>
    class CCreateEntityCommand : public ICommand
    {
    private:
        using TupleType = std::tuple<Cs...>;
        TupleType m_data;
        
    public: 
        CCreateEntityCommand(Cs&&... data)
        : m_data(data...)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            std::apply([&world](auto&&... args) {
                world.createEntity(std::move(args)...);
            }, m_data);
        }

        size_t size() const override
        {
            return sizeof(CCreateEntityCommand<Cs...>);
        }
    };
    
    template<>
    class CCreateEntityCommand<> : public ICommand
    {
    public:
        CCreateEntityCommand()
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.createEntity();
        }

        size_t size() const override
        {
            return sizeof(CCreateEntityCommand<>);
        }
    };



    class IEntityCommand : public ICommand
    {
    protected:
        CEntity m_entity;

    public:
        IEntityCommand(CEntity entity) 
        : m_entity(entity) 
        {

        }
    };

    class CDestroyEntityCommand : public IEntityCommand
    {
    public:
        CDestroyEntityCommand(CEntity entity)
        : IEntityCommand(entity)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.destroyEntity(m_entity);
        }

        size_t size() const override
        {
            return sizeof(CDestroyEntityCommand);
        }
    };



    template<class C>
    class CInsertComponentCommand : public IEntityCommand
    {
    private:
        C m_data;

    public:
        CInsertComponentCommand(CEntity entity, C &&data)
        : IEntityCommand(entity), m_data(data)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.insertComponent(m_entity, std::move(m_data));
        }

        size_t size() const override
        {
            return sizeof(CInsertComponentCommand);
        }
    };

    template<typename C, typename F, 
             std::enable_if_t<std::is_invocable_v<F, C&>, bool> = true>
    class CUpdateComponentCommand : public IEntityCommand
    {
    private:
        F m_updater;

    public:
        CUpdateComponentCommand(CEntity entity, F&& updater)
        : IEntityCommand(entity), m_updater(std::move(updater))
        {
            
        }

        void excecute(CEntityWorld& world) override
        {
            world.getComponent<C>(m_entity)
                 .map([this] (auto handle) { m_updater(*handle); });
        }

        size_t size() const override
        {
            return sizeof(CUpdateComponentCommand<C, F>);
        }
    };

    template<class C>
    class CDestroyComponentCommand : public IEntityCommand
    {
    public:
        CDestroyComponentCommand(CEntity entity)
        : IEntityCommand(entity)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.destroyComponent<C>(m_entity);
        }

        size_t size() const override
        {
            return sizeof(CDestroyComponentCommand);
        }
    };



    class CCommands
    {
    private:
        CCommandQueue m_queue;

    public:
        template<typename... Cs>
        CCommands& createEntity(Cs&&... data)
        {
            m_queue.insert(CCreateEntityCommand<Cs...>(std::forward<Cs>(data)...));
            return *this;
        }

        CCommands& createEntity()
        {
            m_queue.insert(CCreateEntityCommand<>());
            return *this;
        }

        CCommands& destroyEntity(CEntity ent)
        {
            m_queue.insert(CDestroyEntityCommand(ent));
            return *this;
        }

        template<class C>
        CCommands& insertComponent(CEntity ent, C&& data)
        {
            m_queue.insert(CInsertComponentCommand(ent, std::forward<C>(data)));
            return *this;
        }

        template<class C, class F>
        CCommands& updateComponent(CEntity ent, F&& updater)
        {
            m_queue.insert(CUpdateComponentCommand<C, F>(ent, std::forward<F>(updater)));
            return *this;
        }

        template<class C>
        CCommands& destroyComponent(CEntity ent)
        {
            m_queue.insert(CDestroyComponentCommand<C>(ent));
            return *this;
        }


        CCommands& clear()
        {
            m_queue.clear();
            return *this;
        }

        CCommandQueue& getCommandQueue()
        {
            return m_queue;
        }
    };  


} // namespace chestnut::ecs
