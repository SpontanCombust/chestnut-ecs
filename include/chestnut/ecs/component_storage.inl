#include "constants.hpp"

namespace chestnut::ecs::internal
{

inline CComponentStorage::CComponentStorage() 
{
    m_highestId = ENTITY_ID_MINIMAL;
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
inline void CComponentStorage::erase(entityid_t id) noexcept
{
    getSparseSet<T>().erase(id);
}

template<typename T>
CSparseSet<T>& CComponentStorage::getSparseSet() const noexcept
{
    const auto TYPE_INDEX = std::type_index(typeid(T));

    CSparseSet<T> *sparseSetPtr;
    if(m_mapTypeToSparseSet.find(TYPE_INDEX) == m_mapTypeToSparseSet.end())
    {
        sparseSetPtr = new CSparseSet<T>();
        m_mapTypeToSparseSet[TYPE_INDEX] = static_cast<void *>(sparseSetPtr);
    }
    else
    {
        sparseSetPtr = static_cast<CSparseSet<T> *>(m_mapTypeToSparseSet[TYPE_INDEX]);
    }

    return *sparseSetPtr;
}




inline CComponentStorage::CIterator CComponentStorage::begin() noexcept
{
    return CComponentStorage::CIterator(this, ENTITY_ID_MINIMAL);
}

inline CComponentStorage::CIterator CComponentStorage::end() noexcept
{
    return CComponentStorage::CIterator(this, m_highestId + 1);
}

inline CComponentStorage::CConstIterator CComponentStorage::cbegin() const noexcept
{
    return CComponentStorage::CConstIterator(this, ENTITY_ID_MINIMAL);
}

inline CComponentStorage::CConstIterator CComponentStorage::cend() const noexcept
{
    return CComponentStorage::CConstIterator(this, m_highestId + 1);
}

} // namespace chestnut::ecs::internal