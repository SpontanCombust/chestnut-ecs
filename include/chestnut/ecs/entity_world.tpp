#include "component_traits.hpp"

namespace chestnut::ecs
{
    // ========================= PUBLIC ========================= //

    template<class C>
    void CEntityWorld::setupComponentTypeIfDidntAlready() 
    {
        std::type_index type = typeid(C);

        auto it = m_mapCompTypeToStorage.find( type );
        
        if( it == m_mapCompTypeToStorage.end() )
        {
            m_mapCompTypeToStorage[ type ] = new internal::CComponentStorage<C>( 
                ComponentTraits<C>::storageSegmentSize, 
                ComponentTraits<C>::storageInitCapacity 
            );
        }
    }





    template< class C >
    CComponentHandle<C> CEntityWorld::createComponent( entityid entityID ) 
    {
        setupComponentTypeIfDidntAlready<C>();

        internal::IComponentWrapper *uncastedComp;
        uncastedComp = createComponentInternal( std::type_index( typeid( C ) ), entityID );
        auto handle = CComponentHandle<C>( entityID, static_cast< internal::SComponentWrapper<C>* >( uncastedComp ) );

        return handle;
    }

    template< class C >
    bool CEntityWorld::hasComponent( entityid entityID ) const
    {
        return hasComponentInternal( std::type_index( typeid( C ) ), entityID );
    }

    template< class C >
    CComponentHandle<C> CEntityWorld::getComponent( entityid entityID ) const
    {
        internal::IComponentWrapper *uncastedComp;
        uncastedComp = getComponentInternal( std::type_index( typeid( C ) ), entityID );
        auto handle = CComponentHandle<C>( entityID, static_cast< internal::SComponentWrapper<C>* >( uncastedComp ) );

        return handle;
    }

    template< class C >
    void CEntityWorld::destroyComponent( entityid entityID ) 
    {
        destroyComponentInternal( std::type_index( typeid( C ) ), entityID );
    }

} // namespace chestnut::ecs
