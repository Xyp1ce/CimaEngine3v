#version 120
uniform sampler2D textura;
// posición de la lampara en [0,1] relativas a la textura
// hay que dividir entre el tamaño de la textura
uniform vec2 lampPos;
// radio de la lampara
uniform float lampRadio;
// un valor pequeño
uniform float lampSoft;

uniform vec4 fogColor;
uniform float tiempo;

//configuracion de la niebla
uniform float ruido_escala;
uniform float ruido_min; 
uniform float ruido_max;

uniform float opacidad_offset;


float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9090, 78.233)))*43758.5453123 + tiempo);
}

//ruido  (la niebla)
float ruido(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);
    //interpolación
    float a = random(i);
    float b = random(i + vec2(1.0,0.0) );
    float c = random(i + vec2(0.0,1.0) );
    float d = random(i + vec2(1.0,1.0) );

    //vec2 u = f*f*(3.0-2.0*f);
    vec2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    return mix(a,b,u.x) +(c-a)*u.y*(1.0-u.x)+(d-b)*u.x*u.y;
}

void main()
{
    vec4 pixeles = texture2D(textura,gl_TexCoord[0].st);

    //distancia de lampara a la textura
    float dist  = distance(gl_TexCoord[0].st, lampPos);
    
    //0 = visible; 1=niebla
    float visibilidad = smoothstep(lampRadio - lampSoft, lampRadio+lampSoft,dist);
    

    float v_ruido = ruido(gl_TexCoord[0].st*ruido_escala);
    v_ruido = smoothstep(ruido_min,ruido_max,v_ruido);


    //opacidad
    float fogOpacidad = visibilidad * (fogColor.a + v_ruido * opacidad_offset);
    //valores entre 0 y 1 cerrados
    fogOpacidad = clamp(fogOpacidad,0.0,1.0);

    //color
    vec4 color = mix(pixeles, fogColor, fogOpacidad);
    color.a = mix(pixeles.a,fogColor.a,fogOpacidad);

    gl_FragColor = color;

}
