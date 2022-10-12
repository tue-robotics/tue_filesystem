cmake_minimum_required(VERSION 3.0.2)
project(tue_filesystem)

add_compile_options(-Wall -Werror=all)
add_compile_options(-Wextra -Werror=extra)

find_package(catkin REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
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
add_library(${PROJECT_NAME}
    ${HEADER_FILES}
    src/crawler.cpp
    src/path.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_FILESYSTEM_NO_DEPRECATED)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# Declare test
add_executable(test_crawler test/test_crawler.cpp)
target_link_libraries(test_crawler ${PROJECT_NAME})

install(
  DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
)

install(
  TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

if (CATKIN_ENABLE_TESTING)
  find_package(catkin_lint_cmake REQUIRED)
  catkin_add_catkin_lint_test("-W2 --ignore HEADER_OUTSIDE_PACKAGE_INCLUDE_PATH")
endif()
