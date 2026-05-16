cmake_minimum_required(VERSION 3.20)

project(CUDA_coursework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    set(CUDA_AVAILABLE TRUE)
else()
    set(CUDA_AVAILABLE FALSE)
endif()

if(CUDA_AVAILABLE)
    option(BUILD_CUDA "Build GPU solver" ON)
else()
    option(BUILD_CUDA "Build GPU solver" OFF)
    message(STATUS "CUDA compiler not found — building CPU-only")
endif()

add_executable(particle_sim
    src/main.cpp
    src/simulation/cpu_solver.cpp
    src/simulation/integrators.cpp
    src/simulation/simulation.cpp
    src/utils/timer.cpp
    src/utils/logger.cpp
    src/utils/export.cpp
    src/utils/particle_export.cpp
    src/benchmark/benchmark.cpp
)

target_include_directories(particle_sim PRIVATE ${CMAKE_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(particle_sim PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:/W4>
        $<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    target_compile_options(particle_sim PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall>
        $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
        $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>)
endif()

if(BUILD_CUDA)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)

    if(NOT CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES 75-real 86-real 89-real)
    endif()

    find_package(CUDAToolkit REQUIRED)

    target_sources(particle_sim PRIVATE
        src/cuda/memory.cpp
        src/cuda/kernels.cu
        src/simulation/gpu_solver.cu
    )

    target_compile_definitions(particle_sim PRIVATE PARTICLE_SIM_CUDA=1)
    target_link_libraries(particle_sim PRIVATE CUDA::cudart)

    set_target_properties(particle_sim PROPERTIES CUDA_ARCHITECTURES "${CMAKE_CUDA_ARCHITECTURES}")
endif()

# --- Sprint 6: Ray marching ---
option(BUILD_RAYMARCH "Build ray_march SDF renderer" ON)
option(RAYMARCH_GLFW "Enable OpenGL viewer for ray_march" ON)

if(BUILD_RAYMARCH)
    set(RAYMARCH_SOURCES
        src/main_raymarch.cpp
        src/rendering/sdf.cpp
        src/rendering/camera.cpp
        src/rendering/raymarch.cpp
        src/rendering/shading.cpp
        src/rendering/framebuffer.cpp
        src/rendering/renderer.cpp
        src/rendering/gl_viewer.cpp
        src/utils/timer.cpp
        src/utils/logger.cpp
    )

    add_executable(ray_march ${RAYMARCH_SOURCES})
    target_include_directories(ray_march PRIVATE ${CMAKE_SOURCE_DIR}/src)

    if(MSVC)
        target_compile_options(ray_march PRIVATE /W4 /permissive-)
    else()
        target_compile_options(ray_march PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(RAYMARCH_GLFW)
        include(FetchContent)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.3.9
        )
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(glfw)

        target_compile_definitions(ray_march PRIVATE RAYMARCH_USE_GLFW=1)
        target_link_libraries(ray_march PRIVATE glfw)

        if(WIN32)
            target_link_libraries(ray_march PRIVATE opengl32)
        elseif(APPLE)
            find_library(OpenGL_LIBRARY OpenGL)
            target_link_libraries(ray_march PRIVATE ${OpenGL_LIBRARY})
        else()
            find_package(OpenGL REQUIRED)
            target_link_libraries(ray_march PRIVATE OpenGL::GL)
        endif()
    endif()
endif()
