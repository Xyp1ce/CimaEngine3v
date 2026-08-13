# CimaEngine3v

[![Build](https://github.com/Omar-z/CimaEngine3v/actions/workflows/build.yml/badge.svg)](https://github.com/Omar-z/CimaEngine3v/actions/workflows/build.yml)

## Como compilar y ejecutar

1.- crear un folder build y moverse en consola a el

```
mkdir build && cd build
```

2.1 - en la consola dentro del folder build crear el ensamble, en este caso se puede **seleccionar un tool chain de multi-configuración**
* Ninja Multi-Config
* Xcode
* Visual Studio xxxx

```
cmake -G"Ninja Multi-Config" -B . ..
```
2.2 - compilar y generar el ejecutable
* Debug : Tiene banderas de depuración
* Release: Tiene banderas de optimización

```
cmake --build . --config Debug --target CimaEngine3v
```

En el caso que no se tenga un toolchain multi config (basado en Makefiles) entonces, se tiene que especificar la configuración al hacer el build

```
#para el build de Debug , en consola ir a CimaEngine/build/ 
cmake .. -DCMAKE_BUILD_TYPE=Debug

# para el build de Release , en consola ir a CimaEngine/build/ 
cmake .. -DCMAKE_BUILD_TYPE=Release

# Especificar si quiere usar un toolchain diferente

cmake -G"MinGW Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
```

```
# estando en el folder de `build` 
cmake --build . --target CimaEngine3v 
```

1. copiar el archivo `compiler_commands.json` generado en el folder `build` al folder raíz, este archivo le ayuda a tu IDE a autocompletar mejor y encontrar los paths
2.- el ejecutable se crea en `CimaEngine3v/bin/<perfil>/CimaEngine3v.exe`

```
# si se hizo un build de Debug
CimaEngine3v/bin/Debug/CimaEngine3v.exe

# si se hizo un build de Release
CimaEngine3v/bin/Release/CimaEngine3v.exe
```
