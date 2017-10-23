#include "test.h"
#include <catch.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>
#include <google/protobuf/stubs/common.h>

int test_range_zip()
{
    using namespace ranges;
    // zip_with
    {
        std::vector<std::string> v0{ "a", "b", "c" };
        std::vector<std::string> v1{ "x", "y", "z" };
        auto rng = view::zip_with(std::plus<std::string> {}, v0, v1);
        std::vector<std::string> expected;
        copy(rng, ranges::back_inserter(expected));
        std::for_each(expected.begin(), expected.end(), [](auto const & el) { std::cout << el << " "; });
        std::cout << std::endl;

        for (auto i : view::ints(0, 11) | view::remove_if([](int i) {return i % 2 == 1; }) | view::transform([](int i) {return i * i; }))
        { std::cout << i << ' '; }
        std::cout << '\n';

        for (auto i : view::for_each(view::ints(1, 10), [](int i) {return yield_from(view::repeat_n(i, i));}))
        { std::cout << i << ' '; }
        std::cout << '\n';
    }
    return 0;
}
