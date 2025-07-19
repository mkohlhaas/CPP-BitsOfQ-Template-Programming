#pragma once

#include "Metaprogramming.h"

namespace bits_of_q
{
    // tuple

    // empty tuple ✓
    template <typename...>
    struct Tuple
    {
    };

    // tuple with at least one element ✓
    template <typename ELEM0, typename... ELEMS1toN>
    struct Tuple<ELEM0, ELEMS1toN...> : Tuple<ELEMS1toN...>
    {
        ELEM0 data;

        // constructor
        template <typename T, typename... Ts>
        explicit constexpr Tuple(T &&e0, Ts &&...rest) : Tuple<ELEMS1toN...>(std::forward<Ts>(rest)...), data(std::forward<T>(e0))
        {
        }
    };

    // deduction guide to make template argument deduction for constructors work (C++17)
    // deduction guide: mapping from function (in this case constructor) -> type ✓
    template <typename... Ts>
    Tuple(Ts...) -> Tuple<std::unwrap_ref_decay_t<Ts>...>; // remove const and refs from type

    static_assert(std::is_same_v<std::unwrap_ref_decay_t<int>, int>);
    static_assert(std::is_same_v<std::unwrap_ref_decay_t<const int>, int>);
    static_assert(std::is_same_v<std::unwrap_ref_decay_t<const int &>, int>);

    // helper: make_tuple ✓
    template <typename... ELEMS>
    auto
    make_tuple(ELEMS &&...elems)
    {
        return Tuple{std::forward<ELEMS>(elems)...};
    }

    // get

    namespace detail
    {
        // recur TUPLE ✓
        template <size_t i, typename TUPLE>
        struct get_impl : get_impl<i - 1, pop_front_t<TUPLE>>
        {
        };

        // element at index 0 (base case of recursion) ✓
        template <typename TUPLE>
        struct get_impl<0, TUPLE>
        {
            template <typename T>
            static decltype(auto)
            get(T &&t)
            {
                constexpr bool T_is_lvalue_ref = std::is_lvalue_reference_v<T>;
                constexpr bool T_is_const      = std::is_const_v<std::remove_reference_t<T>>;

                using data_t = front_t<TUPLE>;

                if constexpr (T_is_const && T_is_lvalue_ref)
                {
                    // const &
                    return static_cast<const data_t &>(static_cast<const TUPLE &>(t).data); // cast t, take data, cast data
                }
                if constexpr (!T_is_const && T_is_lvalue_ref)
                {
                    // &
                    return static_cast<data_t &>(static_cast<TUPLE &>(t).data);
                }
                if constexpr (T_is_const && !T_is_lvalue_ref)
                {
                    // const &&
                    return static_cast<const data_t &&>(static_cast<const TUPLE &&>(t).data);
                }
                if constexpr (!T_is_const && !T_is_lvalue_ref)
                {
                    // &&
                    return static_cast<data_t &&>(static_cast<TUPLE &&>(t).data);
                }
            }
        };
    } // namespace detail

    // size of tuple ✓

    template <typename TUPLE>
    struct tuple_size;

    template <typename... ELEMS>
    struct tuple_size<Tuple<ELEMS...>> : std::integral_constant<size_t, sizeof...(ELEMS)>
    {
    };

    template <typename TUPLE>
    static constexpr size_t tuple_size_v = tuple_size<TUPLE>::value;

    // forward as tuple (create a tuple from forwarding refs) ✓
    // make_tuple removes all refs and consts, forward_as_tuple doesn't
    template <typename... Ts>
    auto
    forward_as_tuple(Ts &&...ts) noexcept
    {
        return Tuple<Ts &&...>{std::forward<Ts>(ts)...};
    }

    namespace detail
    {
        // make_tuple_from_fwd_tuple ✓

        template <typename INDEX_SEQ>
        struct make_tuple_from_fwd_tuple;

        // index_sequence is any sequence of indices, e.g. 3, 5, 1, 9, ....
        template <size_t... indices>
        struct make_tuple_from_fwd_tuple<std::index_sequence<indices...>>
        {
            template <typename FWD_TUPLE>
            static auto
            f(FWD_TUPLE &&fwd)
            {
                // `get` is from API
                return Tuple{get<indices>(std::forward<FWD_TUPLE>(fwd))...};
            }
        };

        // concat_with_fwd_tuple ✓

        template <typename FWD_INDEX_SEQ, typename TUPLE_INDEX_SEQ>
        struct concat_with_fwd_tuple;

        // fwd = forward, nrm = normal
        template <size_t... fwd_indices, size_t... nrm_indices>
        struct concat_with_fwd_tuple<std::index_sequence<fwd_indices...>, std::index_sequence<nrm_indices...>>
        {
            template <typename FWD_TUPLE, typename NRM_TUPLE>
            static auto
            f(FWD_TUPLE &&fwd, NRM_TUPLE &&t)
            {
                return forward_as_tuple(get<fwd_indices>(std::forward<FWD_TUPLE>(fwd))..., //
                                        get<nrm_indices>(std::forward<NRM_TUPLE>(t))...);
            }
        };

        // tuple_cat_impl ✓

        struct tuple_cat_impl
        {
            template <typename FWD_TUPLE, typename NRM_TUPLE, typename... TUPLES>
            static auto
            f(FWD_TUPLE &&fwd, NRM_TUPLE &&t, TUPLES &&...ts)
            {
                return f(
                    concat_with_fwd_tuple<
                        // NOTE: Why is remove_cvref_t needed ? tuple_size_v doesn't work with const, ref tuple types (see video at 6:25)
                        std::make_index_sequence<tuple_size_v<std::remove_cvref_t<FWD_TUPLE>>>,
                        std::make_index_sequence<tuple_size_v<std::remove_cvref_t<NRM_TUPLE>>>>::f(std::forward<FWD_TUPLE>(fwd),
                                                                                                   std::forward<NRM_TUPLE>(t)),
                    std::forward<TUPLES>(ts)...);
            }

            static_assert(std::is_same_v<std::remove_cvref_t<int>, int>);
            static_assert(std::is_same_v<std::remove_cvref_t<int &>, int>);
            static_assert(std::is_same_v<std::remove_cvref_t<int &&>, int>);
            static_assert(std::is_same_v<std::remove_cvref_t<const int &>, int>);

            // function overload for recursion base case
            template <typename FWD_TUPLE>
            static auto
            f(FWD_TUPLE &&ret)
            {
                return make_tuple_from_fwd_tuple<std::make_index_sequence<tuple_size_v<FWD_TUPLE>>>::f(std::forward<FWD_TUPLE>(ret));
            }
        };

    } // namespace detail

    // API

    // get

    template <size_t i, typename TUPLE>
    decltype(auto)
    get(TUPLE &&tuple)
    {
        return detail::get_impl<i, std::remove_cvref_t<TUPLE>>::get(std::forward<TUPLE>(tuple));
    }

    // tuple cat

    template <typename... TUPLES>
    auto
    tuple_cat(TUPLES &&...tuples)
    {
        return detail::tuple_cat_impl::f(std::forward<TUPLES>(tuples)...);
    }
} // namespace bits_of_q
