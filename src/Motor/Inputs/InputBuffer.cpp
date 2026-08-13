#include "InputBuffer.hpp"
#include <optional>

namespace CE
{
    InputBuffer::InputBuffer(int size)
        :tope{0},salir{0},size(size)
    {
        buffer.resize(size);
    }

    void InputBuffer::pushData(std::optional<sf::Event>& evento)
    {
        buffer[tope] = evento;
        if(ele == size)
            salir = (salir+1)%size;
        tope = (tope+1)%size;
        if(ele<size)
            ele++;
    }
    std::optional<sf::Event>& InputBuffer::popData()
    {
        if(ele == 0)
        {
            //dummy null
            static std::optional<sf::Event> null =std::nullopt;
            return null;
        }
        auto& dato = buffer[salir];
        salir = (salir+1)%size;
        if(ele>0)
            ele--;
        return dato;
    }

    void InputBuffer::clearData()
    {
        salir =0;
        tope =0;
        ele =0;
        buffer.assign(size,std::nullopt);
    }
}
