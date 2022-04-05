cmake_minimum_required(VERSION 3.15)
project(OpenGLearn)

set(CMAKE_CXX_STANDARD 14)

#config glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(3rd_party/glfw)
include_directories(3rd_party/glfw/include)
LIST(APPEND LIBS glfw ${GLFW_LIBRARIES})

#config glad
set(GLAD_SRC 3rd_party/glad/src/glad.c)
if(NOT WIN32)
    LIST(APPEND LIBS dl)
endif()
include_directories(3rd_party/glad/include)

#config glm
include_directories(3rd_party/glm/glm)

#config assimp
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(3rd_party/assimp)
include_directories(3rd_party/assimp/include/)

set(SOURCE_FILES openGLearn/main.cpp ${GLAD_SRC})
add_executable(openGLearn ${SOURCE_FILES})
target_link_libraries(openGLearn ${LIBS})

include_directories(openGLearn)

add_executable(OpenGLearn
        openGLearn/C2_UniformFieldAndDataStruct.cpp
        openGLearn/Camera.cpp
        openGLearn/Camera.h
        openGLearn/CustomShader.cpp
        openGLearn/CustomShader.h
        openGLearn/glad.c
        openGLearn/Main.cpp
        openGLearn/Mesh.cpp
        openGLearn/Mesh.h
        openGLearn/resource.h
        openGLearn/stb_image.cpp
        openGLearn/stb_image.h)

target_link_libraries(openGLearn
        assimp)