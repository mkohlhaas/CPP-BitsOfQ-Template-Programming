#include <iostream>
#include <type_traits>

// has_type ✓

template <typename T>
struct has_type
{
    using type = T;
};

// if_ ✓

template <bool condition, typename THEN, typename ELSE>
struct if_;

template <typename THEN, typename ELSE>
struct if_<true, THEN, ELSE> : has_type<THEN>
{
};

template <typename THEN, typename ELSE>
struct if_<false, THEN, ELSE> : has_type<ELSE>
{
};

template <bool condition, typename THEN, typename ELSE>
using if_t = typename if_<condition, THEN, ELSE>::type;

static_assert(std::is_same_v<if_t<(10 > 5), int, bool>, int>);
static_assert(std::is_same_v<if_t<(10 < 5), int, bool>, bool>);

// type_list ✓

template <typename...>
struct type_list
{
};

// empty ✓

template <typename TL>
struct empty : std::false_type
{
};

template <>
struct empty<type_list<>> : std::true_type
{
};

template <typename TL>
inline constexpr bool empty_v = empty<TL>::value;

static_assert(empty_v<type_list<>>);
static_assert(empty_v<type_list<int, bool>> == false);

// front ✓

template <typename TL>
struct front;

template <typename T, typename... Ts>
struct front<type_list<T, Ts...>> : has_type<T>
{
};

template <typename TL>
using front_t = typename front<TL>::type;

static_assert(std::is_same_v<front_t<type_list<int, bool, float>>, int>);

// pop_front ✓

template <typename TL>
struct pop_front;

template <typename T, typename... Ts>
struct pop_front<type_list<T, Ts...>> : has_type<type_list<Ts...>>
{
};

template <typename TL>
using pop_front_t = typename pop_front<TL>::type;

static_assert(std::is_same_v<pop_front_t<type_list<float>>, type_list<>>);
static_assert(std::is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);

// back ✓

// prerequisite: type_list is not empty
template <typename TL>
struct back : has_type<typename back<pop_front_t<TL>>::type>
{
};

template <typename T>
struct back<type_list<T>> : has_type<T>
{
};

template <typename TL>
using back_t = typename back<TL>::type;

static_assert(std::is_same_v<back_t<type_list<int, bool, float>>, float>);
static_assert(std::is_same_v<back_t<type_list<int, bool, bool>>, bool>);

// push_back ✓

template <typename TL, typename T>
struct push_back;

template <typename... Ts, typename T>
struct push_back<type_list<Ts...>, T> : has_type<type_list<Ts..., T>>
{
};

template <typename TL, typename T>
using push_back_t = typename push_back<TL, T>::type;

static_assert(std::is_same_v<push_back_t<type_list<>, int>, type_list<int>>);
static_assert(std::is_same_v<push_back_t<type_list<int, bool>, float>, type_list<int, bool, float>>);

// pop_back ✓

// prerequisite: type_list is not empty
template <typename TL, typename RET_TL = type_list<>>
struct pop_back;

template <typename T, typename RET_TL>
struct pop_back<type_list<T>, RET_TL> : has_type<RET_TL>
{
};

template <typename T, typename... Ts, typename RET_TL>
struct pop_back<type_list<T, Ts...>, RET_TL> : pop_back<type_list<Ts...>, push_back_t<RET_TL, T>>
{
};

template <typename TL>
using pop_back_t = typename pop_back<TL>::type;

static_assert(std::is_same_v<pop_back_t<type_list<int>>, type_list<>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool, float>>, type_list<int, bool>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool>>, type_list<int>>);

// at ✓

template <typename TL, std::size_t index>
struct at : has_type<typename at<pop_front_t<TL>, index - 1>::type>
{
};

template <typename TL>
struct at<TL, 0> : has_type<front_t<TL>>
{
};

template <typename TL, std::size_t index>
using at_t = typename at<TL, index>::type;

static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 0>, int>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 1>, bool>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 2>, float>);

// contains_type ✓

template <typename SEARCH, typename TL>
struct contains_type : if_t<
                           // IF
                           std::is_same_v<SEARCH, front_t<TL>>,
                           // THEN
                           std::true_type,
                           // ELSE
                           contains_type<SEARCH, pop_front_t<TL>>>
{
};

template <typename SEARCH>
struct contains_type<SEARCH, type_list<>> : std::false_type
{
};

template <typename SEARCH, typename TL>
inline constexpr bool contains_type_v = contains_type<SEARCH, TL>::value;

static_assert(contains_type_v<int, type_list<int, bool, float>>);
static_assert(contains_type_v<float, type_list<int, bool, float>>);
static_assert(not contains_type_v<double, type_list<int, bool, float>>);

// main

int
main()
{
    std::cout << std::boolalpha;

    type_list<int, bool, double> types;
    std::cout << contains_type_v<bool, type_list<>> << '\n';      // false
    std::cout << contains_type_v<bool, decltype(types)> << '\n';  // true
    std::cout << contains_type_v<float, decltype(types)> << '\n'; // false
}
