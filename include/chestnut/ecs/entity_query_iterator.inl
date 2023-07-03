#include <typelist.hpp>

#include <exception>
#include <tuple>

namespace chestnut::ecs
{
    template<typename ...Types>
    class CEntityQuery::Iterator
    {
    public:
        Iterator(CEntityQuery *query, unsigned int queryIdx) noexcept
        : m_query(query), m_currentQueryIdx(queryIdx)
        {
            
        }

        CEntity entity() const
        {
            entityslot_t slot = this->slot();

            auto identExp = m_query->m_storagePtr->at<CIdentityComponent>(slot);
            if (!identExp.has_value())
            {
                throw std::runtime_error("Iterator invalidated: " + identExp.error());
            }

            CUniqueIdentifier uuid = identExp.value()->uuid;
            return CEntity(uuid, slot);
        }

        std::tuple<Types&...> operator*()
        {
            using TL = tl::type_list<Types...>;

            entityslot_t slot = this->slot();
            return TL::template for_each_and_collect<std::tuple>([&](auto t) -> typename decltype(t)::type& {
                using T = typename decltype(t)::type;

                auto compExp = m_query->m_storagePtr->at<T>(slot);
                if (!compExp.has_value())
                {
                    throw std::runtime_error("Iterator invalidated: " + compExp.error());
                }

                return *compExp.value();
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


    private:
        CEntityQuery *m_query;
        unsigned int m_currentQueryIdx;

        inline entityslot_t slot() const
        {
            return m_query->m_vecEntitySlots[m_currentQueryIdx];
        }
    };

} // namespace chestnut::ecs
