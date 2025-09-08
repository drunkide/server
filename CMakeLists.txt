cmake_minimum_required(VERSION 3.5...3.31)
project(Server)

include(DrunkFly/Common)

gcc_visibility_hidden()
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

create_source_list(src server
    cgo.cpp
    cgo.h
    client.cpp
    client.h
    common.h
    )

create_source_list(src_go . HEADERS
    main.cpp
    main.go
    main.h
    )

add_library(server SHARED ${src})

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    PROPERTY VS_STARTUP_PROJECT "server"
    )

if(WIN32)
    set(timestamp_go "${CMAKE_CURRENT_SOURCE_DIR}/main_timestamp.go")
    set(server_exe "${CMAKE_CURRENT_SOURCE_DIR}/build/server.exe")

    add_custom_target(server_exe
        DEPENDS
            "${server_exe}"
        SOURCES
            ${src_go}
        )

    add_custom_command(OUTPUT
            "${timestamp_go}"
        COMMAND
            "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/tools/write_timestamp.cmake"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/server/cgo.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/write_timestamp.cmake"
            ${src_go}
        MAIN_DEPENDENCY
            "${CMAKE_CURRENT_SOURCE_DIR}/main.go"
        )

    add_custom_command(OUTPUT
            "${server_exe}"
        COMMAND
            "${CMAKE_CURRENT_SOURCE_DIR}/go.cmd"
                build
                    -x
                    -o "${server_exe}"
        DEPENDS
            "${timestamp_go}"
        MAIN_DEPENDENCY
            "${CMAKE_CURRENT_SOURCE_DIR}/main.go"
        )

    set_target_properties(server PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:server>"
        VS_DEBUGGER_COMMAND "${server_exe}"
        VS_DEBUGGER_COMMAND_ARGUMENTS ""
        )

    set_target_properties(server_exe PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:server>"
        VS_DEBUGGER_COMMAND "${server_exe}"
        VS_DEBUGGER_COMMAND_ARGUMENTS ""
        )

    add_dependencies(server server_exe)
endif()
