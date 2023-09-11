cmake_minimum_required(VERSION 3.12)
project(Mumble)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
        libavformat
        libswscale
        libavutil
        libavcodec
)

set(TARGET mumble)
set(OBJS src/Main.cpp
		src/Output/Output.cpp
		include/Output/Output.hpp
		src/Output/RawFile.cpp
		include/Output/RawFile.hpp)

add_compile_options(-static -Wall)

add_executable(${TARGET} ${OBJS})

target_link_libraries(${TARGET} PkgConfig::LIBAV)

if(NOT DEFINED FFMPEG_PROJECT_DIR AND CMAKE_SYSTEM_NAME STREQUAL "Android")
	message(SEND_ERROR "Cannot configure MumbleVP for Android platform without external FFmpeg libraries!")
	message(SEND_ERROR "FFMPEG_PROJECT_DIR is not set!")
	message(FATAL_ERROR "Download FFMpeg sources and compile it for your platform, then try again.")

	exit()
endif()
