#include "GestorEscenas.hpp"

namespace CE
{
    GestorEscenas& GestorEscenas::Get()
    {
        if(!instancia)
            instancia = new GestorEscenas();
        return *instancia;
    }
    void GestorEscenas::registrarEscena(const std::string& key, const std::shared_ptr<Escena>& escena)
    {
        if(escenas.empty())
            escena_actual = key;
        escenas[key]=escena;
        escena->nombre = key;
    }
    void GestorEscenas::cambiarEscena(const std::string& key)
    {
        //no hay escena con ese key
        if(escenas.find(key) == escenas.end() || escenas.empty()) return;

        //salir escena anterior
        escenas[escena_actual]->onFinal();
        escena_actual = key;
        //entrar a la nueva escena
        escenas[escena_actual]->onInit();
    }

    void GestorEscenas::cambiarEscenayEliminar(const std::string& key, bool eliminar)
    {
        if(escenas.find(key) == escenas.end() || escenas.empty()) return;

        //salir escena anterior
        std::string key_borrar = escena_actual;
        escenas[escena_actual]->onFinal();
        escena_actual = key;
        if(eliminar)
            escenas.erase(key_borrar);
        //entrar a la nueva escena
        escenas[escena_actual]->onInit();
    }

    Escena& GestorEscenas::getEscenaActual()
    {
        return *escenas[escena_actual];
    }
}
