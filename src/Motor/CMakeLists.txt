project(CimaEngine3v)

add_subdirectory(Render)
add_subdirectory(App)
add_subdirectory(GUI)
add_subdirectory(Primitivos)
add_subdirectory(Utils)
add_subdirectory(Componentes)
add_subdirectory(Camaras)
add_subdirectory(Inputs)


target_sources(CimaEngine3v PRIVATE Motor.cpp)
