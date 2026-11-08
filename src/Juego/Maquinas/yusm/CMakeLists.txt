project(CimaEngine3v)
file(GLOB_RECURSE gl_cpp RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(CimaEngine3v PRIVATE ${gl_cpp})
