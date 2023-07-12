#pragma once

#include <string>
#include <vector>

#include <tl/expected.hpp>


namespace chestnut::ecs::internal
{
    class CSparseSetBase
    {
    protected:
        mutable std::vector<int> m_sparse;

    public:
        inline static const int NIL_INDEX = -1;

    public:
        CSparseSetBase() noexcept = default;
        CSparseSetBase(size_t initSparseSize) noexcept;
        virtual ~CSparseSetBase() = default;

        const std::vector<int>& sparse() const noexcept;   
        size_t sparseSize() const noexcept;
        bool contains(size_t idx) const noexcept;    

        virtual void erase(size_t idx) noexcept;
    };



    template<typename T>
    class CSparseSet : public CSparseSetBase
    {
    private:
        std::vector<T> m_dense;
        std::vector<size_t> m_densePointers;

    public:
        CSparseSet() noexcept = default;
        CSparseSet(size_t initSparseSize) noexcept;

        const std::vector<T>& dense() const noexcept;
        size_t denseSize() const noexcept;

        tl::expected<T*, std::string> at(size_t idx);
        // const T* instead of reference_wrapper<const T>, because tl::expected refused to cooperate using the latter
        tl::expected<const T*, std::string> at(size_t idx) const;

        bool empty() const noexcept;

        void clear() noexcept;
        void insert(size_t idx, T&& arg) noexcept;
        void erase(size_t idx) noexcept override;
    };
    
} // namespace chestnut::ecs::internal


#include "sparse_set.inl"