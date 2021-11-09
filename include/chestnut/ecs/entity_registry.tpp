namespace chestnut::ecs::internal
{
    inline entityid CEntityRegistry::entityIdFromIndex( entityid index ) const
    {
        return index + ENTITY_ID_MINIMAL;
    }

    inline entityid CEntityRegistry::indexFromEntityId( entityid id ) const
    {
        return id - ENTITY_ID_MINIMAL;
    }

} // namespace chestnut::ecs::internal
