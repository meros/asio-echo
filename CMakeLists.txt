cmake_minimum_required(VERSION 2.8)

set( Boost_USE_STATIC_LIBS ON )
find_package( Boost 1.40 COMPONENTS program_options REQUIRED system )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable( echo main_echo.cpp )
target_link_libraries( echo ${Boost_LIBRARIES} )
