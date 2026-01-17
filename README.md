# Proyecto 1 SO - Arquitectura Virtual

**Autores:**  
Carlos Miranda (C.I. 31080238)  
Oscar Allen (C.I. 31115394)  

**Materia:** Sistemas Operativos  
**Escuela:** Escuela de Computación, Facultad de Ciencias  
**Universidad:** Universidad Central de Venezuela  

## Descripción

Este proyecto consiste en la implementación de una arquitectura virtual en lenguaje C. Incluye componentes esenciales como un procesador, memoria principal y secundaria, controlador DMA, sistema de interrupciones, reloj y un cargador de programas. Permite la ejecución de programas simples en dos modos: normal y debug.

## Estructura de Archivos

El proyecto está organizado de la siguiente manera:

- `source/`: Contiene el código fuente en C y los archivos de cabecera (.h).
  - `main.c`: Archivo principal.
  - `procesador.c` y `procesador.h`: Implementación del procesador y sus auxiliares.
  - `memoriaPrincipal.c` y `memoriaPrincipal.h`: Gestión de la memoria principal.
  - `memoriaSecundaria.c` y `memoriaSecundaria.h`: Gestión de la memoria secundaria.
  - `DMA.c` y `DMA.h`: Controlador de operaciones de entrada y salida.
  - `interrupciones.c` y `interrupciones.h`: Manejo de interrupciones.
  - `reloj.c` y `reloj.h`: Simulación del reloj del sistema.
  - `logger.c` y `logger.h`: Sistema de logging para depuración.
  - `cargadorDePrograma.c` y `cargadorDePrograma.h`: Cargador de programas desde archivos.
  - Otros archivos auxiliares como `procesador_aux.c`, `procesador_data.h`, `proceso.h`, etc.
- `test/`: Archivos de prueba con programas de ejemplo (.in).
- `Makefile`: Script para la compilación del proyecto.
- `README.md`: Este archivo de documentación.

## Requisitos

- Compilador GCC compatible con C11.
- Sistema operativo con soporte para hilos (POSIX threads).
- Entorno de desarrollo que soporte Makefiles (como Linux, macOS o Windows con WSL).

## Make
- `make`: Compila.
- `make run`: Compila y ejecuta.
- `make clean`: Elimina archivos objeto y el ejecutable.
- `make debug`: Compila con símbolos de depuración.

## Funcionamiento General

El simulador inicia creando instancias de los componentes principales: logger, procesador y DMA. Luego, presenta una consola interactiva donde el usuario puede ingresar comandos.

- **Comando `ejecutar`**: Permite cargar un programa desde un archivo de texto. El usuario especifica la ruta del archivo y el modo de ejecución (1 para normal, 2 para debug). El cargador de programas lee el archivo, que contiene instrucciones en un formato específico (opcode, dirección, valor), y las carga en memoria. El procesador ejecuta las instrucciones secuencialmente, interactuando con la memoria, DMA para transferencias, y manejando interrupciones según sea necesario. En modo debug, se muestra información detallada de cada paso.

- **Comando `salir`**: Termina la simulación.

El procesador simula un CPU simple con instrucciones como load, sum, push, svc, etc. La memoria se divide en principal (acceso rápido) y secundaria (almacenamiento persistente). El DMA facilita transferencias de datos entre memorias sin intervención del CPU. El reloj proporciona sincronización, y el logger registra eventos para análisis.

Los programas de prueba en `test/` demuestran funcionalidades como operaciones aritméticas, bucles, entrada/salida y modos kernel.