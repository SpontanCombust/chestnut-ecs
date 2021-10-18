namespace chestnut::ecs
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
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                func( c1->data );
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
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< internal::IComponentWrapper * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
                func( c1->data, c2->data );
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
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< internal::IComponentWrapper * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< internal::IComponentWrapper * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
                internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
                func( c1->data, c2->data, c3->data );
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
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< internal::IComponentWrapper * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< internal::IComponentWrapper * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );
            const std::vector< internal::IComponentWrapper * >& vecC4 = batch->mapCompTypeToCompVec.at( typeid(C4) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
                internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
                internal::SComponentWrapper<C4> *c4 = static_cast< internal::SComponentWrapper<C4>* >( vecC4[i] );
                func( c1->data, c2->data, c3->data, c4->data );
            }
        }
    }
    



    template< class C1 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                func( batch->vecEntityIDs[i], c1->data );
            }
        }
    }

    template< class C1, class C2 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1&, C2& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< internal::IComponentWrapper * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
                func( batch->vecEntityIDs[i], c1->data, c2->data );
            }
        }
    }

    template< class C1, class C2, class C3 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1&, C2&, C3& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< internal::IComponentWrapper * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< internal::IComponentWrapper * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
                internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
                func( batch->vecEntityIDs[i], c1->data, c2->data, c3->data );
            }
        }
    }

    template< class C1, class C2, class C3, class C4 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1&, C2&, C3&, C4& ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();
            const std::vector< internal::IComponentWrapper * >& vecC1 = batch->mapCompTypeToCompVec.at( typeid(C1) );
            const std::vector< internal::IComponentWrapper * >& vecC2 = batch->mapCompTypeToCompVec.at( typeid(C2) );
            const std::vector< internal::IComponentWrapper * >& vecC3 = batch->mapCompTypeToCompVec.at( typeid(C3) );
            const std::vector< internal::IComponentWrapper * >& vecC4 = batch->mapCompTypeToCompVec.at( typeid(C4) );

            for (entityid i = 0; i < entCount; i++)
            {
                internal::SComponentWrapper<C1> *c1 = static_cast< internal::SComponentWrapper<C1>* >( vecC1[i] );
                internal::SComponentWrapper<C2> *c2 = static_cast< internal::SComponentWrapper<C2>* >( vecC2[i] );
                internal::SComponentWrapper<C3> *c3 = static_cast< internal::SComponentWrapper<C3>* >( vecC3[i] );
                internal::SComponentWrapper<C4> *c4 = static_cast< internal::SComponentWrapper<C4>* >( vecC4[i] );
                func( batch->vecEntityIDs[i], c1->data, c2->data, c3->data, c4->data );
            }
        }
    }

} // namespace chestnut::ecs
