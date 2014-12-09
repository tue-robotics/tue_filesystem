cmake_minimum_required(VERSION 2.8.3)
project(tue_filesystem)

## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system filesystem)

###################################
## catkin specific configuration ##
###################################
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES tue_filesystem
  DEPENDS system filesystem
)

###########
## Build ##
###########

include_directories(
    include
)

# to show header files in Qt Creator
file(GLOB_RECURSE HEADER_FILES include/*.h)

## Declare a cpp library
add_library(tue_filesystem
    src/path.cpp
    src/crawler.cpp
    ${HEADER_FILES}
)

# Specify libraries to link a library or executable target against
target_link_libraries(tue_filesystem
   ${Boost_LIBRARIES}
)

# Declare test
add_executable(test_crawler test/test_crawler.cpp)

# Specify libraries to link a library or executable target against
target_link_libraries(test_crawler tue_filesystem)
