CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Parser)

FIND_PACKAGE(Boost 1.40.0 REQUIRED
    COMPONENTS
        filesystem
        program_options
        regex
        date_time)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} .)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ADD_DEFINITIONS("-std=c++0x -O4")

ADD_EXECUTABLE(parser main)

TARGET_LINK_LIBRARIES(parser
    boost_program_options-mt
    boost_regex-mt
    boost_filesystem-mt
    boost_system-mt
    boost_date_time-mt)

