# include "ClasePrueba.hpp"
# include <iostream>

namespace IVJ {

		Alumno::Alumno(const char *const nombre, long matricula, int semestre)
				: nombre{nombre}, matricula{matricula}, semestre{semestre} {
				std::cout << "Se creo el objeto: " << nombre << "\n";
		}

		Alumno::~Alumno() {
				std::cout << "Se destruyó el objeto: " << nombre << "\n";
		}

		void Alumno::setNombre(const char *const n) {
				nombre = std::string(n);
		}
		void Alumno::setMatricula(long n) {
				matricula = n;
		}
		void Alumno::setSemestre(int n) {
				matricula = n;
		}
}
