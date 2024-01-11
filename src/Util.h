#ifndef UTIL_H
#define UTIL_H

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename ContainerT, typename RangeT>
ContainerT to(RangeT &&range) {
    return ContainerT(begin(range), end(range));
}
#endif//UTIL_H
