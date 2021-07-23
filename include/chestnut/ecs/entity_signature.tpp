namespace chestnut::ecs
{    
    template<typename T>
    void CEntitySignature::add() 
    {
        m_setComponentTindices.insert( std::type_index( typeid(T) ) );
    }

    template<typename T>
    void CEntitySignature::remove() 
    {
        m_setComponentTindices.erase( std::type_index( typeid(T) ) );
    }

    template<typename T>
    bool CEntitySignature::has() const
    {
        auto it = m_setComponentTindices.find( std::type_index( typeid(T) ) );
        if( it != m_setComponentTindices.end() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
} // namespace chestnut
