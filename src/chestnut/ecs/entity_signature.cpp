#include "entity_signature.hpp"

#include <sstream>

namespace chestnut
{
namespace ecs
{
    
    bool CEntitySignature::isEmpty() 
    {
        return m_setComponentTindices.empty();
    }

    CEntitySignature& CEntitySignature::operator+=( const CEntitySignature& other ) 
    {
        for( auto tindex : other.m_setComponentTindices )
        {
            m_setComponentTindices.insert( tindex );
        }
    }

    CEntitySignature& CEntitySignature::operator-=(const CEntitySignature& other) 
    {
        for( auto tindex : other.m_setComponentTindices )
        {
            m_setComponentTindices.erase( tindex );
        }
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
        return lhs.m_setComponentTindices == rhs.m_setComponentTindices;
    }

} // namespace ecs
   
} // namespace chestnut
