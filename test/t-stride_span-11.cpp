#include "doctest.hpp"

#include <itlib/stride_span.hpp>

#include <vector>
#include <cstring>

TEST_CASE("[stride_span] construction and iteration")
{
    using namespace itlib;

    {
        stride_span<int> e;
        CHECK(!e);
        CHECK(e.size() == 0);
        CHECK(e.begin() == e.end());
        CHECK(e.rbegin() == e.rend());
        CHECK(e.empty());
        CHECK(e.stride() == 4);
        CHECK(e.data() == nullptr);
    }

    {
        stride_span<const int> e;
        CHECK(!e);
        CHECK(e.size() == 0);
        CHECK(e.begin() == e.end());
        CHECK(e.rbegin() == e.rend());
        CHECK(e.empty());
        CHECK(e.stride() == 4);
        CHECK(e.data() == nullptr);
    }

    int i[] = {1,11,2,22,3,33,4,44};

    {
        stride_span<int> eints(reinterpret_cast<uint8_t*>(i), sizeof(int) * 2, 4);
        CHECK(eints);
        CHECK(!eints.empty());
        CHECK(eints.size() == 4);
        CHECK(eints.begin() + 4 == eints.end());
        CHECK(*eints.rbegin() == 4);

        CHECK(eints.front() == 1);
        CHECK(eints.back() == 4);

        CHECK(eints[0] == 1);

        int sum = 0;
        for (auto i : eints)
        {
            sum += i;
        }
        CHECK(sum == 10);

        sum = 0;
        for (auto i = eints.rbegin(); i != eints.rend(); ++i)
        {
            sum *= 10;
            sum += *i;
        }
        CHECK(sum == 4321);

        eints[1] = 5;
        CHECK(i[2] == 5);
    }

    {
        stride_span<const int> eints(reinterpret_cast<uint8_t*>(i + 1), sizeof(int) * 2, 4);
        CHECK(eints);
        CHECK(!eints.empty());
        CHECK(eints.size() == 4);
        CHECK(eints.begin() + 4 == eints.end());

        CHECK(*eints.rbegin() == 44);

        CHECK(eints.front() == 11);
        CHECK(eints.back() == 44);

        CHECK(eints[0] == 11);

        int sum = 0;
        for (auto i : eints)
        {
            sum += i;
        }
        CHECK(sum == 110);

        sum = 0;
        for (auto i = eints.rbegin(); i != eints.rend(); ++i)
        {
            sum *= 100;
            sum += *i;
        }
        CHECK(sum == 44332211);
    }
}

struct foo
{
    float x;
    int y;
    char z;
};

TEST_CASE("[stride_span] make_stride_span")
{
    using namespace itlib;

    {
        int i[] = {1,11,2,22,3,33,4,44};
        auto ss = make_stride_span_from_array(i, std::size(i), 0, 2);
        static_assert(std::is_same<stride_span<int>, decltype(ss)>::value, "make_stride_span_from_array");
        CHECK(ss.size() == 4);
        CHECK(ss[0] == 1);
        CHECK(ss.back() == 4);
    }

    {
        const int i[] = {1,11,2,22,3,33,4,44};
        auto ss = make_stride_span_from_array(i, std::size(i), 1, 2);
        static_assert(std::is_same<stride_span<const int>, decltype(ss)>::value, "make_stride_span_from_array");
        CHECK(ss.size() == 4);
        CHECK(ss[0] == 11);
        CHECK(ss.back() == 44);
    }

    {
        int i[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        auto s03 = make_stride_span_from_array(i, std::size(i), 0, 3);
        CHECK(s03.size() == 4);
        CHECK(s03[0] == 0);
        CHECK(s03.back() == 9);

        auto s13 = make_stride_span_from_array(i, std::size(i), 1, 3);
        CHECK(s13.size() == 3);
        CHECK(s13[0] == 1);
        CHECK(s13.back() == 7);
    }

    {
        foo f[] = {{2.3f, 1, 'a'}, {3.14f, 2, 'b'}, {6.2f, 3, 'c'}};
        auto ss = make_stride_span_member_view(f, std::size(f), &foo::y);
        static_assert(std::is_same<stride_span<int>, decltype(ss)>::value, "make_stride_span_member_view");
        CHECK(ss.size() == 3);
        CHECK(*ss.begin() == 1);
        CHECK(ss[1] == 2);
        CHECK(ss.back() == 3);
    }

    {
        const foo f[] = {{2.3f, 1, 'a'}, {3.14f, 2, 'b'}, {6.2f, 3, 'c'}};
        auto ss = make_stride_span_member_view(f, std::size(f), &foo::z);
        static_assert(std::is_same<stride_span<const char>, decltype(ss)>::value, "make_stride_span_member_view");
        CHECK(ss.size() == 3);
        CHECK(*ss.begin() == 'a');
        CHECK(ss[1] == 'b');
        CHECK(ss.back() == 'c');
    }
}
