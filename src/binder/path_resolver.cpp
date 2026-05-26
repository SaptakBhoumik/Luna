#include "binder/path_resolver.hpp"
#include <iostream>

namespace Luna {
std::filesystem::path get_abs_path(std::string relative_or_absolute_path,std::filesystem::path base_dir_of_file){
    std::filesystem::path p(relative_or_absolute_path);
    if(p.is_absolute()){
        return p;
    }
    else{
        return base_dir_of_file / p;
    }
}
std::vector<std::filesystem::path> get_abs_path(std::vector<std::string> relative_or_absolute_path,std::filesystem::path base_dir_of_file){
    std::vector<std::filesystem::path> abs_paths;
    for(const auto& path : relative_or_absolute_path){
        abs_paths.push_back(get_abs_path(path,base_dir_of_file));
    }
    return abs_paths;
}

std::filesystem::path get_abs_path(std::string relative_or_absolute_path){
    return get_abs_path(relative_or_absolute_path,std::filesystem::current_path());
}
std::vector<std::filesystem::path> get_abs_path(std::vector<std::string> relative_or_absolute_path){
    return get_abs_path(relative_or_absolute_path,std::filesystem::current_path());
}

PathResolver::PathResolver(std::filesystem::path buildin_module_path,std::vector<std::filesystem::path> include_paths){
    this->buildin_module_path = buildin_module_path;
    this->include_paths = include_paths;
}

std::optional<ResolvedPath> PathResolver::resolve(std::string relative_module_path,std::filesystem::path base_dir_of_file) const{
    std::filesystem::path p = std::filesystem::path(relative_module_path).relative_path();
    //Check if base_dir_of_file/p exists and is a file or directory with __init__.luna
    std::filesystem::path required_path = base_dir_of_file / p;
    if(!std::filesystem::exists(required_path)){
        for(const auto& include_path : include_paths){
            required_path = include_path / p;
            if(std::filesystem::exists(required_path)){
                break;
            }
        }
        if(!std::filesystem::exists(required_path)){
            required_path = buildin_module_path / p;
            if(!std::filesystem::exists(required_path)){
                return std::nullopt;
            }
        }
    }
    if(std::filesystem::is_directory(required_path)){
        if(std::filesystem::exists(required_path / "__init__.luna")){
            return ResolvedPath{required_path.lexically_normal(),PathType::DirWithInit};
        }
        else{
            return ResolvedPath{required_path.lexically_normal(),PathType::DirWithOutInit};
        }
    }
    else if(std::filesystem::is_regular_file(required_path)){
        return ResolvedPath{required_path.lexically_normal(),PathType::File};
    }
    else{
        std::cerr << "Error: Path " << required_path << " is neither a file nor a directory.This should not happen. Create an issue." << std::endl;
        exit(1);
    }
}
}