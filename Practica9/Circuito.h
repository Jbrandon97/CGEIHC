#pragma once
#include <glm.hpp>
#include <vector>
#include "Mesh_tn.h"

struct PuntoRuta { glm::vec3 posicion, frente, derecha, arriba; };
class Circuito {
public:
    static constexpr double radio=25, ancho=14;
    static constexpr double longitudPlana=380+50*3.14159265358979323846;
    Circuito();
    PuntoRuta Evaluar(double parametro) const;
    PuntoRuta PorDistancia(double distancia) const;
    double DistanciaEn(double parametro) const;
    double Longitud() const { return acumuladas.back(); }
    void CrearMallas();
    MeshModel superficie,bordeInterior,bordeExterior,piso,arco;
    bool Verificar() const;
private:
    std::vector<double> parametros,acumuladas;
    void Franja(MeshModel&,float,float,float);
};
