#ifndef __SORTED_HEAP_HPP_INCLUDED__
#define __SORTED_HEAP_HPP_INCLUDED__

#include <queue>

#include "common.hpp"

// PriorityQueue based sorted heap and auxiliary machinery to
// convert key-sorted aggregation maps to a value-sorted container.

namespace linkedin {

struct kv_heap_compare {
    template<class T>
    bool operator()(const T& a, const T& b) {
        return a.second < b.second;
    }
};

template<class T, class U>
using kv_heap = std::priority_queue<
    std::pair<T, U>,
    std::vector< std::pair<T, U> >,
    kv_heap_compare
>;

template<class Container>
struct push_iterator:
    std::iterator<std::output_iterator_tag, void, void, void, void>
{
    typedef Container container_type;

public:
    explicit push_iterator(Container& container):
        m_container(container)
    { }

    push_iterator& operator = (typename Container::const_reference value) {
        m_container.push(value);
        return *this;
    }

    push_iterator& operator * () {
        return *this;
    }

    push_iterator& operator ++ () {
        return *this;
    }

    push_iterator operator ++ (int) {
        return *this;
    }

private:
    Container& m_container;
};

template<class Container>
push_iterator<Container> pusher(Container& container) {
    return push_iterator<Container>(container);
}

}

#endif
