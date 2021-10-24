namespace chestnut::ecs
{    
    template< typename T, typename ...Rest >
    void CEntitySignature::addList( TypeList<T, Rest...> ) 
    {
        add( typeid(T) );
        addList( TypeList<Rest...>() );
    }

    template< typename T, typename ...Rest >
    void CEntitySignature::removeList( TypeList<T, Rest...> ) 
    {
        remove( typeid(T) );
        removeList( TypeList<Rest...>() );
    }

    template< typename T, typename ...Rest >
    bool CEntitySignature::hasList( TypeList<T, Rest...> ) const
    {
        return has( typeid(T) ) && hasList( TypeList<Rest...>() );
    }


    template< typename ...Types >
    void CEntitySignature::add() 
    {
        addList( TypeList<Types...>() );
    }

    template< typename ...Types >
    void CEntitySignature::remove() 
    {
        removeList( TypeList<Types...>() );
    }

    template< typename ...Types >
    bool CEntitySignature::has() const
    {
        return hasList( TypeList<Types...>() );
    }
    

    template< typename ...Types >
    CEntitySignature makeEntitySignature()
    {
        CEntitySignature sign;
        sign.add<Types...>();
        return sign;
    }

} // namespace chestnut::ecs
