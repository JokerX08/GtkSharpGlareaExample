cmake_minimum_required(VERSION 3.10)  # 指定CMake的最低版本要�? 
  
# 设置项目名称和版�? 
project(GLRender VERSION 1.0)  
  
# 添加源文件到项目�? 
add_library(GLRender SHARED main.cpp)  # SHARED表示生成动态库  
  
# 设置库的输出名称（可选）  
set_target_properties(GLRender PROPERTIES OUTPUT_NAME "GLRender")  
  
# 其他配置，如包含目录、链接库等（如果需要）  
# include_directories(...)  
# target_link_libraries(GLRender ...)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/include
)

target_link_directories(GLRender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/lib/Release/x64)
find_library(MYLIB_LIBRARY NAMES glew32 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/lib/Release/x64 NO_DEFAULT_PATH) 
 
# ����Ƿ��ҵ��˿�  
if(MYLIB_LIBRARY)  
  message(STATUS "Found glew32: ${MYLIB_LIBRARY}")  
else()  
  message(FATAL_ERROR "glew32 library not found")  
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


set(GLEW
    glew32
    opengl32
)

target_link_libraries(GLRender ${GLEW})

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../GTKTemplate/bin/Debug/net7.0-windows7.0)
#install(TARGETS test DESTINATION bin)

message("GLEW: ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/lib/Release/x64")

