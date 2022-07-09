#include <algorithm> // std::fill
#include <stdexcept>

namespace chestnut::ecs::internal
{

CSparseSetBase::CSparseSetBase(index_type initSparseSize) noexcept
: m_sparse(initSparseSize, NIL_INDEX)
{

}

CSparseSetBase::CSparseSetBase(const CSparseSetBase& other) noexcept
: m_sparse(other.m_sparse)
{

}

CSparseSetBase& CSparseSetBase::operator=(const CSparseSetBase& other) noexcept
{
    this->m_sparse = other.m_sparse;
    return *this;
}

CSparseSetBase::CSparseSetBase(CSparseSetBase&& other) noexcept
: m_sparse(std::move(other.m_sparse))
{

}

CSparseSetBase& CSparseSetBase::operator=(CSparseSetBase&& other) noexcept
{
    this->m_sparse = std::move(other.m_sparse);
    return *this;
}

const std::vector<int>& CSparseSetBase::sparse() const noexcept
{
    return m_sparse;
}

bool CSparseSetBase::contains(index_type idx) const noexcept
{
    if(idx >= m_sparse.size())
    {
        return false;
    }

    return m_sparse[idx] != NIL_INDEX;
}








template<typename T>
CSparseSet<T>::CSparseSet(index_type initSparseSize) noexcept
: CSparseSetBase(initSparseSize), m_dense()
{

}

template<typename T>
CSparseSet<T>::CSparseSet(const CSparseSet<T>& other) noexcept
: CSparseSetBase(other), m_dense(other.m_dense)
{

}

template<typename T>
CSparseSet<T>& CSparseSet<T>::operator=(const CSparseSet<T>& other) noexcept
{   
    CSparseSetBase::operator=(other);
    this->m_dense = other.m_dense;
    return *this;
}

template<typename T>
CSparseSet<T>::CSparseSet(CSparseSet<T>&& other) noexcept
: CSparseSetBase(std::move(other)), m_dense(std::move(other.m_dense))
{

}

template<typename T>
CSparseSet<T>& CSparseSet<T>::operator=(CSparseSet<T>&& other) noexcept
{
    CSparseSetBase::operator=(std::move(other));
    this->m_dense = std::move(other.m_dense);
    return *this;
}

template<typename T>
const std::vector<T>& CSparseSet<T>::dense() const noexcept
{
    return this->m_dense;
}

template<typename T>
T& CSparseSet<T>::at(index_type idx) 
{
    if(idx >= m_sparse.size() || m_sparse[idx] == NIL_INDEX)
    {
        throw std::runtime_error("no value at index found");
    }

    return this->m_dense[m_sparse[idx]];
}

template<typename T>
const T& CSparseSet<T>::at(index_type idx) const
{
    if(idx >= m_sparse.size() || m_sparse[idx] == NIL_INDEX)
    {
        throw std::runtime_error("no value at index found");
    }

    return this->m_dense[m_sparse[idx]];
}

template<typename T>
bool CSparseSet<T>::empty() const noexcept
{
    return m_dense.empty();
}

template<typename T>
CSparseSetBase::index_type CSparseSet<T>::size() const noexcept
{
    return m_dense.size();
}

template<typename T>
void CSparseSet<T>::clear() noexcept
{
    m_dense.clear();

    std::fill(m_sparse.begin(), m_sparse.end(), NIL_INDEX);
}

template<typename T>
void CSparseSet<T>::insert(index_type idx, T&& arg) noexcept
{
    m_sparse.resize(idx + 1, NIL_INDEX);

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
void CSparseSet<T>::erase(index_type idx) noexcept
{
    if(idx < m_sparse.size() && m_sparse[idx] != NIL_INDEX)
    {
        std::swap(m_dense[m_sparse[idx]], m_dense.back());
        m_dense.pop_back();
        m_sparse[idx] = NIL_INDEX;
    }
}

} // namespace chestnut::ecs::internal
