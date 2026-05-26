#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace Luna {
enum class PathType:std::int8_t{
    DirWithInit,//If the path is a directory and has a __init__.luna file in it
    //When a file has a __init__.luna file then whatever import is exposed as public in that file is also exposed when the directory is imported. 
    //That is ``pub A := import("A")`` means A is exposed but ``B := import("B")`` means B is not exposed
    //If no __init__.luna file is present then the directory is treated as a normal directory and every subdirectory and file is exposed via DIR::SUBFILE
    DirWithOutInit,//If the path is a directory and does not have a __init__.luna file in it
    File//If the path is a file
};
struct ResolvedPath{
    //If a director and file both have same name and we import that name then if the directory has __init__.luna then we import the directory
    //otherwise we import the file.
    std::filesystem::path path;
    PathType type;
};

//base_dir_of_file must be absolute path to the directory from where we want this
std::filesystem::path get_abs_path(std::string relative_or_absolute_path,std::filesystem::path base_dir_of_file);
std::vector<std::filesystem::path> get_abs_path(std::vector<std::string> relative_or_absolute_path,std::filesystem::path base_dir_of_file);
//Uses the base directory from where we run the program as the base directory for resolving the path
std::filesystem::path get_abs_path(std::string relative_or_absolute_path);
std::vector<std::filesystem::path> get_abs_path(std::vector<std::string> relative_or_absolute_path);
class PathResolver{
    //Expects buildin_module_path to be a directory and for it to be absolute path
    //include_paths expects to be absolute path and can be directory or file. 
    std::filesystem::path buildin_module_path;//Path to the folder that has all the built in modules.
    //TODO:Determine how to find the buildin_module_path because it can be in local or usr or something else entirely. + Env varable also
    std::vector<std::filesystem::path> include_paths;//List of paths to search for when resolving an import. The order of the paths matter. The first path that has the module is used.
                                           //This is for if someone does -I path/to/some/dir then that path is added to the include_paths 
                                           //Searched before buildin_module_path because we want to allow users to override build in modules.
public:
    PathResolver(std::filesystem::path buildin_module_path,std::vector<std::filesystem::path> include_paths);

    std::optional<ResolvedPath> resolve(std::string relative_module_path,std::filesystem::path base_dir_of_file) const;
    //base_dir_of_file is the absolute path to the directory of the path that imported this path
    //base_dir_of_file is searched first before the include_paths and buildin_module_path because we want to allow relative imports 
};
}