#include<iostream>
#include "../Motor/Motor.hpp"
#include "../Motor/Utils/Utils.hpp"
#include "../Juego/Juego.hpp"
#include <cstdlib>
#include <ctime>

int main(int argc, char** argv)
{
    std::cout<<"argc: "<<argc<<" argv:"<<argv<<"\n";
    std::srand(std::time({}));
    //Crear objeto del motor
    CE::MotorConfig config;
    config.vW=1080;
    config.vH=720;
    config.titulo="CimaEngine3v";
    std::cout<<"Iniciando "<<config.titulo<<"\n";

    CE::Motor motor{config,std::make_unique<IVJ::Juego>()};
    //ejecutar

    return motor.OnRun();
}
