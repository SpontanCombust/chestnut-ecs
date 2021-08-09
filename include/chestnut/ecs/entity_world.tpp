namespace chestnut
{
    // ========================= PUBLIC ========================= //

    template< class C >
    void CEntityWorld::setupComponentType( segsize segmentSize, segsize initCapacity ) 
    {
        std::type_index compType = typeid(C);

        auto it = m_mapCompTypeToStorage.find( compType );
        
        if( it == m_mapCompTypeToStorage.end() )
        {
            m_mapCompTypeToStorage[ compType ] = new internal::CComponentStorage<C>( segmentSize, initCapacity );
        }
    }

    template<class C>
    void CEntityWorld::setupComponentType( segsize segmentSize ) 
    {
        setupComponentType<C>( segmentSize, segmentSize );
    }



    template< class C >
    C* CEntityWorld::createComponent( entityid entityID ) 
    {
        ASSERT_DERIVED_FROM_COMPONENT(C);

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

} // namespace chestnut
