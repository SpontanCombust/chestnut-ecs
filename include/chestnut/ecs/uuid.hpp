#pragma once

#include <cstdint>
#include <random>
#include <sstream>

namespace chestnut::ecs
{
    class CUniqueIdentifier
    {
    public:
        using Type = uint64_t;
        Type val;

        CUniqueIdentifier()
        {
            static std::random_device rd;
            static std::mt19937_64 engine(rd());
            static std::uniform_int_distribution<Type> distrib;

            this->val = distrib(engine);
        }

        inline bool operator==(const CUniqueIdentifier& other) const
        {
            return this->val == other.val;
        }

        inline bool operator!=(const CUniqueIdentifier& other) const
        {
            return this->val != other.val;
        }

        inline bool operator>(const CUniqueIdentifier& other) const
        {
            return this->val > other.val;
        }

        inline bool operator<(const CUniqueIdentifier& other) const
        {
            return this->val < other.val;
        }
    };

} // namespace chestnut::ecs


namespace std
{
    template<>
    struct hash<chestnut::ecs::CUniqueIdentifier>
    {
        size_t operator()(const chestnut::ecs::CUniqueIdentifier& uuid) const
        {
            return uuid.val;
        }
    };

    inline std::string to_string(const chestnut::ecs::CUniqueIdentifier& uuid)
    {
        std::stringstream ss;
        ss << std::hex << uuid.val;
        return ss.str();
    }

} // namespace std

