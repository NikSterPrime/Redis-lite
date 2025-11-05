cmake_minimum_required(VERSION 3.15)
project(Redis-lite VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific libs
if(WIN32)
	set(PLATFORM_LIBS ws2_32)
else()
	set(PLATFORM_LIBS pthread)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(DATASTORE_SOURCES
	src/datastore.cpp
)

# CLI executable
add_executable(redis-cli
	src/main.cpp
	${DATASTORE_SOURCES}
)
target_link_libraries(redis-cli PRIVATE ${PLATFORM_LIBS})
set_target_properties(redis-cli PROPERTIES OUTPUT_NAME "redis" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Server executable (single-threaded basic)
add_executable(redis-server
	src/server_basic.cpp
	${DATASTORE_SOURCES}
)
target_link_libraries(redis-server PRIVATE ${PLATFORM_LIBS})
set_target_properties(redis-server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Client executable
add_executable(redis-client
	src/client.cpp
)
target_link_libraries(redis-client PRIVATE ${PLATFORM_LIBS})
set_target_properties(redis-client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

install(TARGETS redis-cli redis-server redis-client RUNTIME DESTINATION bin)

