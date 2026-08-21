#pragma once
#include <string>

namespace IVJ {
class Alumno {
	public:
		// Constructor y Destructor 
		explicit Alumno(const char * const nombre, long matricula, int semestre);
		~Alumno();
		
		// Getters y Setters
		std::string getNombre() const {
				return nombre;
		}

		long getMatricula() const {
				return matricula;
		}

		int getSemestre() const {
				return semestre;
		}

		void setNombre(const char *const n);
		void setMatricula(long n);
		void setSemestre(int n);

	private:
		std::string nombre;
		long matricula;
		int semestre;

};
}
