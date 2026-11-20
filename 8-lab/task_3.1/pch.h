#pragma once

#define _USE_MATH_DEFINES
//#include <gl/glew.h>
// GLEW must be include before GL/gl.h
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <chrono>
#include <cmath>
#include <mutex>
#include <optional>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include <stdexcept>