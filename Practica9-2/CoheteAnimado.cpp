#include "CoheteAnimado.h"
#include <algorithm>
#include <cmath>

// Despegue progresivo: inicio vertical, luego ascenso con curva suave.
namespace {
constexpr float ESPERA_INICIAL = 2.f;
constexpr float ASCENSO = 17.f;
constexpr float PAUSA_FINAL = 3.f;
constexpr float CICLO = ESPERA_INICIAL + ASCENSO + PAUSA_FINAL;

float TiempoDeVuelo(double tiempo) {
    float t = std::clamp(float(tiempo) - ESPERA_INICIAL, 0.f, ASCENSO);
    // En los ultimos dos segundos reduce suavemente la velocidad hasta detenerse.
    if (t <= 15.f) return t;
    float u = (t - 15.f) / 2.f;
    return 15.f + 2.f * (u + u*u - u*u*u);
}
float AvanceHorizontal(float v) {
    float q = std::max(0.f, v - 4.f);
    return q*q / (1.f + .03f*q);
}
float DerivadaHorizontal(float v) {
    float q = std::max(0.f, v - 4.f);
    float d = 1.f + .03f*q;
    return q*(2.f + .03f*q) / (d*d);
}
}

void CoheteAnimado::Actualizar(double dt) {
    // Al acabar el ascenso, espera unos segundos y vuelve a la base.
    tiempo += std::clamp(dt, 0.0, .08);
    if (tiempo >= CICLO) tiempo = 0.0;
}

glm::vec3 CoheteAnimado::Posicion() const {
    float v = TiempoDeVuelo(tiempo);
    // Aumenta 50% la altura alcanzada, conservando la altura inicial de la base.
    float y = .45f + 1.5f*(1.5f*v + .26f*v*v);
    float horizontal = AvanceHorizontal(v);
    return {65.f + .24f*horizontal, y, -.07f*horizontal};
}

glm::mat4 CoheteAnimado::Matriz() const {
    float v = TiempoDeVuelo(tiempo);
    float derivada = DerivadaHorizontal(v);
    float dx = .24f*derivada, dz = -.07f*derivada, dy = 1.5f*(1.5f + .52f*v);
    // Orientacion segun la tangente de la trayectoria, incluso durante la pausa.
    float giroZ = -std::atan2(dx, dy);
    float giroX = std::atan2(-dz, std::sqrt(dx*dx + dy*dy));
    glm::mat4 m = glm::translate(glm::mat4(1), Posicion());
    m = glm::rotate(m, giroZ, glm::vec3(0,0,1));
    m = glm::rotate(m, giroX, glm::vec3(1,0,0));
    return glm::scale(m, glm::vec3(3.0f));
}
