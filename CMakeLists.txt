cmake_minimum_required(VERSION 3.15)
project(lroc)

# Add the library as a subdirectory
#add_subdirectory(src/flecs)

# Define your target
add_executable(lroc src/main.c)

# Link the library to your target
#target_link_libraries(lroc PRIVATE flecs)
