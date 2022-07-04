#include <typelist.hpp>

namespace chestnut::ecs
{    
    inline void CEntitySignature::add( std::type_index compType ) 
    {
        m_setComponentTypes.insert( compType );
    }

    inline void CEntitySignature::remove( std::type_index compType ) 
    {
        m_setComponentTypes.erase( compType );
    }

    inline bool CEntitySignature::has( std::type_index compType ) const
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

    inline void CEntitySignature::addFrom( const CEntitySignature& otherSign ) 
    {
        for( std::type_index type : otherSign.m_setComponentTypes )
        {
            m_setComponentTypes.insert( type );
        }
    }

    inline void CEntitySignature::removeFrom( const CEntitySignature& otherSign ) 
    {
        for( std::type_index type : otherSign.m_setComponentTypes )
        {
            m_setComponentTypes.erase( type );
        }
    }

    inline bool CEntitySignature::hasAllFrom( const CEntitySignature& otherSign ) const
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

    inline bool CEntitySignature::hasAnyFrom( const CEntitySignature& otherSign ) const
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

    inline void CEntitySignature::clear() 
    {
        m_setComponentTypes.clear();
    }

    inline bool CEntitySignature::isEmpty() const
    {
        return m_setComponentTypes.empty();
    }

    inline int CEntitySignature::getSize() const
    {
        return m_setComponentTypes.size();
    }

    inline CEntitySignature& CEntitySignature::operator+=( const CEntitySignature& other ) 
    {
        addFrom( other );
        return *this;
    }

    inline CEntitySignature& CEntitySignature::operator-=(const CEntitySignature& other) 
    {
        removeFrom( other );
        return *this;
    }

    inline CEntitySignature operator+( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        CEntitySignature newSign = lhs;
        newSign.addFrom( rhs );
        return newSign;
    }

    inline CEntitySignature operator-( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        CEntitySignature newSign = lhs;
        newSign.removeFrom( rhs );
        return newSign;
    }

    inline bool operator==( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        return lhs.m_setComponentTypes == rhs.m_setComponentTypes;
    }

    inline bool operator!=( const CEntitySignature& lhs, const CEntitySignature& rhs )
    {
        return lhs.m_setComponentTypes != rhs.m_setComponentTypes;
    }




    template< typename ...Types >
    void CEntitySignature::add() 
    {
        using list = tl::type_list<Types...>;
        list::for_each( [&](auto t)
        {
            add( typeid( typename decltype(t)::type ) );
        });
    }

    template< typename ...Types >
    void CEntitySignature::remove() 
    {
        using list = tl::type_list<Types...>;
        list::for_each( [&](auto t)
        {
            remove( typeid( typename decltype(t)::type ) );
        });
    }

    template< typename ...Types >
    bool CEntitySignature::has() const
    {
        bool _has = true;

        using list = tl::type_list<Types...>;
        list::for_each( [&](auto t)
        {
            _has = has( typeid( typename decltype(t)::type ) );
            
            if( !_has ) return;
        });

        return _has;
    }
    

    template< typename ...Types >
    CEntitySignature makeEntitySignature()
    {
        CEntitySignature sign;
        sign.add<Types...>();
        return sign;
    }

} // namespace chestnut::ecs
