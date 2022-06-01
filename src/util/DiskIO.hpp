#pragma once


struct DiskIO {
    static std::ifstream openBinaryInput(const std::string &path) {
        auto res = std::ifstream(path, std::ios::in | std::ios::binary);
        if (!res.is_open())
            throw std::runtime_error("Could not open file " + path);
        return std::move(res);
    }

    template<typename T>
    static void read(std::ifstream &input, T &val) {
        input.read(reinterpret_cast<char *>(&val), std::streamsize(sizeof(T)));
    }

    template<typename T>
    static void readToVec(std::ifstream &input, std::vector<T> &vec) {
        T elem;
        while (input.read(reinterpret_cast<char *>(&elem), std::streamsize(sizeof(T))))
            vec.push_back(elem);
    }


    static std::ofstream openBinaryOutput(const std::string &path) {
        auto res = std::ofstream(path, std::ios::out | std::ios::binary);
        if (!res.is_open())
            throw std::runtime_error("Could not open file " + path);
        return std::move(res);
    }

    template<typename T>
    static void write(std::ofstream &output, const T &val) {
        output.write(reinterpret_cast<const char *>(&val), std::streamsize(sizeof(val)));
    }

    template<typename T>
    static void writeVec(std::ofstream &output, std::vector<T> &vec) {
        output.write(reinterpret_cast<const char *>(vec.data()), std::streamsize(sizeof(T) * vec.size()));
    }
};