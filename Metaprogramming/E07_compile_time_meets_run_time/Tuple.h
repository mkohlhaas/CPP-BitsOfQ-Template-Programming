#pragma once

#include "Metaprogramming.h"
#include <functional>
#include <type_traits>

namespace bits_of_q
{
    template <typename... ELEMS>
    struct Tuple
    {
        constexpr Tuple() = default;
    };

    // specialization for tuple with at least one element
    template <typename ELEM0, typename... ELEMS1toN>
    struct Tuple<ELEM0, ELEMS1toN...> : Tuple<ELEMS1toN...>
    {
        ELEM0 data;

        template <typename T, typename... Ts>
        explicit constexpr Tuple(T &&e0, Ts &&...rest)
            : Tuple<ELEMS1toN...>(std::forward<Ts>(rest)...), data(std::forward<T>(e0))
        {
        }
    };

    // deduction guide to make template argument deduction for constructors work (C++17)
    template <typename T, typename... Ts>
    // Tuple(T, Ts...) -> Tuple<T, Ts...>;
    Tuple(T, Ts...) -> Tuple<std::unwrap_ref_decay_t<T>, std::unwrap_ref_decay_t<Ts>...>;

    template <typename... ELEMS>
    constexpr auto
    make_tuple(ELEMS &&...elems)
    {
        return Tuple<std::unwrap_ref_decay_t<ELEMS>...>{std::forward<ELEMS>(elems)...};
    }

    // get

    namespace detail
    {
        template <size_t i, typename TUPLE>
        struct get_impl : get_impl<i - 1, pop_front_t<TUPLE>>
        {
        };

        template <typename TUPLE>
        struct get_impl<0, TUPLE>
        {
            template <typename T>
            constexpr static decltype(auto)
            get(T &&t)
            {
                constexpr bool is_lvalue = std::is_lvalue_reference_v<T>;
                constexpr bool is_const  = std::is_const_v<std::remove_reference_t<T>>;

                using data_t = front_t<TUPLE>;

                if constexpr (is_lvalue && is_const)
                {
                    return static_cast<const data_t &>(static_cast<const TUPLE &>(t).data);
                }

                if constexpr (is_lvalue && !is_const)
                {
                    return static_cast<data_t &>(static_cast<TUPLE &>(t).data);
                }

                if constexpr (!is_lvalue && is_const)
                {
                    return static_cast<const data_t &&>(static_cast<const TUPLE &&>(t).data);
                }

                if constexpr (!is_lvalue && !is_const)
                {
                    return static_cast<data_t &&>(static_cast<TUPLE &&>(t).data);
                }
            }
        };
    } // namespace detail

    template <size_t i, typename TUPLE>
    constexpr decltype(auto)
    get(TUPLE &&tuple)
    {
        return detail::get_impl<i, std::remove_cvref_t<TUPLE>>::get(std::forward<TUPLE>(tuple));
    }
} // namespace bits_of_q
