#include "./resource_manager.h"
#include "../runtime_exception.h"
#include <algorithm>
#include <concepts>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <vector>

namespace fs = std::filesystem;

std::pair<unsigned int, char *> read_file(const char *filename);
std::pair<std::vector<std::string>, std::function<std::vector<unsigned char>(const std::string &)>>
create_loader(const std::string &filepath) {
    auto path = fs::current_path() / "shaders" / filepath;
    if (fs::exists(path)) {
        auto filenames_it =
            fs::directory_iterator(path) | std::views::transform([](auto entry) -> std::string {
                return entry.path().filename();
            });

        std::vector<std::string> filenames;
        for (auto dir : filenames_it) {
            std::cout << "Filename: " << dir << std::endl;
            filenames.push_back(dir);
        }

        return std::pair(filenames, [path](auto file) {
            auto [size, data] = read_file((path / file).c_str());
            return std::vector<unsigned char>(data, data + size + 1);
        });
    } else {
        return std::pair(std::vector<std::string>{},
                         [](auto a) { return std::vector<unsigned char>{}; });
    }
}
