cmake_minimum_required(VERSION 3.15)
project(TodoManagerBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite3 - try to find it, but don't fail if not found via package manager
find_package(SQLite3 QUIET)

# If SQLite3 not found via package manager, try to find it manually
if(NOT SQLite3_FOUND)
    # Try common locations
    find_path(SQLITE3_INCLUDE_DIR sqlite3.h
        PATHS
        ${CMAKE_SOURCE_DIR}/third_party
        C:/vcpkg/installed/x64-windows/include
        /usr/include
        /usr/local/include
    )
    
    find_library(SQLITE3_LIBRARY
        NAMES sqlite3
        PATHS
        C:/vcpkg/installed/x64-windows/lib
        /usr/lib
        /usr/local/lib
    )
    
    if(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
        set(SQLite3_FOUND TRUE)
        set(SQLite3_LIBRARIES ${SQLITE3_LIBRARY})
        set(SQLite3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
    endif()
endif()

# If still not found, we'll link it manually (header-only or system library)
if(NOT SQLite3_FOUND)
    message(WARNING "SQLite3 not found via package manager, will try to link manually")
    set(SQLite3_LIBRARIES "")
    set(SQLite3_INCLUDE_DIRS "")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/routes.cpp
    src/db.cpp
    src/task.cpp
    src/user.cpp
    src/auth.cpp
)

# Include directories
include_directories(
    include
    third_party
    ${SQLite3_INCLUDE_DIRS}
)

# Create executable
add_executable(todomanager ${SOURCES})

# Link libraries
if(SQLite3_LIBRARIES)
    target_link_libraries(todomanager ${SQLite3_LIBRARIES})
else()
    # Try to link sqlite3 as a system library (may be in system path)
    target_link_libraries(todomanager sqlite3)
endif()

# Copy sqlite3.dll to output directory on Windows
if(WIN32)
    if(EXISTS "${SQLITE3_LIBRARY_DIRS}/sqlite3.dll")
        add_custom_command(TARGET todomanager POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SQLITE3_LIBRARY_DIRS}/sqlite3.dll"
            $<TARGET_FILE_DIR:todomanager>)
    endif()
endif()

