cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_PROJECT_NAME makalu_pdm)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(${CMAKE_PROJECT_NAME} C ASM)

# SDK must come before cubemx so makalu_sdk target exists
add_subdirectory(sdk ${CMAKE_BINARY_DIR}/makalu_sdk)

# CubeMX project
add_subdirectory(cubemx/makalu_pdm)

target_link_libraries(${CMAKE_PROJECT_NAME}
        makalu_sdk
        # Add user defined libraries
)


