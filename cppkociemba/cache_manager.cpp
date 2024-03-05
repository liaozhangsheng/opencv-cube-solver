#include "include/cache_manager.h"

bool read_cache(const std::filesystem::path &cache_dir, const std::filesystem::path &name, void *ptr, std::size_t len)
{
    std::filesystem::path file_path = cache_dir / name;
    if (std::filesystem::exists(file_path) && std::filesystem::is_regular_file(file_path))
    {
        std::ifstream file(file_path, std::ios::binary);
        file.read(static_cast<char *>(ptr), len);
        if (file.fail())
        {
            std::cerr << "Failed to read cache table " << file_path << ".\n";
            return false;
        }
        return true;
    }
    else
    {
        std::cerr << "Cache table " << file_path << " was not found. Recalculating.\n";
        return false;
    }
}

void write_cache(const std::filesystem::path &cache_dir, const std::filesystem::path &name, void *ptr, std::size_t len)
{
    try
    {
        if(!std::filesystem::exists(cache_dir))
        {
            std::filesystem::create_directory(cache_dir);
        }
    }
    catch(const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Failed to create cache directory " << cache_dir << ": " << e.what() << '\n';
        return;
    }

    std::filesystem::path file_path = cache_dir / name;
    std::ofstream file(file_path, std::ios::binary);
    file.write(static_cast<const char *>(ptr), len);
    if (file.fail())
    {
        std::cerr << "Failed to write cache table " << file_path << ".\n";
    }
}
