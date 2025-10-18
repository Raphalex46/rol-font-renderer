add_executable(gnu-unifont main.c)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(gnu-unifont glfw)
target_link_libraries(gnu-unifont OpenGL::GL)
