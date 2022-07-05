#pragma once

#include <type_traits>
#include <vector>

namespace chestnut::ecs::internal
{
    template<typename T>
    class CSparseSet
    {
    private:
        mutable std::vector<int> m_sparse;
        std::vector<T> m_dense;

    public:
        static const int NIL_INDEX = -1;


    public:
        CSparseSet() noexcept = default;
        CSparseSet(unsigned int initSparseSize) noexcept;

        CSparseSet(const CSparseSet& other) noexcept;
        CSparseSet& operator=(const CSparseSet& other) noexcept;

        CSparseSet(CSparseSet&& other) noexcept;
        CSparseSet& operator=(CSparseSet&& other) noexcept;


        const std::vector<int>& sparse() const noexcept;
        const std::vector<T>& dense() const noexcept;


        T& at(unsigned int idx);
        const T& at(unsigned int idx) const;

        bool empty() const noexcept;
        unsigned int size() const noexcept;
        bool contains(unsigned int idx) const noexcept;

        void clear() noexcept;
        void insert(unsigned int idx, T&& arg) noexcept;
        void erase(unsigned int idx) noexcept;
    };
    
} // namespace chestnut::ecs::internal


#include "sparse_set.inl"