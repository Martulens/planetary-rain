#pragma once

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

// OpenGL / GLEW
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "GL/glext.h"
#include <GL/freeglut.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Standard library
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

// Macros

#define CHECK_GL_ERROR() framework::checkGLError(__FUNCTION__, __LINE__)

// \todo REVIEW avoid macros for constants. Use consexpr instead.

#ifndef M_PI
/// Pi constant for compatibility (MSVC vs GCC)
#define M_PI 3.14159265358979323846
#endif

// \todo for functional macros, define static utility functions.
//  The compiler would use inline optimization if it considers necessary, which would result in the same effect as using macros.
// Overall using macros can lead to non-descriptive compiler errors and is more of a C-style way. Prefer modern C++ practices.
#ifndef DEGTORAD
/// Degrees to radians conversion macro
#define DEGTORAD(a) ((a) * M_PI / 180.0)
#endif

#ifndef RADTODEG
/// Radians to degrees conversion macro
#define RADTODEG(a) ((a) * 180.0 / M_PI)
#endif

namespace framework
{
    // Constants

    /** @brief Required OpenGL version - major number */
    const int OGL_VER_MAJOR = 3;
    /** @brief Required OpenGL version - minor number */
    const int OGL_VER_MINOR = 3;

    /** @brief Debug level enumeration for OpenGL debug callbacks */
    enum DebugLevel
    {
        DEBUG_OFF = 0, ///< Turn off debug callbacks
        DEBUG_LOW, ///< Enable messages with high severity only
        DEBUG_MEDIUM, ///< Enable messages with high and medium severities
        DEBUG_HIGH ///< Enable messages with all severities
    };

    // MeshData Structure
    /**
     * @struct MeshData
     * @brief Structure to hold built-in mesh data (like teapot, cube, etc.)
     */
    struct MeshData
    {
        unsigned nVertices; ///< Number of vertices/normals/texCoords
        unsigned nTriangles; ///< Number of triangles
        unsigned nAttribsPerVertex; ///< Attributes per vertex (typically 8: 3 pos, 3 norm, 2 tex)

        const float* verticesInterleaved; ///< Vertex data: 3 pos, 3 normal, 2 texcoord per vertex
        const unsigned* triangles; ///< Triangle indices (3 per triangle)
    };

    // Initialization Functions

    /**
     * @brief Initializes GLEW and checks OpenGL version.
     *
     * Call this function after successful creation of OpenGL context
     * (e.g., after glutCreateWindow).
     *
     * @param glVerMajor OpenGL major version to check (returns false if not available)
     * @param glVerMinor OpenGL minor version to check (returns false if not available)
     * @param debugLevel If not DEBUG_OFF, initialize OpenGL debug callback (requires debug context)
     * @return true if initialization succeeded and OpenGL version is sufficient, false otherwise
     */
    bool initialize(int glVerMajor = OGL_VER_MAJOR, int glVerMinor = OGL_VER_MINOR,
                    DebugLevel debugLevel = DEBUG_OFF);

    /**
     * @brief Returns the value of framework_FRAMEWORK_ROOT environment variable.
     * @return Value of the environment variable, or empty string if not set.
     */
    std::string frameworkRoot();

    /**
     * @brief Prints an error message and terminates the application.
     * @param errMsg Error message to display.
     */
    void dieWithError(const std::string& errMsg);

    /**
     * @brief Checks for OpenGL errors and prints them.
     * @param where Function or location name (can be NULL)
     * @param line Line number
     */
    void checkGLError(const char* where = nullptr, int line = 0);

    // Shader Functions

    /**
     * @brief Creates and compiles a shader from source code.
     *
     * Internally uses glCreateShader, glShaderSource, and glCompileShader.
     * Prints GLSL errors and warnings to stderr.
     *
     * @param eShaderType Shader type: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, or GL_GEOMETRY_SHADER
     * @param strShaderText GLSL source code
     * @return OpenGL shader ID, or 0 on failure
     */
    GLuint createShaderFromSource(GLenum eShaderType, const std::string& strShaderText);

    /**
     * @brief Creates and compiles a shader from a file.
     *
     * Loads shader source from the specified file and calls createShaderFromSource().
     *
     * @param eShaderType Shader type: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, or GL_GEOMETRY_SHADER
     * @param filename Path to the shader source file
     * @return OpenGL shader ID, or 0 on failure
     */
    GLuint createShaderFromFile(GLenum eShaderType, const std::string& filename);

    /**
     * @brief Links shaders into a program.
     *
     * Internally uses glCreateProgram, glAttachShader, and glLinkProgram.
     * Prints GLSL errors and warnings to stderr.
     *
     * @param shaderList Vector of compiled shader IDs
     * @return OpenGL program ID, or 0 on failure
     */
    GLuint createProgram(const std::vector<GLuint>& shaderList);

    /**
     * @brief Links shaders into a program (overloaded version).
     *
     * @param shaders Zero-terminated array of shader IDs (e.g., {vsid, fsid, 0})
     * @return OpenGL program ID, or 0 on failure
     */
    GLuint createProgram(const GLuint* shaders);

    /**
     * @brief Deletes a shader program and all attached shaders.
     * @param program Program ID to delete
     */
    void deleteProgramAndShaders(GLuint program);

    // Texture Functions

    /**
     * @brief Creates a texture from an image file.
     *
     * Generates a 2D texture, loads the image using stb_image, sets filtering,
     * and optionally creates mipmaps.
     *
     * @param fileName Path to the image file
     * @param mipmap Enable and generate mipmaps for this texture (default: true)
     * @return OpenGL texture ID, or 0 on failure
     */
    GLuint createTexture(const std::string& fileName, bool mipmap = true);

    /**
     * @brief Loads texture data from file and uploads to currently bound texture.
     *
     * Calls glTexImage2D with the loaded image data.
     *
     * @param fileName Path to the image file
     * @param target Texture target (e.g., GL_TEXTURE_2D)
     * @return true if texture was successfully loaded, false otherwise
     */
    bool loadTexImage2D(const std::string& fileName, GLenum target);

    namespace gldebug
    {
        /**
         * @brief Returns human-readable string for debug source.
         * @param src Debug source enum value
         * @return String representation of the source
         */
        const char* sourceToString(GLenum src);

        /**
         * @brief Returns human-readable string for debug type.
         * @param type Debug type enum value
         * @return String representation of the type
         */
        const char* typeToString(GLenum type);

        /**
         * @brief Returns human-readable string for debug severity.
         * @param severity Debug severity enum value
         * @return String representation of the severity
         */
        const char* severityToString(GLenum severity);

        /**
         * @brief Prints backtrace of the current call stack.
         * @param os Output stream (defaults to std::cout)
         */
        void printBacktrace(std::ostream* os = nullptr);
    } // namespace gldebug
} // namespace framework

#endif