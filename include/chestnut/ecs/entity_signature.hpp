#ifndef __CHESTNUT_ENTITY_SIGNATURE_H__
#define __CHESTNUT_ENTITY_SIGNATURE_H__

#include <string>
#include <typeindex>
#include <unordered_set>

namespace chestnut
{
    class CEntitySignature
    {
    public:
        std::unordered_set< std::type_index > m_setComponentTindices;

    public:
        template< typename T >
        void add();

        template< typename T >
        void remove();

        template< typename T >
        bool has() const;


        bool isEmpty();


        CEntitySignature& operator+=( const CEntitySignature& other );

        CEntitySignature& operator-=( const CEntitySignature& other );
    };
    
    CEntitySignature operator+( const CEntitySignature& lhs, const CEntitySignature& rhs );

    CEntitySignature operator-( const CEntitySignature& lhs, const CEntitySignature& rhs );

    bool operator==( const CEntitySignature& lhs, const CEntitySignature& rhs );

} // namespace chestnut


#include "entity_signature.tpp"


#endif // __CHESTNUT_ENTITY_SIGNATURE_H__