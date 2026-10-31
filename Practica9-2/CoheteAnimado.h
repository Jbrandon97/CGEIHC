#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
class CoheteAnimado {
public:
    void Actualizar(double dt);
    void Reiniciar(){tiempo=0;}
    glm::vec3 Posicion() const;
    glm::mat4 Matriz() const;
    double Tiempo() const{return tiempo;}
private:
    double tiempo=0;
};
