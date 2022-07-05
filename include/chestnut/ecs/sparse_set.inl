#include <algorithm> // std::fill
#include <stdexcept>

namespace chestnut::ecs::internal
{

template<typename T>
CSparseSet<T>::CSparseSet(unsigned int initSparseSize) noexcept
: m_sparse(initSparseSize, NIL_INDEX), m_dense()
{

}

template<typename T>
CSparseSet<T>::CSparseSet(const CSparseSet<T>& other) noexcept
: m_sparse(other.m_sparse), m_dense(other.m_dense)
{

}

template<typename T>
CSparseSet<T>& CSparseSet<T>::operator=(const CSparseSet<T>& other) noexcept
{   
    this->m_sparse = other.m_sparse;
    this->m_dense = other.m_dense;
    return *this;
}

template<typename T>
CSparseSet<T>::CSparseSet(CSparseSet<T>&& other) noexcept
: m_sparse(std::move(other.m_sparse)), m_dense(std::move(other.m_dense))
{

}

template<typename T>
CSparseSet<T>& CSparseSet<T>::operator=(CSparseSet<T>&& other) noexcept
{
    this->m_sparse = std::move(other.m_sparse);
    this->m_dense = std::move(other.m_dense);
    return *this;
}

template<typename T>
const std::vector<int>& CSparseSet<T>::sparse() const noexcept
{
    return this->m_sparse;
}

template<typename T>
const std::vector<T>& CSparseSet<T>::dense() const noexcept
{
    return this->m_dense;
}

template<typename T>
T& CSparseSet<T>::at(unsigned int idx) 
{
    if(idx >= m_sparse.size() || m_sparse[idx] == NIL_INDEX)
    {
        throw std::runtime_error("no value at index found");
    }

    return this->m_dense[m_sparse[idx]];
}

template<typename T>
const T& CSparseSet<T>::at(unsigned int idx) const
{
    if(idx >= m_sparse.size() || m_sparse[idx] == NIL_INDEX)
    {
        throw std::runtime_error("no value at index found");
    }

    return this->m_dense[m_sparse[idx]];
}

template<typename T>
T& CSparseSet<T>::operator[](unsigned int idx) noexcept
{
    resizeSparce(idx + 1);
    
    if(m_sparse[idx] == NIL_INDEX)
    {
        m_dense.emplace_back();
        m_sparse[idx] = m_dense.size() - 1;
    }

    return m_dense[m_sparse[idx]];
}

template<typename T>
bool CSparseSet<T>::empty() const noexcept
{
    return m_dense.empty();
}

template<typename T>
unsigned int CSparseSet<T>::size() const noexcept
{
    return m_dense.size();
}

template<typename T>
bool CSparseSet<T>::contains(unsigned int idx) const noexcept
{
    if(idx >= m_sparse.size())
    {
        return false;
    }

    return m_sparse[idx] != NIL_INDEX;
}

template<typename T>
void CSparseSet<T>::clear() noexcept
{
    m_dense.clear();

    std::fill(m_sparse.begin(), m_sparse.end(), NIL_INDEX);
}

template<typename T>
void CSparseSet<T>::insert(unsigned int idx, T&& arg) noexcept
{
    resizeSparce(idx + 1);

    if(m_sparse[idx] != NIL_INDEX)
    {
        m_dense[m_sparse[idx]] = std::forward(arg);
    }
    else
    {
        m_dense.push_back(std::forward(arg));
        m_sparse[idx] = m_dense.size() - 1;
    }
}

template<typename T>
void CSparseSet<T>::erase(unsigned int idx) noexcept
{
    if(idx < m_sparse.size() && m_sparse[idx] != NIL_INDEX)
    {
        std::swap(m_dense[m_sparse[idx]], m_dense.back());
        m_dense.pop_back();
        m_sparse[idx] = NIL_INDEX;
    }
}

template<typename T>
inline void CSparseSet<T>::resizeSparce(unsigned int newSize) noexcept
{
    m_sparse.resize(newSize, NIL_INDEX);
}

} // namespace chestnut::ecs::internal
