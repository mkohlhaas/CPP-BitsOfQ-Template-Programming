#include <iostream>
#include <string>
#include <tuple>
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

static_assert(std::is_same_v<typename if_<(10 > 5), int, bool>::type, int>);
static_assert(std::is_same_v<typename if_<(10 < 5), int, bool>::type, bool>);

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

template <template <typename...> typename LIST>
struct empty<LIST<>> : std::true_type
{
};

template <typename LIST>
inline constexpr bool empty_v = empty<LIST>::value;

static_assert(empty_v<type_list<>>);
static_assert(empty_v<type_list<int, bool>> == false);

// front ✓

template <typename LIST>
struct front;

template <template <typename...> typename LIST, typename T0, typename... T1toN>
struct front<LIST<T0, T1toN...>> : has_type<T0>
{
};

template <typename LIST>
using front_t = typename front<LIST>::type;

static_assert(std::is_same_v<front_t<type_list<int, bool, float>>, int>);

// pop_front ✓

template <typename LIST>
struct pop_front;

template <template <typename...> typename LIST, typename T0, typename... T1toN>
struct pop_front<LIST<T0, T1toN...>> : has_type<type_list<T1toN...>>
{
};

template <typename LIST>
using pop_front_t = typename pop_front<LIST>::type;

static_assert(std::is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);

// back ✓

template <typename LIST>
struct back : has_type<typename back<pop_front_t<LIST>>::type>
{
};

template <template <typename...> typename LIST, typename T0>
struct back<LIST<T0>> : has_type<T0>
{
};

template <typename LIST>
using back_t = typename back<LIST>::type;

static_assert(std::is_same_v<back_t<type_list<int, bool, float>>, float>);
static_assert(std::is_same_v<back_t<type_list<int, bool>>, bool>);

// push_back ✓

template <typename LIST, typename T>
struct push_back;

template <template <typename...> typename LIST, typename... T0toN, typename T>
struct push_back<LIST<T0toN...>, T> : has_type<LIST<T0toN..., T>>
{
};

template <typename LIST, typename T>
using push_back_t = typename push_back<LIST, T>::type;

static_assert(std::is_same_v<push_back_t<type_list<>, int>, type_list<int>>);
static_assert(std::is_same_v<push_back_t<type_list<int, bool>, float>, type_list<int, bool, float>>);

// pop_back ✓

template <typename LIST, typename RET_LIST = type_list<>>
struct pop_back;

template <template <typename...> typename LIST, typename T0, typename RET_LIST>
struct pop_back<LIST<T0>, RET_LIST> : has_type<RET_LIST>
{
};

template <template <typename...> typename LIST, typename T0, typename T1, typename... T2toN, typename RET_LIST>
struct pop_back<LIST<T0, T1, T2toN...>, RET_LIST> : pop_back<LIST<T1, T2toN...>, push_back_t<RET_LIST, T0>>
{
};

template <typename LIST>
using pop_back_t = typename pop_back<LIST>::type;

static_assert(std::is_same_v<pop_back_t<type_list<int>>, type_list<>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool, float>>, type_list<int, bool>>);
static_assert(std::is_same_v<pop_back_t<type_list<int, bool>>, type_list<int>>);

// at ✓

template <typename LIST, size_t index>
struct at : has_type<typename at<pop_front_t<LIST>, index - 1>::type>
{
};

template <typename LIST>
struct at<LIST, 0> : has_type<front_t<LIST>>
{
};

template <typename LIST, size_t index>
using at_t = typename at<LIST, index>::type;

static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 1>, bool>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 2>, float>);

// any

// PREDICATE takes one type variable.
template <template <typename> typename PREDICATE, typename LIST>
struct any;

// LIST takes variadic number of type variables. (LIST is now more specialized.)
template <template <typename> typename PREDICATE, template <typename...> typename LIST>
struct any<PREDICATE, LIST<>> : std::false_type
{
};

template <template <typename> typename PREDICATE, typename LIST>
struct any : if_< // if predicate matches first type
                 PREDICATE<front_t<LIST>>::value,
                 // then
                 std::true_type,
                 // else
                 typename any<PREDICATE, pop_front_t<LIST>>::type>::type
{
};

template <template <typename> typename PREDICATE, typename LIST>
inline constexpr bool any_v = any<PREDICATE, LIST>::value;

static_assert(any_v<std::is_integral, type_list<int, double, std::string>>);
static_assert(any_v<std::is_integral, type_list<std::string, double, int>>);
static_assert(!any_v<std::is_integral, type_list<std::string, double, float>>);

// contains_type

// currying
// std::is_same has two parameters.
// We create a curried version of it where we specify the first parameter.
// same_as_pred has only one parameter/type variable.
template <typename T>
struct same_as_pred
{
    template <typename U>
    struct predicate : std::is_same<T, U>
    {
    };
};

template <typename SEARCH, typename LIST>
inline constexpr bool contains_type_v = any<same_as_pred<SEARCH>::template predicate, LIST>::value;

static_assert(contains_type_v<int, type_list<int, bool, float>>);
static_assert(contains_type_v<float, type_list<int, bool, float>>);
static_assert(contains_type_v<double, type_list<int, bool, float>> == false);

// List2 ✓

template <typename...>
struct LIST2
{
};

// main

int
main()
{
    // type_lists work with front_t
    type_list<int, float, bool> l;
    static_assert(std::is_same_v<front_t<decltype(l)>, int>);

    // tuples work with front_t
    std::tuple<int, float, bool> t1;
    front_t<decltype(t1)>        first1 = 42;

    // List2 works with front_t
    LIST2<int, bool, bool> l2;
    front_t<decltype(l2)>  first2 = 42;

    // contains_type_v works with all sorts of listy things
    bool b1 = contains_type_v<bool, decltype(l)>;
    bool b2 = contains_type_v<bool, decltype(t1)>;
    bool b3 = contains_type_v<bool, decltype(l2)>;

    std::cout << std::boolalpha;
    std::cout << first1 << '\n'; // 42
    std::cout << first2 << '\n'; // 42
    std::cout << b1 << '\n';     // true
    std::cout << b2 << '\n';     // true
    std::cout << b3 << '\n';     // true
}
