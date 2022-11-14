#pragma once


#include "component_storage.hpp"
#include "entity_registry.hpp"

namespace chestnut::ecs
{
    /**
     * @brief A public iterator that gives a view into all registered entities. 
     * Allows only for iterating forward.
     */
    class CEntityIterator
    {
    private:
        const internal::CEntityRegistry *m_registryPtr;
        internal::CComponentStorage *m_storagePtr;
        entityid_t m_currentId;

    public:
        CEntityIterator(const internal::CEntityRegistry *registryPtr, internal::CComponentStorage *storagePtr, entityid_t id) noexcept
        : m_registryPtr(registryPtr), m_storagePtr(storagePtr), m_currentId(id)
        {
            
        }

        bool isValid() const noexcept
        {
            return m_registryPtr->isEntityRegistered(m_currentId);
        }

        bool canGoForward() const noexcept
        {
            entityid_t highestId = m_registryPtr->getHighestIdRegistered();
            return m_currentId != ENTITY_ID_INVALID
                && highestId != ENTITY_ID_INVALID 
                && m_currentId <= highestId;
        }

        bool canGoBackward() const noexcept
        {
            return m_currentId != ENTITY_ID_INVALID
                && m_currentId > ENTITY_ID_MINIMAL;
        }

        entityid_t id() const noexcept
        {
            return m_currentId;
        }

        template<typename T>
        T& get()
        {
            return m_storagePtr->at<T>(m_currentId);
        }

        template<typename T>
        bool contains() const noexcept
        {
            return m_storagePtr->contains<T>(m_currentId);
        }



        CEntitySignature signature() const noexcept
        {
            return m_storagePtr->signature(m_currentId);
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
                ++m_currentId;
            }
            while(!this->isValid() && this->canGoForward());

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
                --m_currentId;
            }
            while(!this->isValid() && this->canGoBackward());

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
            return m_registryPtr == other.m_registryPtr 
                && m_storagePtr == other.m_storagePtr
                && m_currentId == other.m_currentId;
        }

        bool operator!=(const CEntityIterator& other) const noexcept
        {
            return !(*this == other);
        }
    };




    /**
     * @brief A public iterator that gives a non-mutable view into all registered entities
     * 
     */
    class CEntityConstIterator
    {
    private:
        const internal::CEntityRegistry *m_registryPtr;
        const internal::CComponentStorage *m_storagePtr;
        entityid_t m_currentId;

    public:
        CEntityConstIterator(const internal::CEntityRegistry *registryPtr, const internal::CComponentStorage *storagePtr, entityid_t id) noexcept
        : m_registryPtr(registryPtr), m_storagePtr(storagePtr), m_currentId(id)
        {
            
        }

        bool isValid() const noexcept
        {
            return m_registryPtr->isEntityRegistered(m_currentId);
        }

        bool canGoForward() const noexcept
        {
            entityid_t highestId = m_registryPtr->getHighestIdRegistered();
            return m_currentId != ENTITY_ID_INVALID
                && highestId != ENTITY_ID_INVALID 
                && m_currentId <= highestId;
        }

        bool canGoBackward() const noexcept
        {
            return m_currentId != ENTITY_ID_INVALID
                && m_currentId > ENTITY_ID_MINIMAL;
        }

        entityid_t id() const noexcept
        {
            return m_currentId;
        }

        template<typename T>
        const T& get() const
        {
            return m_storagePtr->at<T>(m_currentId);
        }

        template<typename T>
        bool contains() const noexcept
        {
            return m_storagePtr->contains<T>(m_currentId);
        }



        CEntitySignature signature() const noexcept
        {
            return m_storagePtr->signature(m_currentId);
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
                ++m_currentId;
            }
            while(!this->isValid() && this->canGoForward());

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
                --m_currentId;
            }
            while(!this->isValid() && this->canGoBackward());

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
            return m_registryPtr == other.m_registryPtr 
                && m_storagePtr == other.m_storagePtr
                && m_currentId == other.m_currentId;
        }

        bool operator!=(const CEntityConstIterator& other) const noexcept
        {
            return !(*this == other);
        }
    };

} // namespace chestnut::ecs
