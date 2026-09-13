# Práctica 3 — Modelado geométrico

John Brandon Campos Medrano · Cuenta 418048324 · Laboratorio 2 · Teoría 7 · 2027-1.

Esta rama contiene el reporte de práctica 3: un cohete y una cruz de ocho
pirámides juntos en una sola ventana. No corresponde al cubo del ejercicio de clase.

## Archivos principales

- [Main P03-418048324.cpp](PRACTICA3/P03-418048324.cpp)
- [Reporte PDF](Documentos/P03-418048324.pdf)
- [Reporte Word editable](Documentos/P03-418048324.docx)
- [Video de 45 segundos](Documentos/P03-418048324.mp4)
- [Ejecutable Release](x64/Release/P03-418048324.exe)
- [PDB de Release](x64/Release/P03-418048324.pdb)
- [Solución Visual Studio](PRACTICA3.sln)

## Abrir y ejecutar

Descarga esta rama completa y extrae el ZIP. Abre PRACTICA3.sln con Visual Studio
2022; selecciona Release y x64. Ctrl+Mayús+B compila y Ctrl+F5 ejecuta.
Se requiere Desarrollo para el escritorio con C++, MSVC v143 y Windows SDK.

También puede abrirse x64/Release/P03-418048324.exe directamente. Deben permanecer
glew32.dll, glfw3.dll y la carpeta shaders junto al ejecutable. Se incluyen las
dependencias utilizadas (GLEW, GLFW y GLM), no bibliotecas ajenas como ASSIMP.

## Controles

- Flechas: rodear las figuras con la cámara.
- Arrastrar con botón izquierdo: cambiar la dirección de la mirada.
- W/S: avanzar/retroceder; A/D: desplazarse de lado; Q/E: bajar/subir.
- Rueda: acercar/alejar. Espacio: activar/detener el giro automático.
- R: recuperar la vista inicial y detener el giro. Esc: cerrar.

No hay selector de escenas ni vista de pirámide aislada. No hay plataforma negra.
El cohete conserva una aleta piramidal trasera para cumplir las cinco primitivas.
La cruz tiene ocho pirámides, en cuatro pares separados 0.16 unidades.
Cada pirámide tiene caras roja, verde, amarilla, magenta y base cuadrada azul.

## Estructura del laboratorio

Se conservaron y adaptaron las clases proporcionadas por el profesor:
Window recibe el input; Camera calcula la vista; Shader carga archivos externos;
Mesh y MeshColor dibujan geometría; Sphere genera la ventanilla del cohete.
El main organiza las piezas y llama a esas clases.

Se corrigieron la inicialización de Window, el conteo de MeshColor, índices de
Sphere y la aplicación de view en los shaders actualizados. Se contó con apoyo
de IA para adaptar el código y preparar el reporte, y se comprobó la ejecución real.

## Evidencias y entrega

Compilación y ejecución comprobadas en Release y Debug x64. El modo
`--verify CARPETA` guarda ocho vistas y comprueba errores de OpenGL.
El video muestra la misma escena ejecutada, sin audio.

Para Classroom, adjuntar el reporte, el main y el video por separado. Los archivos
de clases y shaders modificados se pueden adjuntar agrupados. Este repositorio
incluye además lo necesario para compilar y ejecutar; no contiene carpetas .vs,
archivos .obj, bases de datos de compilación ni ejecutables de Debug.
