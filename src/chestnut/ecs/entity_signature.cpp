#include "chestnut/ecs/entity_signature.hpp"

namespace chestnut::ecs
{
    void CEntitySignature::add( std::type_index compType ) 
    {
        m_setComponentTypes.insert( compType );
    }

    void CEntitySignature::remove( std::type_index compType ) 
    {
        m_setComponentTypes.erase( compType );
    }

    bool CEntitySignature::has( std::type_index compType ) const
    {
        auto it = m_setComponentTypes.find( compType );
        if( it != m_setComponentTypes.end() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void CEntitySignature::addFrom( const CEntitySignature& otherSign ) 
    {
        for( std::type_index type : otherSign.m_setComponentTypes )
        {
            m_setComponentTypes.insert( type );
        }
    }

    void CEntitySignature::removeFrom( const CEntitySignature& otherSign ) 
    {
        for( std::type_index type : otherSign.m_setComponentTypes )
        {
            m_setComponentTypes.erase( type );
        }
    }

    bool CEntitySignature::hasAllFrom( const CEntitySignature& otherSign ) const
    {
        bool areAllPresent = true;

        for( std::type_index type : otherSign.m_setComponentTypes )
        {
            if( !has( type ) )
            {
                areAllPresent = false;
                break;
            }
        }

        return areAllPresent;
    }

    bool CEntitySignature::hasAnyFrom( const CEntitySignature& otherSign ) const
    {
        for( std::type_index type : otherSign.m_setComponentTypes )
        {
            if( has( type ) )
            {
                return true;
            }
        }

        return false;
    }

    void CEntitySignature::clear() 
    {
        m_setComponentTypes.clear();
    }

    bool CEntitySignature::isEmpty() const
    {
        return m_setComponentTypes.empty();
    }

    int CEntitySignature::getSize() const
    {
        return m_setComponentTypes.size();
    }

    CEntitySignature& CEntitySignature::operator+=( const CEntitySignature& other ) 
    {
        addFrom( other );
        return *this;
    }

    CEntitySignature& CEntitySignature::operator-=(const CEntitySignature& other) 
    {
        removeFrom( other );
        return *this;
    }

    CEntitySignature operator+( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        CEntitySignature newSign = lhs;
        newSign.addFrom( rhs );
        return newSign;
    }

    CEntitySignature operator-( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        CEntitySignature newSign = lhs;
        newSign.removeFrom( rhs );
        return newSign;
    }

    bool operator==( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        return lhs.m_setComponentTypes == rhs.m_setComponentTypes;
    }

} // namespace chestnut::ecs
