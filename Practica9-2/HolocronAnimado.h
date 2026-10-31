#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <array>

// Cada esquina conserva la geometria exportada en coordenadas globales.
// Corregimos el pivote con T(pivote)*R*T(-pivote).
class HolocronAnimado {
public:
    void Actualizar(double dt);
    glm::mat4 Base() const;
    glm::mat4 Pieza(int numero) const;
    float Apertura() const;
private:
    double tiempo=0;
    std::array<glm::vec3,8> pivotes = {{
        {-2.60855f,-2.58275f,-2.65127f},
        {-2.62319f,-2.65930f,2.55349f},
        {-2.61322f,2.64211f,-2.54705f},
        {-2.61046f,2.56270f,2.65743f},
        {2.61264f,-2.55904f,-2.65415f},
        {2.59912f,-2.64826f,2.52006f},
        {2.61538f,2.66178f,-2.52808f},
        {2.59038f,2.58028f,2.65560f}
    }};
};
