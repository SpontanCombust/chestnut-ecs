namespace chestnut
{
    template< class C1 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                func( *c1 );
            }
        }
    }

    template< class C1 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1* ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                func( c1 );
            }
        }
    }

    template< class C1, class C2 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1&, C2& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< CComponent * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                C2 *c2 = static_cast< C2* >( vecC2[i] );
                func( *c1, *c2 );
            }
        }
    }

    template< class C1, class C2 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1*, C2* ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< CComponent * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                C2 *c2 = static_cast< C2* >( vecC2[i] );
                func( c1, c2 );
            }
        }
    }

    template< class C1, class C2, class C3 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1&, C2&, C3& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< CComponent * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< CComponent * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                C2 *c2 = static_cast< C2* >( vecC2[i] );
                C3 *c3 = static_cast< C3* >( vecC3[i] );
                func( *c1, *c2, *c3 );
            }
        }
    }

    template< class C1, class C2, class C3 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1*, C2*, C3* ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< CComponent * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< CComponent * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                C2 *c2 = static_cast< C2* >( vecC2[i] );
                C3 *c3 = static_cast< C3* >( vecC3[i] );
                func( c1, c2, c3 );
            }
        }
    }

    template< class C1, class C2, class C3, class C4 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1&, C2&, C3&, C4& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< CComponent * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< CComponent * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );
            const std::vector< CComponent * >& vecC4 = batch->mapCompTypeToCompVec.at( typeid(C4) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                C2 *c2 = static_cast< C2* >( vecC2[i] );
                C3 *c3 = static_cast< C3* >( vecC3[i] );
                C4 *c4 = static_cast< C4* >( vecC4[i] );
                func( *c1, *c2, *c3, *c4 );
            }
        }
    }

    template< class C1, class C2, class C3, class C4 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1*, C2*, C3*, C4* ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< CComponent * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< CComponent * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< CComponent * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );
            const std::vector< CComponent * >& vecC4 = batch->mapCompTypeToCompVec.at( typeid(C4) );

            for (entityid i = 0; i < entCount; i++)
            {
                C1 *c1 = static_cast< C1* >( vecC1[i] );
                C2 *c2 = static_cast< C2* >( vecC2[i] );
                C3 *c3 = static_cast< C3* >( vecC3[i] );
                C4 *c4 = static_cast< C4* >( vecC4[i] );
                func( c1, c2, c3, c4 );
            }
        }
    }
    
} // namespace chestnut
