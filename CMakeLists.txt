# Minimum CMake version
cmake_minimum_required(VERSION 3.14)

# Name of the project
project(core-nodes)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
# gui-app-template library
FetchContent_Populate(
    gui-app-template
    GIT_REPOSITORY https://github.com/onurae/gui-app-template.git
    GIT_TAG main
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/gui-app-template
)

# Executable
add_subdirectory(libs/gui-app-template) # Add gui-app-template
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/*.?pp)
add_executable(CoreNodes ${SOURCES})
target_link_libraries(CoreNodes
    PRIVATE GuiAppTemplate
)
target_include_directories(CoreNodes
    PRIVATE libs/gui-app-template
)