cmake_minimum_required(VERSION 3.10)

project(ReCaptchaV3Plugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tenter find_package sur Linux / config auto
find_package(CURL QUIET)

if(NOT CURL_FOUND)
    if(WIN32)
        # Exemple avec vcpkg
        set(CURL_INCLUDE_DIR "C:/vcpkg/installed/x64-windows/include")
        set(CURL_LIBRARY     "C:/vcpkg/installed/x64-windows/lib/libcurl.lib")
    else()
        # Exemple Debian/Ubuntu
        set(CURL_INCLUDE_DIR "/usr/include")
        set(CURL_LIBRARY     "/usr/lib/x86_64-linux-gnu/libcurl.so")
    endif()
else()
    set(CURL_INCLUDE_DIR ${CURL_INCLUDE_DIRS})
    set(CURL_LIBRARY     CURL::libcurl)
endif()

add_library(ReCaptchaV3Plugin SHARED
        ReCaptchaV3Plugin.cpp
        ReCaptchaV3Plugin.h
        ReCaptchaV3Plugin.cpp
)

if(WIN32)
    target_compile_definitions(ReCaptchaV3Plugin PRIVATE RECAPTCHA_EXPORTS)
endif()

target_include_directories(ReCaptchaV3Plugin PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(ReCaptchaV3Plugin PRIVATE ${CURL_LIBRARY})

add_executable(test_plugin
        main.cpp
)

target_include_directories(test_plugin PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(test_plugin PRIVATE ReCaptchaV3Plugin)
