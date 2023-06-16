#pragma once

#include "command.hpp"
#include "entity_world.hpp"
#include "command_queue.hpp"

#include <tuple>
#include <type_traits>

namespace chestnut::ecs
{
    template<class ...Cs>
    class CCreateEntityCommand;
    
    template<>
    class CCreateEntityCommand<> : public ICommand
    {
    private:
        bool m_canRecycleId;

    public:
        CCreateEntityCommand(bool canRecycleId = true)
        : m_canRecycleId(canRecycleId)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.createEntity(m_canRecycleId);
        }

        size_t size() const override
        {
            return sizeof(CCreateEntityCommand<>);
        }
    };

    template<class C, class... CRest>
    class CCreateEntityCommand<C, CRest...> : public ICommand
    {
    private:
        using TupleType = std::tuple<C, CRest...>;
        TupleType m_data;
        bool m_canRecycleId;
        
    public: 
        CCreateEntityCommand(TupleType &&data, bool canRecycleId = true)
        : m_data(std::move(data)), m_canRecycleId(canRecycleId)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.createEntityWithComponents(std::move(m_data), m_canRecycleId);
        }

        size_t size() const override
        {
            return sizeof(CCreateEntityCommand<C, CRest...>);
        }
    };



    class IEntityCommand : public ICommand
    {
    protected:
        entityid_t m_entityId;

    public:
        IEntityCommand(entityid_t entityId) 
        : m_entityId(entityId) 
        {

        }

        entityid_t entity() const
        {
            return m_entityId;
        }
    };

    class CDestroyEntityCommand : public IEntityCommand
    {
    public:
        CDestroyEntityCommand(entityid_t entityId)
        : IEntityCommand(entityId)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.destroyEntity(m_entityId);
        }

        size_t size() const override
        {
            return sizeof(CDestroyEntityCommand);
        }
    };



    template<class C>
    class CCreateOrUpdateComponentCommand : public IEntityCommand
    {
    private:
        C m_data;

    public:
        CCreateOrUpdateComponentCommand(entityid_t id, C &&data)
        : IEntityCommand(id), m_data(data)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.createOrUpdateComponent(m_entityId, std::move(m_data));
        }

        size_t size() const override
        {
            return sizeof(CCreateOrUpdateComponentCommand);
        }
    };

    template<typename C, typename F, 
             std::enable_if_t<std::is_invocable_v<F, C&>, bool> = true>
    class CUpdateComponentCommand : public IEntityCommand
    {
    private:
        F m_updater;

    public:
        CUpdateComponentCommand(entityid_t id, F&& updater)
        : IEntityCommand(id), m_updater(std::move(updater))
        {
            
        }

        void excecute(CEntityWorld& world) override
        {
            auto handle = world.getComponent<C>(m_entityId);
            if(handle)
            {
                m_updater(*handle);
            }
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
        CDestroyComponentCommand(entityid_t id)
        : IEntityCommand(id)
        {

        }

        void excecute(CEntityWorld& world) override
        {
            world.destroyComponent<C>(m_entityId);
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
            using TupleType = std::tuple<Cs...>;
            TupleType t = std::make_tuple(data...);
            m_queue.insert(CCreateEntityCommand<Cs...>(std::forward<TupleType>(t), true));
            return *this;
        }

        CCommands& createEntity()
        {
            m_queue.insert(CCreateEntityCommand<>(true));
            return *this;
        }

        template<typename... Cs>
        CCommands& createUniqueEntity(Cs&&... data)
        {
            using TupleType = std::tuple<Cs...>;
            TupleType t = std::make_tuple(data...);
            m_queue.insert(CCreateEntityCommand<Cs...>(std::forward<TupleType>(t), false));
            return *this;
        }

        CCommands& createUniqueEntity()
        {
            m_queue.insert(CCreateEntityCommand<>(false));
            return *this;
        }

        CCommands& destroyEntity(entityid_t ent)
        {
            m_queue.insert(CDestroyEntityCommand(ent));
            return *this;
        }

        template<class C>
        CCommands& createOrUpdateComponent(entityid_t ent, C&& data)
        {
            m_queue.insert(CCreateOrUpdateComponentCommand(ent, std::forward<C>(data)));
            return *this;
        }

        template<class C, class F>
        CCommands& updateComponent(entityid_t ent, F&& updater)
        {
            m_queue.insert(CUpdateComponentCommand<C, F>(ent, std::forward<F>(updater)));
            return *this;
        }

        template<class C>
        CCommands& destroyComponent(entityid_t ent)
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
