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

template <bool condition, typename THEN, typename ELSE>
using if_t = if_<condition, THEN, ELSE>::type;

static_assert(std::is_same_v<if_t<(10 > 5), int, bool>, int>);
static_assert(std::is_same_v<if_t<10 < 5, int, bool>, bool>);

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
constexpr bool empty_v = empty<LIST>::value;

static_assert(empty_v<type_list<>>);
static_assert(not empty_v<type_list<int, bool>>);

// front ✓

template <typename LIST>
struct front;

// LIST is a template with any number of type parameters
template <template <typename...> typename LIST, typename T, typename... Ts>
struct front<LIST<T, Ts...>> : has_type<T>
{
};

template <typename LIST>
using front_t = front<LIST>::type;

static_assert(std::is_same_v<front_t<type_list<int, bool, float>>, int>);

// pop_front ✓

template <typename LIST>
struct pop_front;

template <template <typename...> typename LIST, typename T, typename... Ts>
struct pop_front<LIST<T, Ts...>> : has_type<type_list<Ts...>>
{
};

template <typename LIST>
using pop_front_t = pop_front<LIST>::type;

static_assert(std::is_same_v<pop_front_t<type_list<int, bool, float>>, type_list<bool, float>>);

// back ✓

template <typename LIST>
struct back : has_type<typename back<pop_front_t<LIST>>::type>
{
};

template <template <typename...> typename LIST, typename T>
struct back<LIST<T>> : has_type<T>
{
};

template <typename LIST>
using back_t = back<LIST>::type;

static_assert(std::is_same_v<back_t<type_list<int, bool, float>>, float>);
static_assert(std::is_same_v<back_t<type_list<int, bool>>, bool>);

// push_back ✓

template <typename LIST, typename T>
struct push_back;

template <template <typename...> typename LIST, typename... Ts, typename T>
struct push_back<LIST<Ts...>, T> : has_type<LIST<Ts..., T>>
{
};

template <typename LIST, typename T>
using push_back_t = push_back<LIST, T>::type;

static_assert(std::is_same_v<push_back_t<type_list<>, int>, type_list<int>>);
static_assert(std::is_same_v<push_back_t<type_list<int, bool>, float>, type_list<int, bool, float>>);

// pop_back ✓

template <typename LIST, typename RET_LIST = type_list<>>
struct pop_back;

template <template <typename...> typename LIST, typename RET_LIST>
struct pop_back<LIST<>, RET_LIST> : has_type<RET_LIST>
{
};

template <template <typename...> typename LIST, typename T, typename RET_LIST>
struct pop_back<LIST<T>, RET_LIST> : has_type<RET_LIST>
{
};

template <template <typename...> typename LIST, typename T, typename... Ts, typename RET_LIST>
struct pop_back<LIST<T, Ts...>, RET_LIST> : pop_back<LIST<Ts...>, push_back_t<RET_LIST, T>>
{
};

template <typename LIST>
using pop_back_t = pop_back<LIST>::type;

static_assert(std::is_same_v<pop_back_t<type_list<>>, type_list<>>);
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
using at_t = at<LIST, index>::type;

static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 0>, int>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 1>, bool>);
static_assert(std::is_same_v<at_t<type_list<int, bool, float>, 2>, float>);

// any ✓

template <template <typename> typename PREDICATE, typename LIST>
struct any;

// template <typename> typename PREDICATE           = template with one template parameter
// template <typename, typename> typename PREDICATE = template with two template parameters
// template <typename...> typename PREDICATE        = template with any number of template parameters
template <template <typename> typename PREDICATE, template <typename...> typename LIST>
struct any<PREDICATE, LIST<>> : std::false_type
{
};

template <template <typename> typename PREDICATE, typename LIST>
using any_t = any<PREDICATE, LIST>::type;

template <template <typename> typename PREDICATE, typename LIST>
struct any : if_t<PREDICATE<front_t<LIST>>::value,     // IF
                  std::true_type,                      // THEN
                  any_t<PREDICATE, pop_front_t<LIST>>> // ELSE
{
};

template <template <typename> typename PREDICATE, typename LIST>
constexpr bool any_v = any<PREDICATE, LIST>::value;

static_assert(any_v<std::is_integral, type_list<int, double, std::string>>);
static_assert(any_v<std::is_integral, type_list<std::string, double, int>>);
static_assert(not any_v<std::is_integral, type_list<std::string, double, float>>);

// contains_type ✓

// Currying!
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

// Currying structure:
//
// Starting point: struct_predicate (in this case two type parameters)
//
// template <typename T>
// struct base
// {
//     template <typename U>
//     struct derived : struct_predicate<T, U>
//     {
//     };
// };

// Access:
// base<T>::template derived<U>
//
// with compiler's type deduction:
// base<T>::template derived
//
// maybe even this:
// base::template derived

template <typename SEARCH, typename LIST>
constexpr bool contains_type_v = any<same_as_pred<SEARCH>::template predicate, LIST>::value;

static_assert(contains_type_v<int, type_list<int, bool, float>>);
static_assert(contains_type_v<float, type_list<int, bool, float>>);
static_assert(not contains_type_v<double, type_list<int, bool, float>>);

// List2 ✓

template <typename...>
struct LIST2
{
};

// main

int
main()
{
    std::cout << std::boolalpha;

    // front_t, contains_type_v can be used with all sorts of "lists"

    {
        // 1. Type Lists

        puts("=== Type Lists");

        type_list<int, float, bool> l;

        // front_t
        static_assert(std::is_same_v<front_t<decltype(l)>, int>);

        // contains_type_v
        bool b = contains_type_v<bool, decltype(l)>;

        std::cout << b << std::endl; // true
    }

    {
        // 2. Tuples

        puts("=== Tuples");

        std::tuple<int, float, bool> l;

        // front_t
        static_assert(std::is_same_v<front_t<decltype(l)>, int>);
        front_t<decltype(l)> fst = 42;

        // contains_type_v
        bool b = contains_type_v<bool, decltype(l)>;

        std::cout << fst << std::endl; // 42
        std::cout << b << std::endl;   // true
    }

    {
        // 3. LIST2s

        puts("=== List2");

        LIST2<int, bool, bool> l;

        // front_t
        static_assert(std::is_same_v<front_t<decltype(l)>, int>);
        front_t<decltype(l)> fst = 42;

        // contains_type_v
        bool b = contains_type_v<bool, decltype(l)>;

        std::cout << fst << std::endl; // 42
        std::cout << b << std::endl;   // true
    }
}
