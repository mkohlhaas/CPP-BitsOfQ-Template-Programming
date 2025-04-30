#include <iostream>
#include <list>
#include <string>

// if_ ✓

template <bool condition, typename THEN, typename ELSE>
struct if_;

template <typename THEN, typename ELSE>
struct if_<true, THEN, ELSE>
{
    using type = THEN;
};

template <typename THEN, typename ELSE>
struct if_<false, THEN, ELSE>
{
    using type = ELSE;
};

template <bool condition, typename THEN, typename ELSE>
using if_t = typename if_<condition, THEN, ELSE>::type;

// type_list ✓

template <typename...>
struct type_list
{
};

// empty ✓

// not needed any more (contains_type changed)

template <typename LIST>
struct empty : std::false_type
{
};

template <>
struct empty<type_list<>> : std::true_type
{
};

template <typename LIST>
inline constexpr bool empty_v = empty<LIST>::value; // ::value as inline constexpr

static_assert(empty<type_list<>>::value);           // empty
static_assert(!empty<type_list<float>>::value);     // not empty

static_assert(empty_v<type_list<>>);                // empty
static_assert(!empty_v<type_list<float>>);          // not empty

// front ✓

template <typename LIST>
struct front;

template <typename T0, typename... T1toN>
struct front<type_list<T0, T1toN...>>
{
    using type = T0;
};

template <typename LIST>
using front_t = typename front<LIST>::type; // ::type as type alias

static_assert(std::is_same_v<front<type_list<int, bool>>::type, int>);
static_assert(std::is_same_v<front_t<type_list<int, bool>>, int>);

// pop_front ✓

template <typename LIST>
struct pop_front;

template <typename T0, typename... T1toN>
struct pop_front<type_list<T0, T1toN...>>
{
    using type = type_list<T1toN...>;
};

template <typename LIST>
using pop_front_t = typename pop_front<LIST>::type;

static_assert(std::is_same_v<pop_front<type_list<int, bool, float>>::type, type_list<bool, float>>);
static_assert(std::is_same_v<front<pop_front<type_list<int, bool, float>>::type>::type, bool>);
static_assert(std::is_same_v<front<pop_front<pop_front<type_list<int, bool, float>>::type>::type>::type, float>);

static_assert(std::is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);
static_assert(std::is_same_v<front_t<pop_front_t<type_list<int, bool, float>>>, bool>);
static_assert(std::is_same_v<front_t<pop_front_t<pop_front_t<type_list<int, bool, float>>>>, float>);

// contains_type ✓

template <typename SEARCH, typename LIST>
struct contains_type : if_t< // IF
                           std::is_same_v<SEARCH, front_t<LIST>>,
                           // THEN
                           std::true_type,
                           // ELSE
                           contains_type<SEARCH, pop_front_t<LIST>>>
{
};

// specialization for empty type_list
template <typename SEARCH>
struct contains_type<SEARCH, type_list<>> : std::false_type
{
};

template <typename SEARCH, typename LIST>
inline constexpr bool contains_type_v = contains_type<SEARCH, LIST>::value;

// contains ✓

// run-time equivalent
bool
contains(const std::string &search, std::list<std::string> l)
{
    if (l.empty())
    {
        return false;
    }

    if (search == l.front())
    {
        return true;
    }

    l.pop_front();
    return contains(search, l);
}

int
main()
{
    // run-time equivalent
    std::list<std::string> list{"int", "bool", "double"};
    std::cout << std::boolalpha;
    std::cout << contains("bool", std::list<std::string>{}) << '\n';   // false
    std::cout << contains("bool", list) << '\n';                       // true
    std::cout << contains("float", list) << '\n';                      // false

    type_list<int, bool, double> types;
    std::cout << contains_type<bool, type_list<>>::value << '\n';      // false
    std::cout << contains_type<bool, decltype(types)>::value << '\n';  // true
    std::cout << contains_type<float, decltype(types)>::value << '\n'; // false

    std::cout << contains_type_v<bool, type_list<>> << '\n';           // false
    std::cout << contains_type_v<bool, decltype(types)> << '\n';       // true
    std::cout << contains_type_v<float, decltype(types)> << '\n';      // false
}
