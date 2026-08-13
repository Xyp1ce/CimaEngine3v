project(CimaEngine3v)

add_executable(CimaEngine3v main.cpp)

# copiar el archivo de configuración para las ides que sepan 
# donde estan todos los paths de las librerias y archivos.
if(NOT WIN32)
add_custom_command(
    TARGET CimaEngine3v POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/build/compile_commands.json # fuente
    ${CMAKE_SOURCE_DIR}/compile_commands.json                           # destino
    COMMENT "copiando compile_commands.json "
    )
endif()
