#pragma once
#include "Circuito.h"
#include <gtc/matrix_transform.hpp>
class Animacion {
public:
    double distancia=0,tiempo=0;
    static constexpr double velocidad=18;
    void Actualizar(double dt,const Circuito&);
    glm::mat4 Nave(const Circuito&) const;
    glm::vec3 PosicionSatelite() const;
    void Satelite(glm::mat4* matrices) const;
    static glm::vec3 Pivote(int i);
};
bool VerificarAnimacion(const Circuito&);
