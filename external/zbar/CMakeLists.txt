add_library(external_zbar INTERFACE IMPORTED GLOBAL)
add_library(desktop-app::external_zbar ALIAS external_zbar)

set(zbar_loc ${third_party_loc}/zbar)
set(zbar_src ${zbar_loc}/zbar)

add_library(external_zbar_bundled STATIC)
init_target(external_zbar_bundled "(external)")

nice_target_sources(external_zbar_bundled ${zbar_src}
PRIVATE
    config.c
    convert.c
    decoder.c
    decoder.h
    error.c
    error.h
    image.c
    image.h
    img_scanner.c
    img_scanner.h
    misc.c
    misc.h
    refcnt.c
    refcnt.h
    scanner.c
    symbol.c
    symbol.h
    decoder/qr_finder.c
    decoder/qr_finder.h
    qrcode/bch15_5.c
    qrcode/bch15_5.h
    qrcode/binarize.c
    qrcode/binarize.h
    qrcode/isaac.c
    qrcode/isaac.h
    qrcode/qrdec.c
    qrcode/qrdec.h
    qrcode/qrdectxt.c
    qrcode/rs.c
    qrcode/rs.h
    qrcode/util.c
    qrcode/util.h
)

target_include_directories(external_zbar_bundled SYSTEM
PUBLIC
    ${zbar_loc}/include
    ${zbar_src}
)

target_include_directories(external_zbar_bundled
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (WIN32)
    target_sources(external_zbar_bundled PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/win_iconv/win_iconv.c
    )
    target_include_directories(external_zbar_bundled PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/win_iconv
    )
    target_link_libraries(external_zbar_bundled PRIVATE winmm)
elseif (APPLE)
    target_link_libraries(external_zbar_bundled PRIVATE
        desktop-app::external_iconv
    )
endif()

if (MSVC)
    target_compile_options(external_zbar_bundled
    PRIVATE
        /wd4013
        /wd4146
        /wd4311
        /wd4554
        /wd4996
    )
else()
    target_compile_options(external_zbar_bundled PRIVATE -w)
endif()

target_compile_definitions(external_zbar_bundled
PRIVATE
    HAVE_CONFIG_H=1
    HAVE_INTTYPES_H=1
    HAVE_ERRNO_H=1
    $<$<NOT:$<PLATFORM_ID:Windows>>:HAVE_SYS_TIME_H=1>
    NO_STATS=1
    ENABLE_QRCODE=1
    ENABLE_EAN=0
    ENABLE_DATABAR=0
    ENABLE_CODE128=0
    ENABLE_CODE93=0
    ENABLE_CODE39=0
    ENABLE_CODABAR=0
    ENABLE_I25=0
    ENABLE_PDF417=0
    ENABLE_SQCODE=0
    ZBAR_VERSION_MAJOR=0
    ZBAR_VERSION_MINOR=23
    ZBAR_VERSION_PATCH=0
    _CRT_SECURE_NO_WARNINGS=1
    _CRT_NONSTDC_NO_DEPRECATE=1
)

target_link_libraries(external_zbar
INTERFACE
    external_zbar_bundled
)
