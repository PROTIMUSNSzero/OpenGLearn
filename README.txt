
CMake配置生成项目时平台要统一（32位）


VS
编译生成的.dll文件放在该项目下，.lib文件放在库目录并连接对应目录与文件，.h文件放在include目录并连接目录


Clion
设置CMakeList
        set(INC_DIR ./head&lib/include)  include_directories(${INC_DIR})  //include及其子目录路径
        target_link_libraries(OpenGLearn glfw3.lib glfw3dll.lib assimp-vc140-mt.lib)  lib连接

在Edit Configuration->Environment variables中设置包含dll的目录(Path=[全局路径])