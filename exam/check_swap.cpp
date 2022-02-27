//
// Created by mikedepetris on 27/02/2022.
//

#include <iostream>

template<typename T>
void bad_swap(T &x, T &y) {
    auto tmp = x;
    x = y;
    y = tmp;
}

template<typename T>
void swap(T &x, T &y) {
    auto tmp = std::move(x); // move ctor
    x = std::move(y);        // move assign.
    y = std::move(tmp);      // move assign.
}

int main() {
    int a{2};
    int b{7};
    std::cout << "a=" << a << ", b=" << b << std::endl;
    swap(a,b);
    std::cout << "a=" << a << ", b=" << b << std::endl;
}