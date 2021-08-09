namespace chestnut
{    
    template<typename T>
    void CEntitySignature::add() 
    {
        m_setComponentTypes.insert( std::type_index( typeid(T) ) );
    }

    template<typename T>
    void CEntitySignature::remove() 
    {
        m_setComponentTypes.erase( std::type_index( typeid(T) ) );
    }

    template<typename T>
    bool CEntitySignature::has() const
    {
        auto it = m_setComponentTypes.find( std::type_index( typeid(T) ) );
        if( it != m_setComponentTypes.end() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
} // namespace chestnut
