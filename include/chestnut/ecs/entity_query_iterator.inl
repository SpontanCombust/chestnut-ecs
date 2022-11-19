#include <typelist.hpp>

#include <tuple>

namespace chestnut::ecs
{
    template<typename ...Types>
    struct CEntityQuery::Iterator
    {
        CEntityQuery *m_query;
        unsigned int m_currentQueryIdx;


        Iterator(CEntityQuery *query, unsigned int queryIdx) noexcept
        : m_query(query), m_currentQueryIdx(queryIdx)
        {
            
        }

        entityid_t entityId() const noexcept
        {
            return m_query->m_vecEntityIDs[m_currentQueryIdx];
        }



        std::tuple<Types&...> operator*()
        {
            using TL = tl::type_list<Types...>;

            return TL::template for_each_and_collect<std::tuple>([&](auto t) -> typename decltype(t)::type& {
                using T = typename decltype(t)::type;
                return m_query->m_storagePtr->at<T>(m_query->m_vecEntityIDs[m_currentQueryIdx]);
            });
        }

        Iterator& operator++() noexcept
        {
            m_currentQueryIdx++;
            return *this;
        }

        Iterator operator++(int) noexcept
        {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        Iterator operator+(unsigned int n) noexcept
        {
            Iterator tmp(*this);
            tmp.m_currentQueryIdx += n;
            return tmp;
        }

        Iterator& operator--() noexcept
        {
            m_currentQueryIdx--;
            return *this;
        }

        Iterator operator--(int) noexcept
        {
            Iterator tmp(*this);
            --(*this);
            return tmp;
        }

        Iterator operator-(unsigned int n) noexcept
        {
            Iterator tmp(*this);
            tmp.m_currentQueryIdx -= n;
            return tmp;
        }

        bool operator==(const Iterator& other) const noexcept
        {
            return m_query == other.m_query && m_currentQueryIdx == other.m_currentQueryIdx;
        }

        bool operator!=(const Iterator& other) const noexcept
        {
            return !(*this == other);
        }
    };

} // namespace chestnut::ecs
