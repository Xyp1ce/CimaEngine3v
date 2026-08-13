#version 110

uniform sampler2D textura;
uniform vec4 color;

void main()
{
    vec4 pixeles = texture2D(textura, gl_TexCoord[0].xy);
    gl_FragColor = pixeles + color;
}

