cmake_minimum_required(VERSION 3.6)

file(GLOB_RECURSE SHADERS shaders/*.glsl)
file(GLOB_RECURSE CPP_SOURCE src/*.cpp)
file(GLOB_RECURSE CPP_HEADER src/*.h)

include_directories(include)
if(${MSVC14})
link_directories(lib/v140/${CMAKE_BUILD_TYPE})
endif()
if(${APPLE})
link_directories(lib/clang/)
endif()

add_executable(ParticleTracking ParticleTracking.cpp ${CPP_SOURCE} ${CPP_HEADER} ${SHADERS})

source_group("Shaders" FILES ${SHADERS})

if(${WIN32})
target_link_libraries(ParticleTracking debug opengl32.lib debug glew32.lib debug glfw3.lib debug FreeImage.lib)
target_link_libraries(ParticleTracking optimized opengl32.lib optimized glew32.lib optimized glfw3.lib optimized FreeImage.lib)
endif()
if(${APPLE})
target_link_libraries(ParticleTracking libglew.a libfreeimage.a libglfw.3.1.dylib ${OPENGL_gl_LIBRARY})
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_custom_command(TARGET ParticleTracking PRE_BUILD COMMAND copy /Y \"${CMAKE_CURRENT_SOURCE_DIR}/shaders\" \"${CMAKE_CURRENT_BINARY_DIR}/shaders\")

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(${MSVC14})
file(GLOB DLLS_RELEASE lib/v140/Release/*.dll)
file(COPY ${DLLS_RELEASE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB DLLS_DEBUG lib/v140/Debug/*.dll)
file(COPY ${DLLS_DEBUG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
if(${APPLE})
file(GLOB DYLIB lib/clang/*.dylib)
file(COPY ${DYLIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB STATICA lib/clang/*.a)
file(COPY ${STATICA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()