#include "chestnut/ecs/entity_signature.hpp"

#include <sstream>

namespace chestnut
{
    void CEntitySignature::__add( std::type_index compType ) 
    {
        m_setComponentTypes.insert( compType );
    }

    void CEntitySignature::__remove( std::type_index compType ) 
    {
        m_setComponentTypes.erase( compType );
    }

    bool CEntitySignature::__has( std::type_index compType ) const
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
        for( auto tindex : other.m_setComponentTypes )
        {
            m_setComponentTypes.insert( tindex );
        }

        return *this;
    }

    CEntitySignature& CEntitySignature::operator-=(const CEntitySignature& other) 
    {
        for( auto tindex : other.m_setComponentTypes )
        {
            m_setComponentTypes.erase( tindex );
        }

        return *this;
    }

    CEntitySignature operator+( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        CEntitySignature newSign = lhs;

        newSign += rhs;

        return newSign;
    }

    CEntitySignature operator-( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        CEntitySignature newSign = lhs;

        newSign -= rhs;

        return newSign;
    }

    bool operator==( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        return lhs.m_setComponentTypes == rhs.m_setComponentTypes;
    }

} // namespace chestnut
