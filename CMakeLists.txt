cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    odyssey
    VERSION 0.1.0
    DESCRIPTION "The Odyssey Programming Language"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    odyssey_lib OBJECT
    source/lib.cpp
)

target_include_directories(
    odyssey_lib ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(odyssey_lib PUBLIC cxx_std_17)

# ---- Declare executable ----

add_executable(odyssey_exe source/main.cpp)
add_executable(odyssey::exe ALIAS odyssey_exe)

set_property(TARGET odyssey_exe PROPERTY OUTPUT_NAME odyssey)

target_compile_features(odyssey_exe PRIVATE cxx_std_17)

target_link_libraries(odyssey_exe PRIVATE odyssey_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT odyssey_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of odyssey"
  )
endif()

include(cmake/dev-mode.cmake)
