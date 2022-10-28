#include <algorithm> // stable_sort
#include <cassert>
#include <numeric> // iota

namespace chestnut::ecs
{

inline CEntityQuery::CEntityQuery(internal::CComponentStorage *storagePtr, CEntitySignature requireSignature, CEntitySignature rejectSignature) noexcept
: m_storagePtr(storagePtr), m_requireSignature(requireSignature), m_rejectSignature(rejectSignature)
{

}

inline const CEntitySignature& CEntityQuery::getRejectSignature() const noexcept
{
    return m_rejectSignature;
}

inline const CEntitySignature& CEntityQuery::getRequireSignature() const noexcept
{
    return m_requireSignature;
}

inline entitysize_t CEntityQuery::getEntityCount() const noexcept
{
    return m_vecEntityIDs.size();
}




template<typename ...Types>
CEntityQuery::Iterator<Types...> CEntityQuery::begin() noexcept
{
    assert(m_requireSignature.has<Types...>() && "All types supplied must be in query's 'require' signature");

    CEntitySignature sign = makeEntitySignature<Types...>();
    assert(m_rejectSignature.hasAnyFrom(sign) && "None of the supplied types should be in query's 'reject' signature");

    return Iterator(this, 0);
}

template<typename ...Types>
CEntityQuery::Iterator<Types...> CEntityQuery::end() noexcept
{
    assert(m_requireSignature.has<Types...>() && "All types supplied must be in query's 'require' signature");

    CEntitySignature sign = makeEntitySignature<Types...>();
    assert(m_rejectSignature.hasAnyFrom(sign) && "None of the supplied types should be in query's 'reject' signature");

    return Iterator(this, m_vecEntityIDs.size());
}

template<typename ...Types>
CEntityQuery CEntityQuery::getSortedBy(std::function<bool(CEntityQuery::Iterator<Types...>, CEntityQuery::Iterator<Types...>)> comparator) noexcept
{
    std::vector<unsigned int> indices(m_vecEntityIDs.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::stable_sort(indices.begin(), indices.end(),
        [&](unsigned int idx1, unsigned int idx2) -> bool {
            Iterator<Types...> it1(this, idx1);
            Iterator<Types...> it2(this, idx2);

            return comparator(it1, it2);
        }
    );

    CEntityQuery sortedQuery(this->m_storagePtr, this->m_id, this->m_requireSignature, this->m_rejectSignature);
    for (unsigned int i = 0; i < m_vecEntityIDs.size(); i++)
    {
        sortedQuery.m_vecEntityIDs[i] = this->m_vecEntityIDs[indices[i]];
    }
    
    return sortedQuery;
}

} // namespace chestnut::ecs
