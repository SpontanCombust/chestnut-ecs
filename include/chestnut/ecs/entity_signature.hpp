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
        std::unordered_set< std::type_index > m_setComponentTypes;

    public:
        template< typename T >
        void add();

        template< typename T >
        void remove();

        template< typename T >
        bool has() const;


        void add( std::type_index compType );

        void remove( std::type_index compType );

        bool has( std::type_index compType ) const;

        
        void add( const CEntitySignature& otherSign );

        void remove( const CEntitySignature& otherSign );

        bool has( const CEntitySignature& otherSign ) const;


        void clear();


        bool isEmpty() const;

        int getSize() const;


        CEntitySignature& operator+=( const CEntitySignature& other );

        CEntitySignature& operator-=( const CEntitySignature& other );

    };

    CEntitySignature operator+( const CEntitySignature& lhs, const CEntitySignature& rhs );

    CEntitySignature operator-( const CEntitySignature& lhs, const CEntitySignature& rhs );

    bool operator==( const CEntitySignature& lhs, const CEntitySignature& rhs );

} // namespace chestnut


#include "entity_signature.tpp"


#endif // __CHESTNUT_ENTITY_SIGNATURE_H__