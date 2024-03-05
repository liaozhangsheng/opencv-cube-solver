#ifndef __PRUNETABLE_HELPERS_H__
#define __PRUNETABLE_HELPERS_H__

#include <iostream>
#include <fstream>
#include <filesystem>

bool read_cache(const std::filesystem::path &cache_dir, const std::filesystem::path &name, void *ptr, std::size_t len);
void write_cache(const std::filesystem::path &cache_dir, const std::filesystem::path &name, void *ptr, std::size_t len);

#endif
