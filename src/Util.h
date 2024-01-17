#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <sstream>

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

std::string read_string_from_file(const std::string &file_path) {
    const std::ifstream input_stream(file_path, std::ios_base::binary);

    if (input_stream.fail()) {
        throw std::runtime_error("Failed to open file");
    }

    std::stringstream buffer;
    buffer << input_stream.rdbuf();

    return buffer.str();
}

#endif//UTIL_H
