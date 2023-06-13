#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage.hpp"
#include "../include/chestnut/ecs/entity_registry.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

struct SmallData {
    int a, b;
};

struct BigData {
    char a[1028];
};

TEST_CASE("Effieciency benchmarks - adding components", "[benchmark]")
{
    SECTION("Small data")
    {
        CSparseSet<SmallData> setSmall;

        auto benchmarker = [&setSmall](const int n) {
            for (int i = 0; i < n; i++)
            {
                setSmall.insert(i, SmallData{});
            }

            return setSmall.size();
        };

        BENCHMARK("1000")
        {
            return benchmarker(1000);
        };
        BENCHMARK("2000")
        {
            return benchmarker(2000);
        };
        BENCHMARK("3000")
        {
            return benchmarker(3000);
        };
        BENCHMARK("4000")
        {
            return benchmarker(4000);
        };
        BENCHMARK("5000")
        {
            return benchmarker(5000);
        };
        BENCHMARK("6000")
        {
            return benchmarker(6000);
        };
        BENCHMARK("7000")
        {
            return benchmarker(7000);
        };
        BENCHMARK("8000")
        {
            return benchmarker(8000);
        };
        BENCHMARK("9000")
        {
            return benchmarker(9000);
        };
        BENCHMARK("10000")
        {
            return benchmarker(10000);
        };
    }

    SECTION("Big data")
    {
        CSparseSet<BigData> setBig;

        auto benchmarker = [&setBig](const int n) {
            for (int i = 0; i < n; i++)
            {
                setBig.insert(i, BigData{});
            }

            return setBig.size();
        };

        BENCHMARK("1000")
        {
            return benchmarker(1000);
        };
        BENCHMARK("2000")
        {
            return benchmarker(2000);
        };
        BENCHMARK("3000")
        {
            return benchmarker(3000);
        };
        BENCHMARK("4000")
        {
            return benchmarker(4000);
        };
        BENCHMARK("5000")
        {
            return benchmarker(5000);
        };
        BENCHMARK("6000")
        {
            return benchmarker(6000);
        };
        BENCHMARK("7000")
        {
            return benchmarker(7000);
        };
        BENCHMARK("8000")
        {
            return benchmarker(8000);
        };
        BENCHMARK("9000")
        {
            return benchmarker(9000);
        };
        BENCHMARK("10000")
        {
            return benchmarker(10000);
        };
    }

}


TEST_CASE("Effieciency benchmarks - finding components", "[benchmark]")
{
    SECTION("Small data")
    {
        CSparseSet<SmallData> setSmall;

        for (int i = 0; i < 50000; i++)
        {
            setSmall.insert(i, SmallData{});
        }

        auto benchmarker = [&setSmall](const int n) {
            bool contains;

            for (int i = 0; i < n; i++)
            {
                contains = setSmall.contains(i);
            }

            return contains;
        };
        
        BENCHMARK("1000")
        {
            return benchmarker(1000);
        };
        BENCHMARK("2000")
        {
            return benchmarker(2000);
        };
        BENCHMARK("3000")
        {
            return benchmarker(3000);
        };
        BENCHMARK("4000")
        {
            return benchmarker(4000);
        };
        BENCHMARK("5000")
        {
            return benchmarker(5000);
        };
        BENCHMARK("6000")
        {
            return benchmarker(6000);
        };
        BENCHMARK("7000")
        {
            return benchmarker(7000);
        };
        BENCHMARK("8000")
        {
            return benchmarker(8000);
        };
        BENCHMARK("9000")
        {
            return benchmarker(9000);
        };
        BENCHMARK("10000")
        {
            return benchmarker(10000);
        };
    }

    SECTION("Big data")
    {
        CSparseSet<BigData> setBig;

        for (int i = 0; i < 50000; i++)
        {
            setBig.insert(i, BigData{});
        }

        auto benchmarker = [&setBig](const int n) {
            bool contains;

            for (int i = 0; i < n; i++)
            {
                contains = setBig.contains(i);
            }

            return contains;
        };
        
        BENCHMARK("1000")
        {
            return benchmarker(1000);
        };
        BENCHMARK("2000")
        {
            return benchmarker(2000);
        };
        BENCHMARK("3000")
        {
            return benchmarker(3000);
        };
        BENCHMARK("4000")
        {
            return benchmarker(4000);
        };
        BENCHMARK("5000")
        {
            return benchmarker(5000);
        };
        BENCHMARK("6000")
        {
            return benchmarker(6000);
        };
        BENCHMARK("7000")
        {
            return benchmarker(7000);
        };
        BENCHMARK("8000")
        {
            return benchmarker(8000);
        };
        BENCHMARK("9000")
        {
            return benchmarker(9000);
        };
        BENCHMARK("10000")
        {
            return benchmarker(10000);
        };
    }
}

TEST_CASE("Effieciency benchmarks - removing components", "[benchmark]")
{
    SECTION("Small data")
    {
        CSparseSet<SmallData> setSmall;

        for (int i = 0; i < 50000; i++)
        {
            setSmall.insert(i, SmallData{});
        }

        auto benchmarker = [&setSmall](const int n) {
            for (int i = 0; i < n; i++)
            {
                setSmall.erase(i);
            }

            return setSmall.size();
        };
        
        BENCHMARK("1000")
        {
            return benchmarker(1000);
        };
        BENCHMARK("2000")
        {
            return benchmarker(2000);
        };
        BENCHMARK("3000")
        {
            return benchmarker(3000);
        };
        BENCHMARK("4000")
        {
            return benchmarker(4000);
        };
        BENCHMARK("5000")
        {
            return benchmarker(5000);
        };
        BENCHMARK("6000")
        {
            return benchmarker(6000);
        };
        BENCHMARK("7000")
        {
            return benchmarker(7000);
        };
        BENCHMARK("8000")
        {
            return benchmarker(8000);
        };
        BENCHMARK("9000")
        {
            return benchmarker(9000);
        };
        BENCHMARK("10000")
        {
            return benchmarker(10000);
        };
    }

    SECTION("Big data")
    {
        CSparseSet<BigData> setBig;
        
        for (int i = 0; i < 50000; i++)
        {
            setBig.insert(i, BigData{});
        }
        
        auto benchmarker = [&setBig](const int n) {
            for (int i = 0; i < n; i++)
            {
                setBig.erase(i);
            }

            return setBig.size();
        };
        
        BENCHMARK("1000")
        {
            return benchmarker(1000);
        };
        BENCHMARK("2000")
        {
            return benchmarker(2000);
        };
        BENCHMARK("3000")
        {
            return benchmarker(3000);
        };
        BENCHMARK("4000")
        {
            return benchmarker(4000);
        };
        BENCHMARK("5000")
        {
            return benchmarker(5000);
        };
        BENCHMARK("6000")
        {
            return benchmarker(6000);
        };
        BENCHMARK("7000")
        {
            return benchmarker(7000);
        };
        BENCHMARK("8000")
        {
            return benchmarker(8000);
        };
        BENCHMARK("9000")
        {
            return benchmarker(9000);
        };
        BENCHMARK("10000")
        {
            return benchmarker(10000);
        };
    }
}