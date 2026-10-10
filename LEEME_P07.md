# Practica 07 - Iluminacion 1
Campos Medrano John Brandon - 418048324 - Laboratorio 2 - Teoria 7 - 2027-1

Abrir P07-418048324.sln en Visual Studio 2022 con C++ de escritorio, seleccionar x64 y compilar Debug o Release. Tambien se incluye el ejecutable x64 en la raiz. Todos los recursos y DLL estan incluidos; el programa resuelve sus rutas junto al ejecutable y la compilacion copia los recursos automaticamente.

## Controles
La ayuda se imprime una sola vez al iniciar. No hay funciones de movimiento en las flechas ni en Page Up/Page Down.
- WASD + mouse: camara libre (cursor capturado).
- Q/E: rover avanza/retrocede; Z/X: gira izquierda/derecha.
- U/O: avion avanza/retrocede; I/K: sube/baja; J/L: gira izquierda/derecha.
- 1: faro frontal rover; 2: luz inferior avion; 3: faro frontal avion (conmutacion individual).
- 4: apaga las tres si estan encendidas; si alguna esta apagada, enciende todas.
- C/V: base del brazo; B/N: brazo 1; M/coma: brazo 2.
- F5: camara libre; F6: dado y holocron; F7: rover; F8: avion.
- R: restablecer vehiculos y articulaciones. Esc: salir.

## Posiciones
Dado: centro (-5,5,6), escala 2 y rotacion -90 grados alrededor de Z; cara inferior a 4 unidades sobre el piso.
Holocron: centro (-9,7,6), escala 0.6; punto inferior a 5.43884 unidades sobre el piso.
Piso en Y=0. Se conservan las geometrias y coordenadas UV originales.

## Modelos y jerarquia
Dado: CrearDado del Ejercicio 6, 24 vertices/36 indices y UV Star Wars originales.
Holocron: CrearHolocron de P06, 12 posiciones/20 triangulos, UV originales y normales planas por producto cruz.
Avion: OBJ/MTL y textura Ripslinger originales de P06, sin alteracion de geometria ni UV.
Rover: fuente E04 corregida en Blender; 12 OBJ locales: cuerpo, dos suspensiones, base y dos segmentos del brazo, seis conjuntos pata-rueda. La jerarquia y los pivotes estan en Practica7/Models/rover_jerarquia.json y Rover_Corregido.blend. Las suspensiones y los conjuntos pata-rueda mantienen su posicion local respecto al cuerpo. El brazo conserva sus tres articulaciones.

Las tres luces se actualizan con AsociarLuz: posicion = padre * vec4(offset,1); direccion = normalize(padre * vec4(direccionLocal,0)). El faro frontal del avion se inclina hacia el piso para mostrar su haz. Se conserva DirectionalLight, PointLight, SpotLight, Material y shader_light; no hay luces puntuales activas.

## Verificacion reproducible
Ejecutar P07-418048324.exe --pruebas RUTA_DE_SALIDA genera 14 capturas PPM reales del framebuffer y pruebas.csv con posiciones/direcciones. La secuencia usa la misma funcion de movimiento que el teclado y termina automaticamente. Incluye avance/retroceso, subida/bajada, giros, articulacion del brazo y seguimiento de las luces.

Las carpetas Models, Textures y shaders de la raiz permiten ejecutar el EXE directamente; sus copias en Practica7 permiten compilar y copiar recursos, siguiendo P05/P06. Los temporales de compilacion estan excluidos del ZIP y de Git.

## Materiales originales del rover
Las 12 piezas conservan sus vertices, triangulos y pivotes. Cada cara recupera su material original del archivo Rover_Separado.blend, exportado como usemtl/Kd/Ks/Ns en rover.mtl. Model.cpp aplica el color por submalla; el rover no carga plain.png.
Materiales: SpacialRoverExploreralumWhite, alumGray, alumBlack, alumOrange, alumGrayDeep, SpacialRoverExplorerwheels y SpacialRoverExploreralumArm (los nombres alum... comparten el prefijo SpacialRoverExplorer).
LIMITACION DEL ARCHIVO FUENTE: SpacialRoverT301Texture.png esta referenciada por el brazo, pero no esta en los archivos proporcionados ni incrustada en el BLEND/FBX. Esa zona conserva el color base original del material alumArm (0.8,0.8,0.8). No se invento ni se descargo una textura sustituta. El ejecutable es autocontenido y no intenta cargar ese archivo ausente.
Los colores y la asignacion por cara son los originales; el brillo/aspereza se aproxima al sistema Phong de la practica, sin convertirlo en un renderizador PBR.
