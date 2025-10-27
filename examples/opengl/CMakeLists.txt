add_executable(opengl main.c)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(opengl glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(opengl OpenGL::GL)
find_package(GLEW)
target_link_libraries(opengl GLEW::GLEW)

target_link_libraries(opengl m)
target_link_libraries(opengl example-utils)
target_link_libraries(opengl rol-font-renderer)
