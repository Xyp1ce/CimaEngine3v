uniform float wave_phase;
uniform vec2 wave_amplitude;

void main()
{
    vec4 vertex = gl_Vertex;
    vertex.x += cos(gl_Vertex.y * 0.12 + wave_phase * 3.8) * wave_amplitude.x
              + sin(gl_Vertex.y * 0.12 + wave_phase * 6.3) * wave_amplitude.x * 3.0;
    vertex.y += sin(gl_Vertex.x * 0.12 + wave_phase * 3.8) * wave_amplitude.y
              + cos(gl_Vertex.x * 0.12 + wave_phase * 6.3) * wave_amplitude.y * 3.0;

    gl_Position = gl_ModelViewProjectionMatrix * vertex;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
