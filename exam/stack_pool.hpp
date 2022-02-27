#pragma once

#include "ap_error.hpp"
#include <vector>
#include <iostream>

/*! \mainpage Created on 20/02/2022 for the AP exam.
 *
 * \section intro_sec A pool of blazingly fast stacks
 *
 * by mikedepetris
 *
 */

// namespace stackpool { can't use without changing tests.cpp, not compliant with exam requirements

struct Stack_pool_exception;

/**
 * iterator for the stack pool
 */
template<typename S, typename T, typename N>
class stack_pool_iterator {
    using stack_pool_type = S;
    using stack_type = N;

    stack_pool_type *const stack_pool;
    stack_type current;

public:
    /// must have all five member types for STL
    using value_type = T;
    using reference = value_type &;
    using pointer = value_type *;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    /**
     * called by custom single argument constructor, we pass the pool as *this
     * for: iterator begin(const stack_type head) { return iterator(*this, head); }
     */
    stack_pool_iterator(stack_pool_type &pool, stack_type head) : stack_pool{&pool}, current{head} {} //

    /**
     * no explicit or it cannot be used for implicit conversions and copy-initialization
     * if explicit constructor must be used: auto constructed_not_assigned{pool.begin(l1)};
     */
    stack_pool_iterator(const stack_pool_iterator &iterator) = default;

    /**
     * copy assignment operator needed for const iterator
     */
    stack_pool_iterator &operator=(const stack_pool_iterator &x) noexcept {
        if (current != x.current)
            current = x.current;
        return *this;
    }

    reference operator*() const { return stack_pool->value(current); }

    stack_pool_iterator &operator++() {  // pre-increment
        current = stack_pool->next(current);
        return *this;
    }

    stack_pool_iterator operator++(int) {  // post-increment
        stack_pool_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const stack_pool_iterator &x, const stack_pool_iterator &y) {
        return x.current == y.current;
    }

    friend bool operator!=(const stack_pool_iterator &x, const stack_pool_iterator &y) {
        return !(x == y);
    }

};

/**
 * A pool of blazingly fast stacks
 */
template<typename T, typename N = std::size_t>
class stack_pool {
    using value_type = T;
    using stack_type = N; // index/head of the specific stack in the pool

    struct node_t {
        value_type value;
        stack_type next;

        // needed by pool.emplace_back(std::forward<TT>(val), head);
        node_t() = default; //
        template<typename TT>
        node_t(TT &&val, stack_type next) : value{std::forward<TT>(val)}, next{next} {}; //
        ~node_t() = default;
    };

    std::vector<node_t> pool;
    stack_type free_nodes; // at the beginning, it is empty

    node_t &node(const stack_type x) noexcept { return pool[x - 1]; } // node(x).value node(x).next
    const node_t &node(const stack_type x) const noexcept { return pool[x - 1]; } // const { return node(x).next; }

public:
    using size_type = typename std::vector<node_t>::size_type;

    stack_pool() : pool{}, free_nodes{end()} {}; // init with zero capacity
    stack_pool(const stack_pool &) = default; // copy ctor
    stack_pool(stack_pool &&) noexcept = default; // move ctor
    ~stack_pool() noexcept = default;

    stack_pool &operator=(stack_pool &&) noexcept = default; // move assign.
    stack_pool &operator=(const stack_pool &) = default; // copy assign.

    explicit stack_pool(const size_type n) // custom ctor: reserves n nodes in the pool
            : pool{}, free_nodes{new_stack()} {
        reserve(n);
        free_nodes = new_stack();
    };

    using iterator = stack_pool_iterator<stack_pool<value_type, stack_type>, value_type, stack_type>;
    using const_iterator = stack_pool_iterator<const stack_pool<value_type, stack_type>, const value_type, stack_type>;

    iterator begin(stack_type x) { return iterator(*this, x); } //
    iterator end(stack_type) noexcept { return iterator(*this, end()); } // this is not a typo
    const_iterator begin(stack_type x) const { return const_iterator(*this, x); } //
    const_iterator end(stack_type) const noexcept { return const_iterator(*this, end()); } //
    const_iterator cbegin(stack_type x) const { return const_iterator(*this, x); } //
    const_iterator cend(stack_type) const noexcept { return const_iterator(*this, end()); } //

    stack_type new_stack() noexcept { return end(); } // return an empty stack
    void reserve(size_type n) { pool.reserve(n); } // reserve n nodes in the pool
    size_type capacity() const noexcept { return pool.capacity(); } // the capacity of the pool
    bool empty(stack_type x) const noexcept { return x == end(); } // check if stack is empty

    stack_type end() const noexcept { return stack_type(0); } // stack end is index 0 (trick)

    // get the last node of the stack
    stack_type last(const stack_type x) const noexcept {
        if (empty(x))
            return end();
        stack_type current = x;
        while (next(current) != end())
            ++current;
        return current;
    }

    value_type &value(stack_type x) {
        AP_ASSERT_SOFT(!empty(x), Stack_pool_exception) << "Can't access value in empty stack";
        return node(x).value;
    }

    const value_type &value(const stack_type x) const {
        AP_ASSERT_SOFT(!empty(x), Stack_pool_exception) << "Can't access value in empty stack";
        return node(x).value;
    }

    stack_type &next(stack_type x) { return node(x).next; } //
    const stack_type &next(stack_type x) const { return node(x).next; }

    stack_type push(const value_type &val, stack_type head) { return _push(val, head); } //
    stack_type push(value_type &&val, stack_type head) { return _push(std::move(val), head); }

    /**
     * POP removes the first node and this does not return the node but returns the new head
     */
    stack_type pop(stack_type x) {
        if (empty(x))
            return end();
        auto new_head = next(x);
        next(x) = free_nodes;
        free_nodes = x;
        return new_head;
    }

    /**
     * free_stack frees entire stack
     */
    stack_type free_stack(stack_type x) {
        // easy way
        // while (!empty(x)) x = this->pop(x);
        // New free_nodes head must become head of stack to free, appending free_nodes at the end
        if (empty(x))
            return end();
        if (!empty(free_nodes)) {
            stack_type last_node = last(x);
            next(last_node) = free_nodes;
        }
        free_nodes = x;
        x = end();
        return x;
    }

    // initializer must be stack_pool constructor, but we need a head
    // std::initializer_list has higher priority vs custom ctors (call them with round parenthesis)
    //explicit stack_pool(stack_type head, std::initializer_list<T> l) {
    //    for (auto&& x : l)
    //        push(std::move(x), head);
    //}

    void print_pool_int() {
        std::cout << "print pool[" << pool.capacity() << "]"
                  << " free_nodes=" << free_nodes
                  << " pool=[ ";
        for (auto i = end(); i < pool.capacity(); ++i)
            std::cout << static_cast<int>(pool[i].value) << "/" << pool[i].next << " ";
        std::cout << "]" << std::endl;
    }

    void print_pool_vector() {
        std::cout << "print pool[" << pool.capacity() << "]"
                  << " free_nodes=" << free_nodes
                  << " pool=[ ";
        std::cout << "not integer values ";
        for (auto i = end(); i < pool.capacity(); ++i)
            std::cout << "<>/" << pool[i].next << " ";
        std::cout << "]" << std::endl;
    }

    void print_freenodes_int() {
        std::cout << "free_nodes=" << free_nodes << " ";
        print_stack_int(free_nodes);
    }

    void print_freenodes_vector() {
        std::cout << "free_nodes=" << free_nodes << " ";
        print_stack_vector(free_nodes);
    }

    void print_stack_int(stack_type x) {
        std::cout << "print stack[" << x << "] = [ ";
        while (!empty(x)) {
            std::cout << value(x) << "/" << next(x) << " ";
            x = next(x);
        }
        std::cout << "]" << std::endl;
    }

    void print_stack_vector(stack_type x) {
        std::cout << "print stack[" << x << "] = [ ";
        std::cout << "not integer values ";
        while (!empty(x)) {
            std::cout << "<>/" << next(x) << " ";
            x = next(x);
        }
        std::cout << "]" << std::endl;
    }

private:
    /**
     * when we push the head changes, we can use references but since there are integers
     * and those integers can be smaller than 8 bytes (reference/pointer) and want to be fast
     * I prefer to pass the head by value and return the head
     * when you modify the head you have to return it like in:
     * l1 = pool.push(3, l1);
     */
    template<typename TT>
    stack_type _push(TT &&val, stack_type head) {
        if (head >= capacity()) {
            // ERROR no free space, we can increase the size, for example doubling it
            reserve(2 * capacity());
        }
        if (empty(free_nodes)) {
            pool.emplace_back(std::forward<TT>(val), head);
            return static_cast<stack_type>(pool.size());
        } else {
            stack_type new_head = free_nodes;
            free_nodes = next(free_nodes);
            value(new_head) = std::forward<TT>(val);
            // do we need to check or head already equal to end() when empty?
            next(new_head) = empty(head) ? end() : head;
            return new_head;
        }
    }

};

/**
 * Exception generated by asserts inside the code
 */
struct Stack_pool_exception {
    std::string message_string;

    explicit Stack_pool_exception(std::string s) : message_string{std::move(s)} {}

    const char *what() const { return message_string.c_str(); }
};
