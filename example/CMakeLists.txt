cmake_minimum_required(VERSION 3.21)
project(libmediapipe_example)

find_package(OpenCV REQUIRED)

add_executable(example example.cpp)

option(MediaPipe_DIR "MediaPipe_DIR")
add_library(mediapipe SHARED IMPORTED)
target_include_directories(mediapipe INTERFACE ${MediaPipe_DIR}/include)
target_link_libraries(example PRIVATE mediapipe)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set_target_properties(mediapipe PROPERTIES
		IMPORTED_LOCATION ${MediaPipe_DIR}/lib/libmediapipe.so
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set_target_properties(mediapipe PROPERTIES
		IMPORTED_LOCATION ${MediaPipe_DIR}/bin/mediapipe.dll
		IMPORTED_IMPLIB ${MediaPipe_DIR}/lib/mediapipe.lib
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set_target_properties(mediapipe PROPERTIES
		IMPORTED_LOCATION ${MediaPipe_DIR}/lib/libmediapipe.dylib
	)
endif ()

target_include_directories(example PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(example PRIVATE mediapipe ${OpenCV_LIBS})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_custom_command(TARGET example POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:example> $<TARGET_FILE_DIR:example>
		COMMAND_EXPAND_LISTS
	)
endif ()
