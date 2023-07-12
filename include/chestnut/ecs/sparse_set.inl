#include <algorithm> // std::fill
#include <exception>

namespace chestnut::ecs::internal
{
inline CSparseSetBase::CSparseSetBase(size_t initSparseSize) noexcept 
: m_sparse(initSparseSize, NIL_INDEX)
{

}

inline const std::vector<int> &CSparseSetBase::sparse() const noexcept
{
    return this->m_sparse;
}

inline size_t CSparseSetBase::sparseSize() const noexcept
{
    return this->m_sparse.size();
}

inline bool CSparseSetBase::contains(size_t idx) const noexcept
{
    if(idx >= this->m_sparse.size())
    {
        return false;
    }

    return this->m_sparse[idx] != NIL_INDEX;
}

inline void CSparseSetBase::erase(size_t idx) noexcept
{
    if(idx < this->m_sparse.size())
    {
       this->m_sparse[idx] = NIL_INDEX;   
    }
}





template<typename T>
inline CSparseSet<T>::CSparseSet(size_t initSparseSize) noexcept
: CSparseSetBase(initSparseSize), m_densePointers(), m_dense()
{

}

template<typename T>
inline const std::vector<T>& CSparseSet<T>::dense() const noexcept
{
    return this->m_dense;
}

template<typename T>
inline size_t CSparseSet<T>::denseSize() const noexcept
{
    return this->m_dense.size();
}

template<typename T>
inline tl::expected<T*, std::string> CSparseSet<T>::at(size_t idx) 
{
    if (idx >= this->m_sparse.size())
    {
        return tl::make_unexpected("Index " + std::to_string(idx) + " out of range");
    }
    else if (this->m_sparse[idx] == NIL_INDEX)
    {
        return tl::make_unexpected("No value at index " + std::to_string(idx));
    }
    else
    {
        return &this->m_dense[m_sparse[idx]];
    }
}

template<typename T>
inline tl::expected<const T*, std::string> CSparseSet<T>::at(size_t idx) const
{
    if (idx >= this->m_sparse.size())
    {
        return tl::make_unexpected("Index " + std::to_string(idx) + " out of range");
    }
    else if (this->m_sparse[idx] == NIL_INDEX)
    {
        return tl::make_unexpected("No value at index " + std::to_string(idx));
    }
    else
    {
        return &this->m_dense[m_sparse[idx]];
    }
}

template<typename T>
inline bool CSparseSet<T>::empty() const noexcept
{
    return m_dense.empty();
}

template<typename T>
inline void CSparseSet<T>::clear() noexcept
{
    m_dense.clear();
    m_densePointers.clear();

    std::fill(m_sparse.begin(), m_sparse.end(), NIL_INDEX);
}

template<typename T>
inline void CSparseSet<T>::insert(size_t idx, T&& arg) noexcept
{
    if(idx >= m_sparse.size())
    {
        m_sparse.resize(idx + 1, NIL_INDEX);
    }

    if(m_sparse[idx] != NIL_INDEX)
    {
        m_dense[m_sparse[idx]] = std::forward<T>(arg);
        m_densePointers[m_sparse[idx]] = idx;
    }
    else
    {
        m_dense.push_back(std::forward<T>(arg));
        m_densePointers.push_back(idx);
        m_sparse[idx] = (int)m_dense.size() - 1;
    }
}

template<typename T>
inline void CSparseSet<T>::erase(size_t idx) noexcept
{
    if(idx < m_sparse.size() && m_sparse[idx] != NIL_INDEX)
    {
        m_sparse[m_densePointers.back()] = m_sparse[idx];
        std::swap(m_dense[m_sparse[idx]], m_dense.back());
        std::swap(m_densePointers[m_sparse[idx]], m_densePointers.back());
        m_dense.pop_back();
        m_densePointers.pop_back();
        m_sparse[idx] = NIL_INDEX;
    }
}

} // namespace chestnut::ecs::internal
