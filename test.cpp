#include "core.hxx"
#include <iostream>

using namespace JKT::core;

template <class T>
enable_if_t<!is_same<T, char>::value, T> f() {
    return T(6);
}

template <class T>
enable_if_t<is_any_of<T, char>::value && sizeof(T) == 1, T> f() {
    return T('$');
}

// template <typename T, enable_if_t>
// struct g {
//     using type = int;
// };

// template <typename T, JKT_STRUCT(is_any_of<T, int, char, float, double>::value)>
// struct g {
//     using type = T;
// };

int main() {
    std::cout << is_integer<int>::value << std::endl;
    std::cout << f<char>() << std::endl;
    std::cout << f<float>() << std::endl;
    std::cout << f<double>() << std::endl;
    return 0;
}