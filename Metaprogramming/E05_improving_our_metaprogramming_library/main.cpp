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

static_assert(std::is_same_v<typename if_<(10 > 5), int, bool>::type, int>);
static_assert(std::is_same_v<typename if_<(10 < 5), int, bool>::type, bool>);

static_assert(std::is_same_v<if_t<(10 > 5), int, bool>, int>);
static_assert(std::is_same_v<if_t<(10 < 5), int, bool>, bool>);

// type_list ✓

template <typename...>
struct type_list
{
};

// empty ✓

template <typename LIST>
struct empty : std::false_type
{
};

template <>
struct empty<type_list<>> : std::true_type
{
};

template <typename LIST>
inline constexpr bool empty_v = empty<LIST>::value;

static_assert(empty_v<type_list<>>);
static_assert(empty_v<type_list<int, bool>> == false);

// front ✓

template <typename LIST>
struct front;

template <typename T0, typename... T1toN>
struct front<type_list<T0, T1toN...>> : has_type<T0>
{
};

template <typename LIST>
using front_t = typename front<LIST>::type;

static_assert(std::is_same_v<front_t<type_list<int, bool, float>>, int>);

// pop_front ✓

template <typename LIST>
struct pop_front;

template <typename T0, typename... T1toN>
struct pop_front<type_list<T0, T1toN...>> : has_type<type_list<T1toN...>>
{
};

template <typename LIST>
using pop_front_t = typename pop_front<LIST>::type;

static_assert(std::is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);

// back ✓

// prerequisite: type_list is not empty
template <typename LIST>
struct back : has_type<typename back<pop_front_t<LIST>>::type>
{
};

template <typename T0>
struct back<type_list<T0>> : has_type<T0>
{
};

template <typename LIST>
using back_t = typename back<LIST>::type;

static_assert(std::is_same_v<back_t<type_list<int, bool, float>>, float>);
static_assert(std::is_same_v<back_t<type_list<int, bool, bool>>, bool>);

// push_back ✓

template <typename LIST, typename T>
struct push_back;

template <typename... T0toN, typename T>
struct push_back<type_list<T0toN...>, T> : has_type<type_list<T0toN..., T>>
{
};

template <typename LIST, typename T>
using push_back_t = typename push_back<LIST, T>::type;

static_assert(std::is_same_v<push_back_t<type_list<>, int>, type_list<int>>);
static_assert(std::is_same_v<push_back_t<type_list<int, bool>, float>, type_list<int, bool, float>>);

// pop_back ✓

// prerequisite: type_list is not empty
template <typename LIST, typename RET_LIST = type_list<>>
struct pop_back;

template <typename T0, typename RET_LIST>
struct pop_back<type_list<T0>, RET_LIST> : has_type<RET_LIST>
{
};

template <typename T0, typename... T1toN, typename RET_LIST>
struct pop_back<type_list<T0, T1toN...>, RET_LIST> : pop_back<type_list<T1toN...>, push_back_t<RET_LIST, T0>>
{
};

template <typename LIST>
using pop_back_t = typename pop_back<LIST>::type;

static_assert(std::is_same_v<pop_back_t<type_list<int>>, type_list<>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool, float>>, type_list<int, bool>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool>>, type_list<int>>);

// at ✓

template <typename LIST, std::size_t index>
struct at : has_type<typename at<pop_front_t<LIST>, index - 1>::type>
{
};

template <typename LIST>
struct at<LIST, 0> : has_type<front_t<LIST>>
{
};

template <typename LIST, std::size_t index>
using at_t = typename at<LIST, index>::type;

static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 0>, int>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 1>, bool>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 2>, float>);

// contains_type ✓

template <typename SEARCH, typename LIST>
struct contains_type : if_< // IF
                           std::is_same_v<SEARCH, front_t<LIST>>,
                           // THEN
                           std::true_type,
                           // ELSE
                           contains_type<SEARCH, pop_front_t<LIST>>>::type
{
};

template <typename SEARCH>
struct contains_type<SEARCH, type_list<>> : std::false_type
{
};

template <typename SEARCH, typename LIST>
inline constexpr bool contains_type_v = contains_type<SEARCH, LIST>::value;

static_assert(contains_type_v<int, type_list<int, bool, float>>);
static_assert(contains_type_v<float, type_list<int, bool, float>>);
static_assert(contains_type_v<double, type_list<int, bool, float>> == false);

// main

int
main()
{
    std::cout << std::boolalpha;

    type_list<int, bool, double> types;
    std::cout << contains_type<bool, type_list<>>::value << '\n';      // false
    std::cout << contains_type<bool, decltype(types)>::value << '\n';  // true
    std::cout << contains_type<float, decltype(types)>::value << '\n'; // false

    std::cout << contains_type_v<bool, type_list<>> << '\n';           // false
    std::cout << contains_type_v<bool, decltype(types)> << '\n';       // true
    std::cout << contains_type_v<float, decltype(types)> << '\n';      // false
}
