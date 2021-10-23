#ifndef __CHESTNUT_ECS_ENTITY_SIGNATURE_H__
#define __CHESTNUT_ECS_ENTITY_SIGNATURE_H__

#include <string>
#include <typeindex>
#include <unordered_set>

namespace chestnut::ecs
{
    class CEntitySignature
    {
    public:
        std::unordered_set< std::type_index > m_setComponentTypes;

    public:
        template< typename ...Types >
        void add();

        template< typename ...Types >
        void remove();

        template< typename ...Types >
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


    private:
        template< typename ... >
        struct TypeList {};

        template< typename T, typename ...Rest >
        void addList( TypeList<T, Rest...> );

        inline void addList( TypeList<> ) {}

        template< typename T, typename ...Rest >
        void removeList( TypeList<T, Rest...> );

        inline void removeList( TypeList<> ) {} 

        template< typename T, typename ...Rest >
        bool hasList( TypeList<T, Rest...> ) const;

        inline bool hasList( TypeList<> ) const { return true; }
    };

    CEntitySignature operator+( const CEntitySignature& lhs, const CEntitySignature& rhs );

    CEntitySignature operator-( const CEntitySignature& lhs, const CEntitySignature& rhs );

    bool operator==( const CEntitySignature& lhs, const CEntitySignature& rhs );


    template< typename ...Types >
    CEntitySignature makeEntitySignature();

} // namespace chestnut::ecs


#include "entity_signature.tpp"


#endif // __CHESTNUT_ECS_ENTITY_SIGNATURE_H__