#include "constants.hpp"
#include "native_components.hpp"

namespace chestnut::ecs::internal
{
    inline CComponentStorage::CComponentStorage() 
    {
        
    }

    inline CComponentStorage::~CComponentStorage() 
    {
        
    }




    template<typename T>
    inline tl::expected<T*, std::string> CComponentStorage::at(entityslot_t slot) 
    {
        return getSparseSet<T>().at(slot);
    }

    template<typename T>
    inline tl::expected<const T*, std::string> CComponentStorage::at(entityslot_t slot) const
    {
        return getSparseSet<T>().at(slot);
    }

    template<typename T>
    inline bool CComponentStorage::empty() const noexcept
    {
        return getSparseSet<T>().empty();
    }

    template<typename T>
    inline size_t CComponentStorage::size() const noexcept
    {
        return getSparseSet<T>().denseSize();
    }

    inline size_t CComponentStorage::maxSlot() const noexcept
    {
        size_t max = 0;
        for(const auto& [_, ss] : m_mapTypeToSparseSet)
        {
            max = std::max(max, ss->sparseSize());
        }
        
        return max;
    }

    template<typename T>
    inline bool CComponentStorage::contains(entityslot_t slot) const noexcept
    {
        return getSparseSet<T>().contains(slot);
    }

    template<typename T>
    inline void CComponentStorage::clear() noexcept
    {
        getSparseSet<T>().clear();
    }

    template<typename T>
    inline void CComponentStorage::insert(entityslot_t slot, T&& arg) noexcept
    {   
        getSparseSet<T>().insert(slot, std::forward<T>(arg));
    }

    template<typename T>
    inline void CComponentStorage::insert(entityslot_t slot) noexcept
    {
        this->insert(slot, T());
    }

    template<typename T>
    inline void CComponentStorage::erase(entityslot_t slot) noexcept
    {
        getSparseSet<T>().erase(slot);
    }

    template<typename T>
    inline CSparseSet<T>& CComponentStorage::getSparseSet() const noexcept
    {
        const auto TYPE_INDEX = std::type_index(typeid(T));

        CSparseSet<T> *sparseSetPtr;
        if(m_mapTypeToSparseSet.find(TYPE_INDEX) == m_mapTypeToSparseSet.end())
        {
            sparseSetPtr = new CSparseSet<T>();
            m_mapTypeToSparseSet[TYPE_INDEX] = std::move(std::unique_ptr<CSparseSet<T>>(sparseSetPtr));
        }
        else
        {
            sparseSetPtr = static_cast<CSparseSet<T> *>(m_mapTypeToSparseSet[TYPE_INDEX].get());
        }

        return *sparseSetPtr;
    }




    inline void CComponentStorage::eraseAll(entityslot_t slot) noexcept
    {
        for(const auto& [typeIndex, sparseSetBase] : m_mapTypeToSparseSet)
        {
            sparseSetBase->erase(slot);
        }   
    }

    inline tl::optional<CEntitySignature> CComponentStorage::signature(entityslot_t slot) const noexcept
    {
        if (!getSparseSet<CIdentityComponent>().contains(slot))
        {
            return tl::nullopt;
        }

        CEntitySignature sign;

        for(const auto& [typeIndex, sparseSetBase] : m_mapTypeToSparseSet)
        {
            if(sparseSetBase->contains(slot))
            {
                sign.add(typeIndex);
            }
        }

        return sign;
    }

} // namespace chestnut::ecs::internal