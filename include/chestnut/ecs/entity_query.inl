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
    return (entitysize_t)m_vecEntityIDs.size();
}




template<typename ...Types>
CEntityQuery::Iterator<Types...> CEntityQuery::begin() noexcept
{
    assert(m_requireSignature.has<Types...>() && "All types supplied must be in query's 'require' signature");

    CEntitySignature sign = makeEntitySignature<Types...>();
    assert(!m_rejectSignature.hasAnyFrom(sign) && "None of the supplied types should be in query's 'reject' signature");

    return Iterator<Types...>(this, 0);
}

template<typename ...Types>
CEntityQuery::Iterator<Types...> CEntityQuery::end() noexcept
{
    assert(m_requireSignature.has<Types...>() && "All types supplied must be in query's 'require' signature");

    CEntitySignature sign = makeEntitySignature<Types...>();
    assert(!m_rejectSignature.hasAnyFrom(sign) && "None of the supplied types should be in query's 'reject' signature");

    return Iterator<Types...>(this, m_vecEntityIDs.size());
}



template<typename ...Types>
void CEntityQuery::forEach(const std::function<void(Types&...)>& handler )
{
    for(auto it = this->begin<Types...>(); it != this->end<Types...>(); it++)
    {
        std::apply(handler, *it);
    }
}



template<typename ...Types>
void CEntityQuery::sort(std::function<bool(CEntityQuery::Iterator<Types...>, CEntityQuery::Iterator<Types...>)> comparator) noexcept
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

    std::vector<entityid_t> sortedEnts(m_vecEntityIDs.size());

    for (unsigned int i = 0; i < m_vecEntityIDs.size(); i++)
    {
        sortedEnts[i] = this->m_vecEntityIDs[indices[i]];
    }

    this->m_vecEntityIDs = std::move(sortedEnts);
}

} // namespace chestnut::ecs
