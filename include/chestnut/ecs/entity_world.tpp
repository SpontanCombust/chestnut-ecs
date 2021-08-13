namespace chestnut::ecs
{
    // ========================= PUBLIC ========================= //

    template< class C >
    void CEntityWorld::setupComponentType( segsize segmentSize, segsize initCapacity ) 
    {
        if( !hasComponentTypeSetup<C>() )
        {
            m_mapCompTypeToStorage[ typeid(C) ] = new internal::CComponentStorage<C>( segmentSize, initCapacity );
        }
    }

    template<class C>
    void CEntityWorld::setupComponentType( segsize segmentSize ) 
    {
        if( !hasComponentTypeSetup<C>() )
        {
            m_mapCompTypeToStorage[ typeid(C) ] = new internal::CComponentStorage<C>( segmentSize, segmentSize );
        }
    }

    template<class C>
    bool CEntityWorld::hasComponentTypeSetup() const
    {
        auto it = m_mapCompTypeToStorage.find( typeid(C) );
        
        if( it != m_mapCompTypeToStorage.end() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }




    template< class C >
    C* CEntityWorld::createComponent( entityid entityID ) 
    {
        CHESTNUT_STATIC_ASSERT_VALID_COMPONENT_CLASS(C);

        setupComponentType<C>( CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE ); // if it wasn't done before then try to

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
