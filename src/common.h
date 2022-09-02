#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <string>
#include <optional>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>

// '\' --> line spaceing 
#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>;\
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;
/*
#define above is same as below.

CLASS_PTR(Shader);
class Shader;
using ShaderUPtr= std::unique_ptr<Shader>; --> new type: ShaderUPtr
using ShaderPtr = std::shared_ptr<Shader>; --> new type: ShaderPtr
using ShaderWPtr = std::weak_ptr<Shader>;  --> new type: ShaderWPtr
*/

std::optional<std::string> LoadTextFile(const std::string& filename);

#endif // __COMMON_H__