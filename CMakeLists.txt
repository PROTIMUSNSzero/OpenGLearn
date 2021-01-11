cmake_minimum_required(VERSION 3.15)
project(OpenGLearn)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR ./head&lib/include)
set(LINK_DIR ./head&lib/lib)

include_directories(openGLearn)

include_directories(${INC_DIR})
include_directories(${LINK_DIR})

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
        openGLearn/Model.cpp
        openGLearn/Model.h
        openGLearn/resource.h
        openGLearn/stb_image.cpp
        openGLearn/stb_image.h)

target_link_libraries(OpenGLearn
        glfw3.lib
        glfw3dll.lib
        assimp-vc140-mt.lib)