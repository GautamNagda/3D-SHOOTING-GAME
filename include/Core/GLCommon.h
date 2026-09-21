#pragma once

#ifdef __EMSCRIPTEN__
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif
