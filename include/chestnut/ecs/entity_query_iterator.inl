#include "native_components.hpp"

#include <exception>
#include <tuple>

namespace chestnut::ecs
{
    class CEntityQuery::IteratorBase
    {
    public:
        IteratorBase(CEntityQuery *query, size_t queryIdx) noexcept
        : m_query(query), m_currentQueryIdx(queryIdx)
        {

        }

        CEntity entity() const
        {
            const auto& identComp = this->fetchComponent<CIdentityComponent>();
            CUniqueIdentifier uuid = identComp.uuid;
            return CEntity(uuid, this->slot());
        }

        bool operator==(const IteratorBase& other) const noexcept
        {
            return m_query == other.m_query && m_currentQueryIdx == other.m_currentQueryIdx;
        }

        bool operator!=(const IteratorBase& other) const noexcept
        {
            return !(*this == other);
        }

    protected:
        CEntityQuery *m_query;
        size_t m_currentQueryIdx;

        virtual entityslot_t slot() const = 0;


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



    template<typename ...Types>
    class CEntityQuery::CurrentEntitiesIterator : public CEntityQuery::IteratorBase
    {
    public:
        CurrentEntitiesIterator(CEntityQuery *query, size_t queryIdx) noexcept
        : IteratorBase(query, queryIdx)
        {
            
        }

        CurrentEntitiesIterator(const IteratorBase& base) noexcept
        : IteratorBase(base)
        {

        }

        CurrentEntitiesIterator& operator++() noexcept
        {
            m_currentQueryIdx++;
            return *this;
        }

        CurrentEntitiesIterator operator++(int) noexcept
        {
            CurrentEntitiesIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        CurrentEntitiesIterator operator+(unsigned int n) noexcept
        {
            CurrentEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx += n;
            return tmp;
        }

        CurrentEntitiesIterator& operator--() noexcept
        {
            m_currentQueryIdx--;
            return *this;
        }

        CurrentEntitiesIterator operator--(int) noexcept
        {
            CurrentEntitiesIterator tmp(*this);
            --(*this);
            return tmp;
        }

        CurrentEntitiesIterator operator-(unsigned int n) noexcept
        {
            CurrentEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx -= n;
            return tmp;
        }

        std::tuple<Types&...> operator*()
        {
            return std::tie(this->fetchComponent<Types>()...);
        }

    protected:
        entityslot_t slot() const override
        {
            return m_query->m_vecCurrentEntitySlots[m_currentQueryIdx];
        }
    };


    template<typename ...Types>
    class CEntityQuery::AcquiredEntitiesIterator : public CEntityQuery::IteratorBase
    {
    public:
        AcquiredEntitiesIterator(CEntityQuery *query, size_t queryIdx) noexcept
        : IteratorBase(query, queryIdx)
        {
            
        }

        AcquiredEntitiesIterator(const IteratorBase& base) noexcept
        : IteratorBase(base)
        {

        }

        AcquiredEntitiesIterator& operator++() noexcept
        {
            m_currentQueryIdx++;
            return *this;
        }

        AcquiredEntitiesIterator operator++(int) noexcept
        {
            AcquiredEntitiesIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        AcquiredEntitiesIterator operator+(unsigned int n) noexcept
        {
            AcquiredEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx += n;
            return tmp;
        }

        AcquiredEntitiesIterator& operator--() noexcept
        {
            m_currentQueryIdx--;
            return *this;
        }

        AcquiredEntitiesIterator operator--(int) noexcept
        {
            AcquiredEntitiesIterator tmp(*this);
            --(*this);
            return tmp;
        }

        AcquiredEntitiesIterator operator-(unsigned int n) noexcept
        {
            AcquiredEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx -= n;
            return tmp;
        }

        std::tuple<Types&...> operator*()
        {
            return std::tie(this->fetchComponent<Types>()...);
        }

    protected:
        entityslot_t slot() const override
        {
            return m_query->m_vecAcquiredEntitySlots[m_currentQueryIdx];
        }
    };


    class CEntityQuery::DiscardedEntitiesIterator : public CEntityQuery::IteratorBase
    {
    public:
        DiscardedEntitiesIterator(CEntityQuery *query, size_t queryIdx) noexcept
        : IteratorBase(query, queryIdx)
        {
            
        }

        DiscardedEntitiesIterator(const IteratorBase& base) noexcept
        : IteratorBase(base)
        {

        }

        DiscardedEntitiesIterator& operator++() noexcept
        {
            m_currentQueryIdx++;
            return *this;
        }

        DiscardedEntitiesIterator operator++(int) noexcept
        {
            DiscardedEntitiesIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        DiscardedEntitiesIterator operator+(unsigned int n) noexcept
        {
            DiscardedEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx += n;
            return tmp;
        }

        DiscardedEntitiesIterator& operator--() noexcept
        {
            m_currentQueryIdx--;
            return *this;
        }

        DiscardedEntitiesIterator operator--(int) noexcept
        {
            DiscardedEntitiesIterator tmp(*this);
            --(*this);
            return tmp;
        }

        DiscardedEntitiesIterator operator-(unsigned int n) noexcept
        {
            DiscardedEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx -= n;
            return tmp;
        }

    protected:
        entityslot_t slot() const override
        {
            return m_query->m_vecDiscardedEntitySlots[m_currentQueryIdx];
        }
    };

} // namespace chestnut::ecs
