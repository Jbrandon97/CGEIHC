#include "HolocronAnimado.h"
#include <cmath>
#include <algorithm>
namespace {
constexpr float PI=3.14159265359f;
float Suave(float t){t=std::clamp(t,0.f,1.f);return t*t*(3-2*t);}
}
void HolocronAnimado::Actualizar(double dt){tiempo+=std::clamp(dt,0.0,.08);}
float HolocronAnimado::Apertura() const{
    // Primero se despegan, despues giran separados y finalmente regresan.
    float t=float(std::fmod(tiempo,11.0));
    if(t<2.f)return Suave(t/2.f);
    if(t<7.f)return 1.f;
    if(t<9.f)return 1.f-Suave((t-7.f)/2.f);
    return 0.f;
}
glm::mat4 HolocronAnimado::Base() const{
    glm::mat4 m=glm::translate(glm::mat4(1),glm::vec3(-65,23,0));
    m=glm::rotate(m,float(tiempo*.15),glm::vec3(0,1,0));
    return glm::scale(m,glm::vec3(1.45f));
}
glm::mat4 HolocronAnimado::Pieza(int numero) const{
    // Se conserva el giro de P05: cada esquina rota sobre el eje diagonal
    // que une el centro del holocron con la punta de esa esquina.
    // Solo agregamos separacion radial para que se desprenda antes de girar.
    float apertura=Apertura();
    float t=float(std::fmod(tiempo,11.0));
    glm::vec3 eje=glm::normalize(pivotes.at(numero));

    // 0-2 s: separacion; 2-5 s: vuelta; 7-9 s: reunion.
    float giro=2.f*PI*Suave((t-2.f)/3.f);
    glm::mat4 m=glm::translate(glm::mat4(1),eje*(3.5f*apertura));
    // El giro es alrededor del eje local propio, NO sobre Y global.
    return glm::rotate(m,giro,eje);
}
