#pragma once


#include "component_storage.hpp"
#include "native_components.hpp"

#include <exception>

namespace chestnut::ecs
{
    /**
     * @brief A public iterator that gives a view into all registered entities. 
     * Allows only for iterating forward.
     */
    class CEntityIterator
    {
    public:
        CEntityIterator(internal::CComponentStorage *storagePtr, entityslot_t slot) noexcept
        : m_storagePtr(storagePtr), m_currentSlot(slot)
        {
            
        }

        inline bool isSlotTaken() const noexcept
        {
            return m_storagePtr->contains<CIdentityComponent>(m_currentSlot);
        }

        inline bool isInBounds() const noexcept
        {
            entityslot_t maxSlot = (entityslot_t)m_storagePtr->maxSlot();
            return m_currentSlot >= ENTITY_SLOT_MINIMAL
                && m_currentSlot <= maxSlot;
        }

        CEntity entity() const
        {
            if (!isSlotTaken())
            {
                throw std::runtime_error("Iterator invalidated");
            }

            auto uuid = m_storagePtr->at<CIdentityComponent>(m_currentSlot).uuid;
            return CEntity(uuid, m_currentSlot);
        }

        template<typename T>
        T& get()
        {
            return m_storagePtr->at<T>(m_currentSlot);
        }

        template<typename T>
        bool contains() const noexcept
        {
            return m_storagePtr->contains<T>(m_currentSlot);
        }



        CEntitySignature signature() const noexcept
        {
            return m_storagePtr->signature(m_currentSlot);
        }


        /**
         * @brief Moves the iterator forward and if it isn't valid and hasn't gone past the end yet
         * moves it even further until is valid or goes past the end. Forcing it to move past end
         * is undefined behaviour.
         * 
         * @return advanced iterator
         */
        CEntityIterator& operator++() noexcept
        {
            do
            {
                ++m_currentSlot;
            }
            while(!this->isSlotTaken() && this->isInBounds());

            return *this;
        }

        /**
         * @brief Moves the iterator forward and if it isn't valid and hasn't gone past the end yet
         * moves it even further until is valid or goes past the end. Forcing it to move past 
         * the end is undefined behaviour.
         * 
         * @return original iterator
         */
        CEntityIterator operator++(int) noexcept
        {
            CEntityIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        /**
         * @brief Moves the iterator backward and if it isn't valid and hasn't gone past the beginning yet
         * moves it even further back until is valid or stops at ENTITY_ID_MINIMAL. Forcing it to move past
         * the beginning is undefined behaviour.
         * 
         * @return advanced iterator
         */
        CEntityIterator& operator--() noexcept
        {
            do
            {
                --m_currentSlot;
            }
            while(!this->isSlotTaken() && this->isInBounds());

            return *this;
        }

        /**
         * @brief Moves the iterator backward and if it isn't valid and hasn't gone past the beginning yet
         * moves it even further back until is valid or stops at ENTITY_ID_MINIMAL. Forcing it to move past
         * the beginning is undefined behaviour.
         * 
         * @return original iterator
         */
        CEntityIterator operator--(int) noexcept
        {
            CEntityIterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const CEntityIterator& other) const noexcept
        {
            return m_storagePtr == other.m_storagePtr
                && m_currentSlot == other.m_currentSlot;
        }

        bool operator!=(const CEntityIterator& other) const noexcept
        {
            return !(*this == other);
        }

    private:
        internal::CComponentStorage *m_storagePtr;
        entityslot_t m_currentSlot;
    };




    /**
     * @brief A public iterator that gives a non-mutable view into all registered entities
     * 
     */
    class CEntityConstIterator
    {
    public:
        CEntityConstIterator(const internal::CComponentStorage *storagePtr, entityslot_t slot) noexcept
        : m_storagePtr(storagePtr), m_currentSlot(slot)
        {
            
        }

        inline bool isSlotTaken() const noexcept
        {
            return m_storagePtr->contains<CIdentityComponent>(m_currentSlot);
        }

        inline bool isInBounds() const noexcept
        {
            entityslot_t maxSlot = (entityslot_t)m_storagePtr->maxSlot();
            return m_currentSlot >= ENTITY_SLOT_MINIMAL
                && m_currentSlot <= maxSlot;
        }

        CEntity entity() const
        {
            if (!isSlotTaken())
            {
                throw std::runtime_error("Iterator out of bounds");
            }

            auto uuid = m_storagePtr->at<CIdentityComponent>(m_currentSlot).uuid;
            return CEntity(uuid, m_currentSlot);
        }

        template<typename T>
        const T& get() const
        {
            return m_storagePtr->at<T>(m_currentSlot);
        }

        template<typename T>
        bool contains() const noexcept
        {
            return m_storagePtr->contains<T>(m_currentSlot);
        }



        CEntitySignature signature() const noexcept
        {
            return m_storagePtr->signature(m_currentSlot);
        }


        /**
         * @brief Moves the iterator forward and if it isn't valid and hasn't gone past the end yet
         * moves it even further until is valid or goes past the end. Forcing it to move past end
         * is undefined behaviour.
         * 
         * @return advanced iterator
         */
        CEntityConstIterator& operator++() noexcept
        {
            do
            {
                ++m_currentSlot;
            }
            while(!this->isSlotTaken() && this->isInBounds());

            return *this;
        }

        /**
         * @brief Moves the iterator forward and if it isn't valid and hasn't gone past the end yet
         * moves it even further until is valid or goes past the end. Forcing it to move past 
         * the end is undefined behaviour.
         * 
         * @return original iterator
         */
        CEntityConstIterator operator++(int) noexcept
        {
            CEntityConstIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        /**
         * @brief Moves the iterator backward and if it isn't valid and hasn't gone past the beginning yet
         * moves it even further back until is valid or stops at ENTITY_ID_MINIMAL. Forcing it to move past
         * the beginning is undefined behaviour.
         * 
         * @return advanced iterator
         */
        CEntityConstIterator& operator--() noexcept
        {
            do
            {
                --m_currentSlot;
            }
            while(!this->isSlotTaken() && this->isInBounds());

            return *this;
        }

        /**
         * @brief Moves the iterator backward and if it isn't valid and hasn't gone past the beginning yet
         * moves it even further back until is valid or stops at ENTITY_ID_MINIMAL. Forcing it to move past
         * the beginning is undefined behaviour.
         * 
         * @return original iterator
         */
        CEntityConstIterator operator--(int) noexcept
        {
            CEntityConstIterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const CEntityConstIterator& other) const noexcept
        {
            return m_storagePtr == other.m_storagePtr
                && m_currentSlot == other.m_currentSlot;
        }

        bool operator!=(const CEntityConstIterator& other) const noexcept
        {
            return !(*this == other);
        }


    private:
        const internal::CComponentStorage *m_storagePtr;
        entityslot_t m_currentSlot;
    };

} // namespace chestnut::ecs
