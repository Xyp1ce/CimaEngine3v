project(CimaEngine3v)

#agregar los folders restantes
add_subdirectory(Figuras)
add_subdirectory(Sistemas)
add_subdirectory(Escenas)
add_subdirectory(objetos)
add_subdirectory(Componentes)
add_subdirectory(Maquinas)

target_sources(CimaEngine3v PRIVATE Juego.cpp)




