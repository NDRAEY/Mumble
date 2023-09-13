cmake_minimum_required(VERSION 3.12)
project(Mumble)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# If you need to force config with PkgConfig, use -DCMAKE_SYSTEM_NAME=Linux
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	find_package(PkgConfig REQUIRED)

	pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
			libavformat
			libswscale
			libavutil
			libavcodec
	)
endif()

set(TARGET mumble)
set(OBJS src/Main.cpp
		src/Output/Output.cpp
		include/Output/Output.hpp
		src/Output/Debug.cpp
		include/Output/Debug.hpp
		include/Base.hpp
		include/Output/FindByName.hpp
		src/Output/FindByName.cpp
		src/Output/FramebufferIO.cpp
		include/Output/FramebufferIO.hpp
		src/Output/TerminalIO.cpp
		include/Output/TerminalIO.hpp)

add_compile_options(-static -Wall -O1)

# We don't need PkgConfig's LIBAV when we configuring for Android
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
	add_executable(${TARGET} ${OBJS})

	target_link_libraries(${TARGET} PkgConfig::LIBAV)
endif()

if(NOT DEFINED FFMPEG_PROJECT_DIR AND CMAKE_SYSTEM_NAME STREQUAL "Android")
	message(SEND_ERROR "Cannot configure MumbleVP for Android platform without external FFmpeg libraries!")
	message(SEND_ERROR "FFMPEG_PROJECT_DIR is not set!")
	message(FATAL_ERROR "Download FFMpeg sources and compile it for your platform, then try again.")

	exit()
endif()

# TODO: Add external FFMpeg's libraries and headers to 
#       compile options
if(DEFINED FFMPEG_PROJECT_DIR AND CMAKE_SYSTEM_NAME STREQUAL "Android")
	message(WARNING "Configuring for Android")
	message("FFMPEG: ${FFMPEG_PROJECT_DIR}")

	add_compile_options(-I${FFMPEG_PROJECT_DIR}/)

	add_link_options(
		${FFMPEG_PROJECT_DIR}/libavutil/libavutil.a
		${FFMPEG_PROJECT_DIR}/libavformat/libavformat.a
		${FFMPEG_PROJECT_DIR}/libavcodec/libavcodec.a
		${FFMPEG_PROJECT_DIR}/libswscale/libswscale.a
		${FFMPEG_PROJECT_DIR}/libswresample/libswresample.a
		-lz
		-lc
	)

	add_executable(${TARGET} ${OBJS})
endif()
