#include <string>
#include <iostream>
#include <filesystem>

const auto default_source_files_extentions = std::vector<std::string> {
    ".h", ".hpp", ".c", ".cpp"
};

auto trim_begin(const std::string& value, std::size_t length) {
    std::string result = value;
    result.erase(0, length);
    return result;
}

auto fix_slash(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    return result;
}

bool ends_with(const std::string& value, const std::string& ending) {
    if (ending.size() > value.size()) { return false; }
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

auto is_source_file(const std::string& path) {
    return std::any_of(default_source_files_extentions.begin()
                     , default_source_files_extentions.end()
    , [&path](const std::string& ext) {
        return ends_with(path, ext);
    });
}

int main(int argc, char *argv[])
{
    std::cout << "Source files extentions: ";
    for (const auto& ext : default_source_files_extentions) {
        std::cout << ext << ' ';
    }
    std::cout << '\n' << '\n';

    if (argc > 2) {
        std::cerr << "Wrong number of arguments - need 0 or 1\n";
        return 1;
    }

    const auto target_directory_path = [&]() {
        if (1 == argc) {
            return std::filesystem::current_path();
        }
        return std::filesystem::path(argv[1]);
    }();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(target_directory_path)) {
        const auto entry_absolute_path = entry.path().string();
        const auto entry_relative_path = trim_begin(entry_absolute_path, target_directory_path.string().length() - target_directory_path.filename().string().length());
        const auto entry_fixed_relative_path = fix_slash(entry_relative_path);
        if (is_source_file(entry_fixed_relative_path)) {
            std::cout << "${CMAKE_CURRENT_SOURCE_DIR}/" << entry_fixed_relative_path << '\n';
        }
    }

    return 0;
}