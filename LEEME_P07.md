# Practica 07 - Iluminacion 1
Campos Medrano John Brandon - 418048324 - Laboratorio 2 - Teoria 7 - 2027-1

Abrir P07-418048324.sln en Visual Studio 2022 con C++ de escritorio, seleccionar x64 y compilar Debug o Release. Tambien se incluye el ejecutable x64 en la raiz. Todos los recursos y DLL estan incluidos; el programa resuelve sus rutas junto al ejecutable y la compilacion copia los recursos automaticamente.

## Controles
- 0: camara libre; W/A/S/D: mover; boton derecho y raton: mirar.
- 1: dado y holocron; 2: rover; 3: avion.
- I/K: avance y retroceso del rover; J/L: giro.
- Flechas arriba/abajo: avance y retroceso del avion; RePag/AvPag: subir y bajar; Z/X: giro.
- T/G: base del brazo; Y/H: primer segmento; U/N: segundo segmento.
- O/P: suspension; 4-9: seleccionar una de las seis patas; B/V: girar la pata seleccionada con su rueda.
- F1: faro rover; F2: luz inferior avion; F3: faro frontal avion.
- R: restablecer posiciones y articulaciones; Esc: salir.

## Modelos y jerarquia
Dado: CrearDado del Ejercicio 6, 24 vertices/36 indices y UV Star Wars originales.
Holocron: CrearHolocron de P06, 12 posiciones/20 triangulos, UV originales y normales planas por producto cruz.
Avion: OBJ/MTL y textura Ripslinger originales de P06, sin alteracion de geometria ni UV.
Rover: fuente E04 corregida en Blender; 12 OBJ locales: cuerpo, dos suspensiones, base y dos segmentos del brazo, seis conjuntos pata-rueda. La jerarquia y los pivotes estan en Practica7/Models/rover_jerarquia.json y Rover_Corregido.blend. La rotacion de una pata conserva su rueda como parte de la misma malla; no simula rodadura ni contacto fisico con el terreno.

Las tres luces se actualizan con AsociarLuz: posicion = padre * vec4(offset,1); direccion = normalize(padre * vec4(direccionLocal,0)). El faro frontal del avion se inclina hacia el piso para mostrar su haz. Se conserva DirectionalLight, PointLight, SpotLight, Material y shader_light; no hay luces puntuales activas.

## Verificacion reproducible
Ejecutar P07-418048324.exe --pruebas RUTA_DE_SALIDA genera 20 capturas PPM reales del framebuffer y pruebas.csv con posiciones/direcciones. La secuencia usa la misma funcion de movimiento que el teclado y termina automaticamente. Incluye avance/retroceso, subida/bajada, giros, brazo/suspension y las seis patas. No genera video.

Las carpetas Models, Textures y shaders de la raiz permiten ejecutar el EXE directamente; sus copias en Practica7 permiten compilar y copiar recursos, siguiendo P05/P06. Los temporales de compilacion estan excluidos del ZIP y de Git.
