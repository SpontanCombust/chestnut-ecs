#include "constants.hpp"

namespace chestnut::ecs::internal
{

inline CComponentStorage::CComponentStorage() 
{
    m_highestId = ENTITY_ID_MINIMAL;
}

inline CComponentStorage::~CComponentStorage() 
{
    
}

template<typename T>
inline const std::vector<int>& CComponentStorage::sparse() const noexcept
{
    return getSparseSet<T>().sparse();
}

template<typename T>
inline const std::vector<T>& CComponentStorage::dense() const noexcept
{
    return getSparseSet<T>().dense();
}

template<typename T>
inline T& CComponentStorage::at(entityid_t id) 
{
    return getSparseSet<T>().at(id);
}

template<typename T>
inline const T& CComponentStorage::at(entityid_t id) const
{
    return getSparseSet<T>().at(id);
}

template<typename T>
inline bool CComponentStorage::empty() const noexcept
{
    return getSparseSet<T>().empty();
}

template<typename T>
inline entitysize_t CComponentStorage::size() const noexcept
{
    return (entitysize_t)getSparseSet<T>().size();
}

template<typename T>
inline bool CComponentStorage::contains(entityid_t id) const noexcept
{
    return getSparseSet<T>().contains(id);
}

template<typename T>
inline void CComponentStorage::clear() noexcept
{
    getSparseSet<T>().clear();
}

template<typename T>
inline void CComponentStorage::insert(entityid_t id, T&& arg) noexcept
{
    if(id > m_highestId)
    {
        m_highestId = id;
    }
    
    getSparseSet<T>().insert(id, std::forward<T>(arg));
}

template<typename T>
inline void CComponentStorage::insert(entityid_t id) noexcept
{
    this->insert(id, T());
}

template<typename T>
inline void CComponentStorage::erase(entityid_t id) noexcept
{
    getSparseSet<T>().erase(id);
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




inline void CComponentStorage::eraseAll(entityid_t id) noexcept
{
    for(const auto& [typeIndex, sparseSetBase] : m_mapTypeToSparseSet)
    {
        sparseSetBase->erase(id);
    }   
}

inline CEntitySignature CComponentStorage::signature(entityid_t id) const noexcept
{
    CEntitySignature sign;

    for(const auto& [typeIndex, sparseSetBase] : m_mapTypeToSparseSet)
    {
        if(sparseSetBase->contains(id))
        {
            sign.add(typeIndex);
        }
    }

    return sign;
}





inline CComponentStorage::Iterator CComponentStorage::begin() noexcept
{
    return CComponentStorage::Iterator(this, ENTITY_ID_MINIMAL);
}

inline CComponentStorage::Iterator CComponentStorage::end() noexcept
{
    return CComponentStorage::Iterator(this, m_highestId + 1);
}

inline CComponentStorage::ConstIterator CComponentStorage::cbegin() const noexcept
{
    return CComponentStorage::ConstIterator(this, ENTITY_ID_MINIMAL);
}

inline CComponentStorage::ConstIterator CComponentStorage::cend() const noexcept
{
    return CComponentStorage::ConstIterator(this, m_highestId + 1);
}

} // namespace chestnut::ecs::internal