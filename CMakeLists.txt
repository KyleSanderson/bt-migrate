cmake_minimum_required(VERSION 3.5)
project(BtMigrate)

if(BT_MIGRATE_PREFER_STATIC_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a .so .sl .dylib .dll.a)
    set(Boost_USE_STATIC_LIBS ON)

    if(BT_MIGRATE_BOOST_USES_ICU)
        add_definitions(-DU_STATIC_IMPLEMENTATION)
    endif()
endif()

find_package(Boost 1.50 REQUIRED COMPONENTS date_time filesystem locale program_options system thread)

if(BT_MIGRATE_BOOST_USES_ICU)
    find_package(ICU REQUIRED data i18n uc)
    target_link_libraries(Boost::locale
        INTERFACE
            ICU::data
            ICU::i18n
            ICU::uc)
    target_link_libraries(ICU::i18n
        INTERFACE
            ICU::uc)
    target_link_libraries(ICU::uc
        INTERFACE
            ICU::data)
endif()

add_library(jsoncons::jsoncons INTERFACE IMPORTED)
set_property(TARGET jsoncons::jsoncons PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/ThirdParty/jsoncons/include")

add_library(pugixml::pugixml INTERFACE IMPORTED)
set_property(TARGET pugixml::pugixml PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/ThirdParty/pugixml/src")
set_property(TARGET pugixml::pugixml PROPERTY INTERFACE_COMPILE_DEFINITIONS PUGIXML_HEADER_ONLY)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DBOOST_ALL_NO_LIB)

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

include_directories(.)

add_subdirectory(Codec)
add_subdirectory(Common)
add_subdirectory(Store)
add_subdirectory(Torrent)

add_executable(BtMigrate
    ImportHelper.cpp
    ImportHelper.h
    MigrationTransaction.cpp
    MigrationTransaction.h
    main.cpp)

target_link_libraries(BtMigrate
    PUBLIC
        BtMigrateCommon
    PRIVATE
        BtMigrateStore
        BtMigrateTorrent)

target_link_libraries(BtMigrate
    PUBLIC
        Boost::filesystem
    PRIVATE
        Boost::date_time
        Boost::locale
        Boost::program_options
        Boost::thread)
