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
    C* CEntityWorld::createComponent( entityid entityID ) 
    {
        CHESTNUT_STATIC_ASSERT_VALID_COMPONENT_CLASS(C);

        setupComponentTypeIfDidntAlready<C>();

        CComponent *uncastedComp;
        C *castedComp;

        uncastedComp = createComponentInternal( std::type_index( typeid( C ) ), entityID );
        castedComp = static_cast<C*>( uncastedComp );

        return castedComp;
    }

    template< class C >
    bool CEntityWorld::hasComponent( entityid entityID ) const
    {
        return hasComponentInternal( std::type_index( typeid( C ) ), entityID );
    }

    template< class C >
    C* CEntityWorld::getComponent( entityid entityID ) const
    {
        CComponent *uncastedComp;
        C *castedComp;

        uncastedComp = getComponentInternal( std::type_index( typeid( C ) ), entityID );
        castedComp = static_cast<C*>( uncastedComp );

        return castedComp;
    }

    template< class C >
    void CEntityWorld::destroyComponent( entityid entityID ) 
    {
        destroyComponentInternal( std::type_index( typeid( C ) ), entityID );
    }

} // namespace chestnut::ecs
