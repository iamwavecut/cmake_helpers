# This file is part of Forkgram.

add_library(external_libsodium INTERFACE IMPORTED GLOBAL)
add_library(desktop-app::external_libsodium ALIAS external_libsodium)

set(libsodium_min_version 1.0.19)

find_package(unofficial-sodium CONFIG QUIET)
if (unofficial-sodium_FOUND)
    target_link_libraries(external_libsodium INTERFACE unofficial-sodium::sodium)
    return()
endif()

find_package(PkgConfig)
if (PkgConfig_FOUND)
    pkg_check_modules(DESKTOP_APP_LIBSODIUM IMPORTED_TARGET libsodium>=${libsodium_min_version})
endif()

if (DESKTOP_APP_LIBSODIUM_FOUND)
    target_link_libraries(external_libsodium INTERFACE PkgConfig::DESKTOP_APP_LIBSODIUM)
    return()
endif()

if (DESKTOP_APP_USE_PACKAGED)
    message(FATAL_ERROR "Could not find libsodium >= ${libsodium_min_version}")
endif()

if (WIN32)
    set(libsodium_loc ${libs_loc}/libsodium)
    target_compile_definitions(external_libsodium INTERFACE SODIUM_STATIC)
    target_include_directories(external_libsodium SYSTEM INTERFACE ${libsodium_loc}/include)
    target_link_libraries(external_libsodium INTERFACE
        ${libsodium_loc}/out/$<IF:$<CONFIG:Debug>,Debug,Release>/libsodium.lib
    )
elseif (APPLE)
    target_include_directories(external_libsodium SYSTEM INTERFACE ${libs_loc}/local/include)
    target_link_libraries(external_libsodium INTERFACE ${libs_loc}/local/lib/libsodium.a)
else()
    find_path(DESKTOP_APP_LIBSODIUM_INCLUDE_DIR sodium.h REQUIRED)
    find_library(DESKTOP_APP_LIBSODIUM_LIBRARIES NAMES sodium libsodium REQUIRED)
    target_include_directories(external_libsodium SYSTEM INTERFACE ${DESKTOP_APP_LIBSODIUM_INCLUDE_DIR})
    target_link_libraries(external_libsodium INTERFACE ${DESKTOP_APP_LIBSODIUM_LIBRARIES})
endif()
