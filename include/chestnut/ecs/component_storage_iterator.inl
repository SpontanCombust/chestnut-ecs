namespace chestnut::ecs::internal
{
    struct CComponentStorage::Iterator
    {
        CComponentStorage *m_storage;
        entityid_t m_currentId;


        Iterator(CComponentStorage *storage, entityid_t id) noexcept
        : m_storage(storage), m_currentId(id)
        {
            
        }

        entityid_t id() const noexcept
        {
            return m_currentId;
        }

        template<typename T>
        T& get()
        {
            return m_storage->at<T>(m_currentId);
        }

        template<typename T>
        void set(T&& arg) noexcept
        {
            m_storage->insert<T>(m_currentId, std::forward(arg));
        }

        template<typename T>
        bool contains() const noexcept
        {
            return m_storage->contains<T>(m_currentId);
        }

        template<typename T>
        void erase() noexcept
        {
            m_storage->erase<T>(m_currentId);
        }



        Iterator& operator++() noexcept
        {
            m_currentId++;
            return *this;
        }

        Iterator operator++(int) noexcept
        {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        Iterator& operator--() noexcept
        {
            m_currentId--;
            return *this;
        }

        Iterator operator--(int) noexcept
        {
            Iterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const noexcept
        {
            return m_storage == other.m_storage && m_currentId == other.m_currentId;
        }

        bool operator!=(const Iterator& other) const noexcept
        {
            return !(*this == other);
        }
    };




    struct CComponentStorage::ConstIterator
    {
        const CComponentStorage *m_storage;
        entityid_t m_currentId;


        ConstIterator(const CComponentStorage *storage, entityid_t id) noexcept
        : m_storage(storage), m_currentId(id)
        {
            
        }

        entityid_t id() const noexcept
        {
            return m_currentId;
        }

        template<typename T>
        const T& get() const
        {
            return m_storage->at<T>(m_currentId);
        }

        template<typename T>
        bool contains() const noexcept
        {
            return m_storage->contains<T>(m_currentId);
        }



        ConstIterator& operator++() noexcept
        {
            m_currentId++;
            return *this;
        }

        ConstIterator operator++(int) noexcept
        {
            ConstIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        ConstIterator& operator--() noexcept
        {
            m_currentId--;
            return *this;
        }

        ConstIterator operator--(int) noexcept
        {
            ConstIterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const noexcept
        {
            return m_storage == other.m_storage && m_currentId == other.m_currentId;
        }

        bool operator!=(const ConstIterator& other) const noexcept
        {
            return !(*this == other);
        }
    };

} // namespace chestnut::ecs::internal
