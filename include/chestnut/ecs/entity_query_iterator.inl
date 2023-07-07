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
            const auto& identComp = this->fetchComponent<CIdentityComponent>();
            CUniqueIdentifier uuid = identComp.uuid;
            return CEntity(uuid, this->slot());
        }

        std::tuple<Types&...> operator*()
        {
            return std::tie(this->fetchComponent<Types>()...);
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

        // can throw exception
        template<typename T>
        T& fetchComponent()
        {
            auto compExp = m_query->m_storagePtr->at<T>(this->slot());
            if (!compExp.has_value())
            {
                throw std::runtime_error("Iterator invalidated: " + compExp.error());
            }

            return *compExp.value();
        }

        // can throw exception
        template<typename T>
        const T& fetchComponent() const
        {
            const auto compExp = m_query->m_storagePtr->at<T>(this->slot());
            if (!compExp.has_value())
            {
                throw std::runtime_error("Iterator invalidated: " + compExp.error());
            }

            return *compExp.value();
        }
    };

} // namespace chestnut::ecs
