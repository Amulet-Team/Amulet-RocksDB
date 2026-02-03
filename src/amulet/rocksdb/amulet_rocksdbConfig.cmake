if (NOT TARGET rocksdb)
    message(STATUS "Finding rocksdb")

    set(rocksdb_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
    find_library(rocksdb_LIBRARY NAMES rocksdb PATHS "${CMAKE_CURRENT_LIST_DIR}")
    message(STATUS "rocksdb_LIBRARY: ${rocksdb_LIBRARY}")

    add_library(rocksdb_bin SHARED IMPORTED)
    set_target_properties(rocksdb_bin PROPERTIES
        IMPORTED_IMPLIB "${rocksdb_LIBRARY}"
    )

    add_library(rocksdb INTERFACE)
    target_link_libraries(rocksdb INTERFACE rocksdb_bin)
    target_include_directories(rocksdb INTERFACE ${rocksdb_INCLUDE_DIR})
endif()
