#ifndef BOQ_TESTS_METAPROGRAMMING_H
#define BOQ_TESTS_METAPROGRAMMING_H

#include <tuple>
#include <type_traits>

#include "TypeList.h"

// different implementations
#include "Composed.h"
#include "ComposedDefaults.h"
#include "ComposedDefaults64.h"
#include "ComposedFastTrack.h"
#include "ComposedFastTrack64.h"
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

#define transform_tests                                                                                                \
    static_assert(                                                                                                     \
        std::is_same_v<transform_t<type_list<int, bool, float>, ::detail::to_void>, type_list<void, void, void>>)

#define join_tests                                                                                                     \
    static_assert(                                                                                                     \
        std::is_same_v<join_t<type_list<int, bool>, type_list<float, double>>, type_list<int, bool, float, double>>);  \
                                                                                                                       \
    static_assert(std::is_same_v<join_t<type_list<>, type_list<int, bool>>, type_list<int, bool>>);                    \
    static_assert(std::is_same_v<join_t<type_list<int, float>, type_list<int, bool>, type_list<float, double>>,        \
                                 type_list<int, float, int, bool, float, double>>)

namespace bits_of_q::naive
{
    remove_if_tests;
} // namespace bits_of_q::naive

namespace bits_of_q::naive_lazy
{
    remove_if_tests;
} // namespace bits_of_q::naive_lazy

namespace bits_of_q::composed
{
    join_tests;
    transform_tests;
    remove_if_tests;
} // namespace bits_of_q::composed

namespace bits_of_q::composed_fast_track
{
    join_tests;
    transform_tests;
    remove_if_tests;
} // namespace bits_of_q::composed_fast_track

namespace bits_of_q::composed_fast_track64
{
    join_tests;
    transform_tests;
    remove_if_tests;
} // namespace bits_of_q::composed_fast_track64

namespace bits_of_q::composed_defaults
{
    join_tests;
    transform_tests;
    remove_if_tests;
} // namespace bits_of_q::composed_defaults

namespace bits_of_q::composed_defaults64
{
    join_tests;
    transform_tests;
    remove_if_tests;
} // namespace bits_of_q::composed_defaults64

#endif
