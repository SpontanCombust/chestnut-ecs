namespace chestnut::ecs::internal
{
    class CComponentStorage::CIterator
    {
    private:
        CComponentStorage *m_storage;
        entityid_t m_currentId;


    public:
        CIterator(CComponentStorage *storage, entityid_t id) noexcept
        : m_storage(storage), m_currentId(id)
        {
            
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



        CIterator& operator++() noexcept
        {
            m_currentId++;
            return *this;
        }

        CIterator operator++(int) noexcept
        {
            CIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        CIterator& operator--() noexcept
        {
            m_currentId--;
            return *this;
        }

        CIterator operator--(int) noexcept
        {
            CIterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const CIterator& other) const noexcept
        {
            return m_storage == other.m_storage && m_currentId == other.m_currentId;
        }

        bool operator!=(const CIterator& other) const noexcept
        {
            return !(*this == other);
        }
    };




    class CComponentStorage::CConstIterator
    {
    private:
        const CComponentStorage *m_storage;
        entityid_t m_currentId;


    public:
        CConstIterator(const CComponentStorage *storage, entityid_t id) noexcept
        : m_storage(storage), m_currentId(id)
        {
            
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



        CConstIterator& operator++() noexcept
        {
            m_currentId++;
            return *this;
        }

        CConstIterator operator++(int) noexcept
        {
            CConstIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        CConstIterator& operator--() noexcept
        {
            m_currentId--;
            return *this;
        }

        CConstIterator operator--(int) noexcept
        {
            CConstIterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const CConstIterator& other) const noexcept
        {
            return m_storage == other.m_storage && m_currentId == other.m_currentId;
        }

        bool operator!=(const CConstIterator& other) const noexcept
        {
            return !(*this == other);
        }
    };

} // namespace chestnut::ecs::internal
