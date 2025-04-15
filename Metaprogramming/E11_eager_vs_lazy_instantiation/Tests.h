#ifndef BOQ_TESTS_METAPROGRAMMING_H
#define BOQ_TESTS_METAPROGRAMMING_H

#include <tuple>
#include <type_traits>

#include "TypeList.h"

// different implementations
#include "Naive.h"
#include "NaiveLazy.h"

#define remove_if_tests                                                                                                \
    static_assert(std::is_same_v<remove_if_t<std::is_floating_point, type_list<int, bool, float, double, char>>,       \
                                 type_list<int, bool, char>>);                                                         \
                                                                                                                       \
    static_assert(                                                                                                     \
        std::is_same_v<remove_if_t<std::is_floating_point, type_list<int, bool, char>>, type_list<int, bool, char>>);  \
                                                                                                                       \
    static_assert(std::is_same_v<remove_if_t<std::is_floating_point, type_list<float, double, float>>, type_list<>>)

/*
// homework exercise: update the different implementations to allow them to operate on any list-like type, such as the
// std::tuple and make the following static_assert pass.
static_assert(std::is_same_v<remove_if_t<std::is_floating_point, std::tuple<int, bool, float, double, char>>,       \
                               std::tuple<int, bool, char>>);                                                       \
*/

namespace detail
{
    template <typename T>
    struct to_void
    {
        using type = void;
    };
} // namespace detail

namespace bits_of_q::naive
{
    remove_if_tests;
} // namespace bits_of_q::naive

namespace bits_of_q::naive_lazy
{
    remove_if_tests;
} // namespace bits_of_q::naive_lazy

// more implementations will follow in the next episodes!
#endif
