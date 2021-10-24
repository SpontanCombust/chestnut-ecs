namespace chestnut::ecs
{
    template< class C1 >
    void CEntityQuery::forEachEntityWith( std::function< void( C1& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            func( c1->data );
        }
    }

    template< class C1, class C2 >
    void CEntityQuery::forEachEntityWith( std::function< void( C1&, C2& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );
        const std::vector< internal::IComponentWrapper * >& vecC2 = m_mapCompTypeToVecComp.at( typeid(C2) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
            func( c1->data, c2->data );
        }
    }

    template< class C1, class C2, class C3 >
    void CEntityQuery::forEachEntityWith( std::function< void( C1&, C2&, C3& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );
        const std::vector< internal::IComponentWrapper * >& vecC2 = m_mapCompTypeToVecComp.at( typeid(C2) );
        const std::vector< internal::IComponentWrapper * >& vecC3 = m_mapCompTypeToVecComp.at( typeid(C3) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
            internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
            func( c1->data, c2->data, c3->data );
        }
    }

    template< class C1, class C2, class C3, class C4 >
    void CEntityQuery::forEachEntityWith( std::function< void( C1&, C2&, C3&, C4& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );
        const std::vector< internal::IComponentWrapper * >& vecC2 = m_mapCompTypeToVecComp.at( typeid(C2) );
        const std::vector< internal::IComponentWrapper * >& vecC3 = m_mapCompTypeToVecComp.at( typeid(C3) );
        const std::vector< internal::IComponentWrapper * >& vecC4 = m_mapCompTypeToVecComp.at( typeid(C4) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
            internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
            internal::SComponentWrapper<C4> *c4 = static_cast< internal::SComponentWrapper<C4>* >( vecC4[i] );
            func( c1->data, c2->data, c3->data, c4->data );
        }
    }





    
    template< class C1 >
    void CEntityQuery::forEachEntityWith( std::function< void( entityid, C1& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            func( m_vecEntityIDs[i], c1->data );
        }
    }

    template< class C1, class C2 >
    void CEntityQuery::forEachEntityWith( std::function< void( entityid, C1&, C2& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );
        const std::vector< internal::IComponentWrapper * >& vecC2 = m_mapCompTypeToVecComp.at( typeid(C2) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
            func( m_vecEntityIDs[i], c1->data, c2->data );
        }
    }

    template< class C1, class C2, class C3 >
    void CEntityQuery::forEachEntityWith( std::function< void( entityid, C1&, C2&, C3& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );
        const std::vector< internal::IComponentWrapper * >& vecC2 = m_mapCompTypeToVecComp.at( typeid(C2) );
        const std::vector< internal::IComponentWrapper * >& vecC3 = m_mapCompTypeToVecComp.at( typeid(C3) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
            internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
            func( m_vecEntityIDs[i], c1->data, c2->data, c3->data );
        }
    }

    template< class C1, class C2, class C3, class C4 >
    void CEntityQuery::forEachEntityWith( std::function< void( entityid, C1&, C2&, C3&, C4& ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();
        const std::vector< internal::IComponentWrapper * >& vecC1 = m_mapCompTypeToVecComp.at( typeid(C1) );
        const std::vector< internal::IComponentWrapper * >& vecC2 = m_mapCompTypeToVecComp.at( typeid(C2) );
        const std::vector< internal::IComponentWrapper * >& vecC3 = m_mapCompTypeToVecComp.at( typeid(C3) );
        const std::vector< internal::IComponentWrapper * >& vecC4 = m_mapCompTypeToVecComp.at( typeid(C4) );

        for (entityid i = 0; i < entCount; i++)
        {
            internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
            internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
            internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
            internal::SComponentWrapper<C4> *c4 = static_cast< internal::SComponentWrapper<C4>* >( vecC4[i] );
            func( m_vecEntityIDs[i], c1->data, c2->data, c3->data, c4->data );
        }
    }

} // namespace chestnut::ecs
