#include <typelist.hpp>

namespace chestnut::ecs
{    
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
