cmake_minimum_required(VERSION 3.23)
project(fibers
        VERSION 0.1.0
        DESCRIPTION "Lightweight fibers library")

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} STATIC)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE INCLUDE_FILES include/*.h)


target_sources(${PROJECT_NAME}
        PRIVATE
        ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

foreach(header ${public_headers})
    file(RELATIVE_PATH header_file_path "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}" "${header}")
    get_filename_component(header_directory_path "${header_file_path}" DIRECTORY)
    install(
            FILES ${header}
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${header_directory_path}"
    )
endforeach()

add_executable(example example/example.cpp ${INCLUDE_FILES} ${SOURCE_FILES})