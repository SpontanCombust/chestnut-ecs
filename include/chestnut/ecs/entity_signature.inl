#include "entity_signature.hpp"

#include <algorithm>

namespace chestnut::ecs
{
    inline CEntitySignature::CEntitySignature()
    : m_vecTypes()
    {

    }

    inline void CEntitySignature::addImpl(std::type_index type)
    {
        auto it = std::find(this->m_vecTypes.begin(), this->m_vecTypes.end(), type);
        if (it == this->m_vecTypes.end())
            this->m_vecTypes.push_back(type);
    }

    inline void CEntitySignature::removeImpl(std::type_index type)
    {
        auto it = std::find(this->m_vecTypes.begin(), this->m_vecTypes.end(), type);
        if (it != this->m_vecTypes.end())
            this->m_vecTypes.erase(it);
    }

    inline void CEntitySignature::sort()
    {
        std::sort(this->m_vecTypes.begin(), this->m_vecTypes.end());
    }

    inline CEntitySignature& chestnut::ecs::CEntitySignature::add(std::type_index type)
    {
        this->addImpl(type);
        this->sort();
        return *this;
    }

    inline CEntitySignature& CEntitySignature::remove(std::type_index type)
    {
        this->removeImpl(type);
        this->sort();
        return *this;
    }

    inline bool CEntitySignature::has(std::type_index type) const
    {
        return std::binary_search(this->m_vecTypes.begin(), this->m_vecTypes.end(), type);
    }


    inline CEntitySignature& CEntitySignature::add(const CEntitySignature &other)
    {
        std::vector<std::type_index> result;
        std::set_union(this->m_vecTypes.begin(), this->m_vecTypes.end(),
                       other.m_vecTypes.begin(), other.m_vecTypes.end(),
                       std::back_inserter(result));
        this->m_vecTypes = std::move(result);

        return *this;
    }

    inline CEntitySignature& CEntitySignature::remove(const CEntitySignature &other)
    {
        std::vector<std::type_index> result;
        std::set_difference(this->m_vecTypes.begin(), this->m_vecTypes.end(),
                            other.m_vecTypes.begin(), other.m_vecTypes.end(),
                            std::back_inserter(result));   
        this->m_vecTypes = std::move(result);

        return *this;
    }

    inline bool CEntitySignature::has(const CEntitySignature &other) const
    {
        for(std::type_index type : other.m_vecTypes)
        {
            if (!this->has(type))
            {
                return false;
            }
        }

        return true;
    }


    inline CEntitySignature &CEntitySignature::clear()
    {
        this->m_vecTypes.clear();
        return *this;
    }

    inline bool CEntitySignature::empty() const
    {
        return this->m_vecTypes.empty();
    }

    inline size_t CEntitySignature::size() const
    {
        return this->m_vecTypes.size();
    }




    template<typename Type, typename ...Rest>
    inline CEntitySignature CEntitySignature::from()
    {
        CEntitySignature sign;
        sign.add<Type, Rest...>();
        return sign;
    }


    template<typename ...Types>
    inline CEntitySignature& CEntitySignature::add()
    {
        (this->addImpl(typeid(Types)), ...);
        this->sort();
        return *this;
    }

    template<typename ...Types>
    inline CEntitySignature& CEntitySignature::remove()
    {
        (this->removeImpl(typeid(Types)), ...);
        this->sort();
        return *this;
    }

    template<typename ...Types>
    inline bool CEntitySignature::has() const
    {
        return (this->has(typeid(Types)) && ...);
    }




    inline bool CEntitySignature::operator==(const CEntitySignature& other) const
    {
        return this->m_vecTypes == other.m_vecTypes;
    }

    inline bool CEntitySignature::operator!=(const CEntitySignature& other) const
    {
        return this->m_vecTypes != other.m_vecTypes;
    }


    inline CEntitySignature& CEntitySignature::operator+=(const CEntitySignature& other)
    {
        return this->add(other);
    }

    inline CEntitySignature& CEntitySignature::operator-=(const CEntitySignature& other)
    {
        return this->remove(other);
    }

    inline CEntitySignature CEntitySignature::operator+(const CEntitySignature &other) const
    {
        CEntitySignature sign = *this;
        sign.add(other);
        return sign;
    }

    inline CEntitySignature CEntitySignature::operator-(const CEntitySignature &other) const
    {
        CEntitySignature sign = *this;
        sign.remove(other);
        return sign;
    }

    inline CEntitySignature CEntitySignature::operator&(const CEntitySignature &other) const
    {
        std::vector<std::type_index> result;
        std::set_intersection(this->m_vecTypes.begin(), this->m_vecTypes.end(),
                              other.m_vecTypes.begin(), other.m_vecTypes.end(),
                              std::back_inserter(result));

        CEntitySignature sign;
        sign.m_vecTypes = std::move(result);
        
        return sign;
    }




    inline CEntitySignature::Iterator CEntitySignature::begin() const
    {
        return this->m_vecTypes.cbegin();
    }

    inline CEntitySignature::Iterator CEntitySignature::end() const
    {
        return this->m_vecTypes.cend();
    }

} // namespace chestnut::ecs
