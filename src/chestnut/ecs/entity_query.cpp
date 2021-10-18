#include "chestnut/ecs/entity_query.hpp"

namespace chestnut::ecs
{
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid ) > func )
    {
        if( query.vecBatches.empty() )
        {
            return;
        }

        for( const SComponentBatch *batch : query.vecBatches )
        {
            entityid entCount = batch->vecEntityIDs.size();

            for (entityid i = 0; i < entCount; i++)
            {
                func( batch->vecEntityIDs[i] );
            }
        }
    }
    
} // namespace chestnut::ecs
