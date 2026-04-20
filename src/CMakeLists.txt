cmake_minimum_required(VERSION 3.16)
project(PlanetaryRain LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Policy for FetchContent
if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif ()

include(FetchContent)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Fetch Dependencies
message(STATUS "Fetching dependencies...")

# --- GLM (header-only) ---
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)
message(STATUS "GLM fetched")

# --- GLEW ---
FetchContent_Declare(
        glew
        URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
)
FetchContent_GetProperties(glew)
if (NOT glew_POPULATED)
    FetchContent_Populate(glew)

    # GLEW doesn't have a proper CMakeLists.txt in the root, so we build it manually
    add_library(glew STATIC
            ${glew_SOURCE_DIR}/src/glew.c
    )
    target_include_directories(glew PUBLIC ${glew_SOURCE_DIR}/include)
    target_compile_definitions(glew PUBLIC GLEW_STATIC)
    target_link_libraries(glew PUBLIC ${OPENGL_LIBRARIES})

    if (WIN32)
        target_link_libraries(glew PUBLIC opengl32)
    endif ()
endif ()
message(STATUS "GLEW fetched")

# --- FreeGLUT ---
if (UNIX AND NOT APPLE)
    find_package(GLUT REQUIRED)
    set(FREEGLUT_LIBRARIES ${GLUT_LIBRARIES})
    set(FREEGLUT_INCLUDE_DIRS ${GLUT_INCLUDE_DIR})
    set(USE_SYSTEM_GLUT TRUE)
    message(STATUS "Using system FreeGLUT")
else ()
    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_PRINT_ERRORS ON CACHE BOOL "" FORCE)
    set(FREEGLUT_PRINT_WARNINGS ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
            freeglut
            GIT_REPOSITORY https://github.com/FreeGLUTProject/freeglut.git
            GIT_TAG v3.2.2
    )
    FetchContent_MakeAvailable(freeglut)
    set(USE_SYSTEM_GLUT FALSE)
    message(STATUS "FreeGLUT fetched")
endif ()

# --- Assimp ---
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.3.1
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(assimp)
message(STATUS "Assimp fetched")

# --- stb_image (header-only, pinned commit for reproducibility) ---
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        31c1ad37456438565541f4919958214b6e762fb4 # stb_image v2.30
)
FetchContent_MakeAvailable(stb)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})
message(STATUS "stb_image fetched")

# --- imgui ---
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.90.1
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

# Create ImGui library with GLUT backend
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glut.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC ${OPENGL_LIBRARIES} glew)

# Source Files
set(SOURCES
        main.cpp
        framework.cpp
        callbacks.cpp
        game.cpp
        utilities.cpp
        camera.cpp
        draw.cpp
        debug.cpp
        object.cpp
        shaderProgram.cpp
        meshGeometry.cpp
        modelTexture.cpp
        sphere.cpp
        light.cpp
        scene.cpp
        skybox.cpp
        envMap.cpp
        ui.cpp
        variables.cpp
)

set(HEADERS
        framework.h
        config.h
        callbacks.h
        game.h
        gamestate.h
        draw.h
        camera.h
        debug.h
        object.h
        shaderProgram.h
        light.h
        meshGeometry.h
        modelTexture.h
        sphere.h
        utilities.h
        skybox.h
        envMap.h
        ui.h
        noise.h
        variables.h
)

set(SHADERS
        shaders/template.vert
        shaders/template.frag
        shaders/planet.vert
        shaders/planet.frag
)

# Create Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SHADERS})

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${glew_SOURCE_DIR}/include
        ${assimp_SOURCE_DIR}/include
        ${assimp_BINARY_DIR}/include
        ${stb_SOURCE_DIR}
)

if (USE_SYSTEM_GLUT)
    target_include_directories(${PROJECT_NAME} PRIVATE ${FREEGLUT_INCLUDE_DIRS})
else ()
    target_include_directories(${PROJECT_NAME} PRIVATE ${freeglut_SOURCE_DIR}/include)
endif ()

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OPENGL_LIBRARIES}
        glew
        glm::glm
        assimp
        imgui
)

if (USE_SYSTEM_GLUT)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${FREEGLUT_LIBRARIES})
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE freeglut_static)
endif ()

# Linux-specific libraries
if (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            GL
            GLU
            m
            pthread
            dl
            X11
            Xrandr
            Xi
    )
endif ()

# Compile Definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

if (NOT USE_SYSTEM_GLUT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE FREEGLUT_STATIC)
endif ()

# For development: load shaders from source directory
target_compile_definitions(${PROJECT_NAME} PRIVATE
        SHADER_SOURCE_DIR="${CMAKE_SOURCE_DIR}/shaders/"
)

# =============================================================================
# SHADER AUTO-COPY (runs on every build, not just when source files change)
# =============================================================================
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMENT "Copying shaders to build directory"
)

# =============================================================================
# Copy Resources to Build Directory
# =============================================================================
if (EXISTS "${CMAKE_SOURCE_DIR}/textures")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/textures"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures"
            COMMENT "Copying textures to build directory"
    )
endif ()

if (EXISTS "${CMAKE_SOURCE_DIR}/models")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/models"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models"
            COMMENT "Copying models to build directory"
    )
endif ()

# IDE Organization
source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Shaders" FILES ${SHADERS})

# Compiler Settings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            NOMINMAX
    )
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif ()

# Print Configuration Summary
message(STATUS "")
message(STATUS "=== Planetary Rain Configuration ===")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Dependencies:")
message(STATUS "  GLM:      ${glm_SOURCE_DIR}")
message(STATUS "  GLEW:     ${glew_SOURCE_DIR}")
if (USE_SYSTEM_GLUT)
    message(STATUS "  FreeGLUT: System (${FREEGLUT_LIBRARIES})")
else ()
    message(STATUS "  FreeGLUT: ${freeglut_SOURCE_DIR}")
endif ()
message(STATUS "  Assimp:   ${assimp_SOURCE_DIR}")
message(STATUS "  stb:      ${stb_SOURCE_DIR}")
message(STATUS "  imgui:    ${imgui_SOURCE_DIR}")

message(STATUS "")