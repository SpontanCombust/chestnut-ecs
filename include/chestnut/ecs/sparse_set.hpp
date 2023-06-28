#pragma once

#include <string>
#include <type_traits>
#include <vector>


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
    public:
        struct SDenseElement 
        {
            T e;
            size_t i;
        };

    private:
        std::vector<SDenseElement> m_dense;


    public:
        CSparseSet() noexcept = default;
        CSparseSet(size_t initSparseSize) noexcept;

        const std::vector<SDenseElement>& dense() const noexcept;
        size_t denseSize() const noexcept;

        T& at(size_t idx);
        const T& at(size_t idx) const;

        bool empty() const noexcept;

        void clear() noexcept;
        void insert(size_t idx, T&& arg) noexcept;
        void erase(size_t idx) noexcept override;
    };
    
} // namespace chestnut::ecs::internal


#include "sparse_set.inl"