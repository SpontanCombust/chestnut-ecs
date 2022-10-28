#pragma once

#include <type_traits>
#include <vector>

namespace chestnut::ecs::internal
{
    class CSparseSetBase
    {
    protected:
        mutable std::vector<int> m_sparse;

    public:
        using index_type = unsigned int;

        inline static const int NIL_INDEX = -1;



    public:
        CSparseSetBase() noexcept = default;
        CSparseSetBase(index_type initSparseSize) noexcept;

        CSparseSetBase(const CSparseSetBase& other) noexcept;
        CSparseSetBase& operator=(const CSparseSetBase& other) noexcept;

        CSparseSetBase(CSparseSetBase&& other) noexcept;
        CSparseSetBase& operator=(CSparseSetBase&& other) noexcept; 

        virtual ~CSparseSetBase() = default;


        const std::vector<int>& sparse() const noexcept;   

        bool contains(index_type idx) const noexcept;    

        virtual void erase(index_type idx) noexcept;
    };



    template<typename T>
    class CSparseSet : public CSparseSetBase
    {
    public:
        struct SDenseElement 
        {
            T e;
            index_type i;
        };

    private:
        std::vector<SDenseElement> m_dense;


    public:
        CSparseSet() noexcept = default;
        CSparseSet(index_type initSparseSize) noexcept;

        CSparseSet(const CSparseSet& other) noexcept;
        CSparseSet& operator=(const CSparseSet& other) noexcept;

        CSparseSet(CSparseSet&& other) noexcept;
        CSparseSet& operator=(CSparseSet&& other) noexcept;


        const std::vector<SDenseElement>& dense() const noexcept;


        T& at(index_type idx);
        const T& at(index_type idx) const;

        bool empty() const noexcept;
        index_type size() const noexcept;

        void clear() noexcept;
        void insert(index_type idx, T&& arg) noexcept;
        void erase(index_type idx) noexcept override;
    };
    
} // namespace chestnut::ecs::internal


#include "sparse_set.inl"