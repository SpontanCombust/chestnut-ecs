namespace chestnut::ecs::internal
{
    inline entityid_t CEntityRegistry::entityIdFromIndex( entityid_t index ) const
    {
        return index + ENTITY_ID_MINIMAL;
    }

    inline entityid_t CEntityRegistry::indexFromEntityId( entityid_t id ) const
    {
        return id - ENTITY_ID_MINIMAL;
    }

} // namespace chestnut::ecs::internal
