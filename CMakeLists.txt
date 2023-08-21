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
    GIT_TAG 5e03a3347adb4d8c55bd8c6090c0a75dfea065a3 # release-0.2.0
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/gui-app-template
)

# pugixml library
FetchContent_Populate(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG a0e064336317c9347a91224112af9933598714e9 # release-1.13.0
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/pugixml-1.13.0
)

# Executable
add_subdirectory(libs/gui-app-template) # Add gui-app-template
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/*.?pp)
file(GLOB SOURCES_XML ${PROJECT_SOURCE_DIR}/libs/pugixml-1.13.0/src/*.?pp)
add_executable(CoreNodes ${SOURCES} ${SOURCES_XML})
target_link_libraries(CoreNodes
    PRIVATE GuiAppTemplate
)
target_include_directories(CoreNodes
    PRIVATE libs/gui-app-template
    PRIVATE libs/pugixml-1.13.0/src
)