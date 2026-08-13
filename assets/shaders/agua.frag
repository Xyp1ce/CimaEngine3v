#version 110
uniform sampler2D textura;
uniform float dt;

uniform float escala_distorcion;
uniform float vel_distorcion;
uniform float frecuencia;

void main()
{
    vec2 p =gl_TexCoord[0].xy;

    float offsetX = sin(p.y*frecuencia +dt*vel_distorcion)*escala_distorcion;
    float offsetY = cos(p.x*frecuencia +dt*vel_distorcion)*escala_distorcion;

    vec2 uv = p+vec2(offsetX,offsetY);

    vec4 color = texture2D(textura,uv);


    gl_FragColor = color;
}
