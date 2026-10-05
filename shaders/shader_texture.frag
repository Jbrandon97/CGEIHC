#version 330
in vec4 vColor;
in vec2 TexCoord;
in vec3 Normal;
out vec4 color;
uniform sampler2D theTexture;
void main()
{
    float luz = 0.72 + 0.28 * max(dot(normalize(Normal), normalize(vec3(0.4, 0.8, 0.6))), 0.0);
    color = vColor * texture(theTexture, TexCoord) * vec4(vec3(luz), 1.0);
}
