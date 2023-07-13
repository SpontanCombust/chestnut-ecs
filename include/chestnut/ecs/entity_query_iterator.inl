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
            return m_query->m_vecEntitySlots[m_currentQueryIdx];
        }
    };


    template<typename ...Types>
    class CEntityQuery::IncomingEntitiesIterator : public CEntityQuery::IteratorBase
    {
    public:
        IncomingEntitiesIterator(CEntityQuery *query, size_t queryIdx) noexcept
        : IteratorBase(query, queryIdx)
        {
            
        }

        IncomingEntitiesIterator(const IteratorBase& base) noexcept
        : IteratorBase(base)
        {

        }

        IncomingEntitiesIterator& operator++() noexcept
        {
            m_currentQueryIdx++;
            return *this;
        }

        IncomingEntitiesIterator operator++(int) noexcept
        {
            IncomingEntitiesIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        IncomingEntitiesIterator operator+(unsigned int n) noexcept
        {
            IncomingEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx += n;
            return tmp;
        }

        IncomingEntitiesIterator& operator--() noexcept
        {
            m_currentQueryIdx--;
            return *this;
        }

        IncomingEntitiesIterator operator--(int) noexcept
        {
            IncomingEntitiesIterator tmp(*this);
            --(*this);
            return tmp;
        }

        IncomingEntitiesIterator operator-(unsigned int n) noexcept
        {
            IncomingEntitiesIterator tmp(*this);
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
            return m_query->m_vecIncomingEntitySlots[m_currentQueryIdx];
        }
    };


    class CEntityQuery::OutgoingEntitiesIterator : public CEntityQuery::IteratorBase
    {
    public:
        OutgoingEntitiesIterator(CEntityQuery *query, size_t queryIdx) noexcept
        : IteratorBase(query, queryIdx)
        {
            
        }

        OutgoingEntitiesIterator(const IteratorBase& base) noexcept
        : IteratorBase(base)
        {

        }

        OutgoingEntitiesIterator& operator++() noexcept
        {
            m_currentQueryIdx++;
            return *this;
        }

        OutgoingEntitiesIterator operator++(int) noexcept
        {
            OutgoingEntitiesIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        OutgoingEntitiesIterator operator+(unsigned int n) noexcept
        {
            OutgoingEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx += n;
            return tmp;
        }

        OutgoingEntitiesIterator& operator--() noexcept
        {
            m_currentQueryIdx--;
            return *this;
        }

        OutgoingEntitiesIterator operator--(int) noexcept
        {
            OutgoingEntitiesIterator tmp(*this);
            --(*this);
            return tmp;
        }

        OutgoingEntitiesIterator operator-(unsigned int n) noexcept
        {
            OutgoingEntitiesIterator tmp(*this);
            tmp.m_currentQueryIdx -= n;
            return tmp;
        }

    protected:
        entityslot_t slot() const override
        {
            return m_query->m_vecOutgoingEntitySlots[m_currentQueryIdx];
        }
    };

} // namespace chestnut::ecs
