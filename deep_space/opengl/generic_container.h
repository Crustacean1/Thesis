#ifndef GENERIC_CONTAINER
#define GENERIC_CONTAINER

#include <vector>

template <typename T> class Container {
    std::vector<T> data;

  public:
    template <typename... C> unsigned int allocate_component(C... c) {
        unsigned int i = data.size();
        data.push_back(T{c...});
        return i;
    }
    T& get_component(unsigned int i) { return data[i]; }
};

template <typename... Attrs> class GenericContainer : private Container<Attrs>... {
  public:
    template <typename T, typename... C> unsigned int allocate(C... c) {
        return Container<T>::allocate_component(c...);
    }
    template <typename T> T &get(unsigned int i) { return Container<T>::get_component(i); }
};

#endif /*GENERIC_CONTAINER*/
