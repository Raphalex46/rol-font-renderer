add_executable(gnu-unifont main.c)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(gnu-unifont glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(gnu-unifont OpenGL::GL)
find_package(GLEW)
target_link_libraries(gnu-unifont GLEW::GLEW)

target_link_libraries(gnu-unifont m)
target_link_libraries(gnu-unifont example-utils)
