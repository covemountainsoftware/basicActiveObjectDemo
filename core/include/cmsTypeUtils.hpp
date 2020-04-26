#ifndef CMSTYPEUTILS_HPP
#define CMSTYPEUTILS_HPP

#include <type_traits>

namespace cms
{

//"is_base_of_any" via: https://stackoverflow.com/a/32478174/49956

template<template<typename> class C>
struct is_base_of_any_helper
{
    template<typename T>
    std::true_type operator()(const C<T> *) const;

    std::false_type operator()(...) const;
};

template<template<typename> class C, typename T>
using is_base_of_any = decltype(is_base_of_any_helper<C>{}(std::declval<const T *>()));

} //namespace cms

#endif // CMSTYPEUTILS_HPP
