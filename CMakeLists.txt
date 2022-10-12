cmake_minimum_required(VERSION 3.0.2)
project(tue_filesystem)

add_compile_options(-Wall -Werror=all)
add_compile_options(-Wextra -Werror=extra)

find_package(catkin REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES tue_filesystem
  DEPENDS Boost
)

include_directories(
    include
    SYSTEM
    ${Boost_INCLUDE_DIRS}
)

# to show header files in Qt Creator
file(GLOB_RECURSE HEADER_FILES include/*.h)

## Declare a cpp library
add_library(tue_filesystem
    src/path.cpp
    src/crawler.cpp
    ${HEADER_FILES}
)

target_compile_definitions(tue_filesystem PRIVATE BOOST_FILESYSTEM_NO_DEPRECATED)
target_link_libraries(tue_filesystem ${Boost_LIBRARIES})

# Declare test
add_executable(test_crawler test/test_crawler.cpp)
target_link_libraries(test_crawler tue_filesystem)

if (CATKIN_ENABLE_TESTING)
  find_package(catkin_lint_cmake REQUIRED)
  catkin_add_catkin_lint_test("-W2")
endif()
