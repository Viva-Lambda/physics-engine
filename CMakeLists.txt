cmake_minimum_required (VERSION 3.0.2)
project("physics-engine")

find_package(OpenGL REQUIRED)

message(STATUS "Found GL in ${OPENGL_LIBRARY}")

find_package (Threads REQUIRED)

set (CMAKE_BUILD_TYPE "Debug")
set (CMAKE_EXPORT_COMPILE_COMMANDS true)
set (CMAKE_CXX_FLAGS "-std=c++17")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -ggdb -Wall -Wextra -ldl")

include_directories(
    "./include/"
)

# for importing modules
set(AbsPathPrefix 
    "/media/kaan/Data7510/GitProjects")

# glfw
# find_package(glfw3 REQUIRED)
add_library(glfwLib SHARED IMPORTED)
set_target_properties(glfwLib PROPERTIES IMPORTED_LOCATION
    "${AbsPathPrefix}/glfw/bin/lib/libglfw.so")
include_directories("${AbsPathPrefix}/glfw/bin/include/")

# glut
#add_library(glutLib SHARED IMPORTED)
#set_target_properties(glutLib PROPERTIES IMPORTED_LOCATION
#    "${AbsPathPrefix}/FreeGLUT/freeglut/freeglut/lib/libglut.so.3")
#include_directories("${AbsPathPrefix}/FreeGLUT/freeglut/freeglut/include/")

#set(AssimpSOPath
#    "/media/kaan/Data7510/GitProjects/assimp/build/bin/libassimp.so")

#add_library(
#    AssimpLib 
#    SHARED
#    IMPORTED
#)
#set_property(
#   TARGET 
#   AssimpLib 
#   PROPERTY IMPORTED_LOCATION 
#   ${AssimpSOPath}
#   )

add_executable(main.out "src/main.cpp")

add_executable(
    demoAppTest.out 
    "src/demos/glad.c"
    "src/demos/demowindow.cpp"
    )
target_link_libraries(demoAppTest.out glfwLib)

# new application interface demo

add_executable(
    newdemo.out 
    "src/demos/glad.c"
    "src/demos/newapp.cpp"
    )

#target_link_libraries(final.out AssimpLib)
target_link_libraries(newdemo.out glfwLib)

# new engine architecture test

add_executable(
    newarch.out 
    "src/demos/glad.c"
    "src/demos/demowindow2.cpp"
)

#target_link_libraries(final.out AssimpLib)
target_link_libraries(newarch.out glfwLib)




add_executable(
    MeshDemoAppTest.out 
    "src/demos/glad.c"
    "src/demos/meshdemowindow.cpp"
    )
#target_link_libraries(final.out AssimpLib)

target_link_libraries(MeshDemoAppTest.out glfwLib)

# ballistics demo
add_executable(
    BallisticDemo.out 
    "src/demos/glad.c"
    "src/demos/ballisticdemo.cpp"
    )
#target_link_libraries(final.out AssimpLib)


target_link_libraries(BallisticDemo.out glfwLib)

# platform demo
add_executable(
    PlatformDemo.out 
    "src/demos/glad.c"
    "src/demos/platform.cpp"
    )
target_link_libraries(PlatformDemo.out glfwLib)

install(TARGETS main.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS demoAppTest.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS newdemo.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS MeshDemoAppTest.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS BallisticDemo.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
install(TARGETS PlatformDemo.out DESTINATION "${PROJECT_SOURCE_DIR}/bin/")
