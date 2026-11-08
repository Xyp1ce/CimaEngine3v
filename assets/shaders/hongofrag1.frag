#version 110
uniform sampler2D texture;
uniform vec3 colorTarget;
uniform float tolerancia;
uniform vec2 texel;
uniform float tiempo;
uniform float velocidad;
uniform float maxAltura;
uniform vec4 tinta;
uniform vec4 frame_real;
const int LAYERS = 10; //columnas
const float semilla = 43758.5453;

vec4 frameSample(vec2 uv)
{
    vec2 adentro = step(frame_real.xy,uv)*step(uv,frame_real.zw);
    return texture2D(texture,uv)*adentro.x*adentro.y;
}
float seleccionar(vec4 c)
{
    float distancia = distance(c.rgb, colorTarget);
    float paso = step(0.5,c.a);
    float lerp = smoothstep(tolerancia*0.5, tolerancia, distancia);
    return (1.0 - lerp)*paso;
}
//funcion random
float hash(float n){ return fract(sin(n)*semilla); }
void main()
{
    // lookup the pixel in the texture
    vec2 uv = gl_TexCoord[0].xy;
    vec4 base = frameSample(uv);
    vec4 subir = vec4(0.0);
    //columnas de 5 pixeles
    float columna = floor(uv.x/texel.x/5.0);
    for (int i=0; i<LAYERS; i++)
    {
        //numeros random
        float fase = hash(columna*13.7+float(i)*7.1);
        float dt = fract(tiempo*velocidad+fase+float(i)/float(LAYERS));
        float dist = dt*maxAltura;
        //buscar dist pixeles abajo
        vec4 buscar = texture2D(texture, uv + vec2(0.0, dist) * texel);
        float es = seleccionar(buscar)* (1.0-dt)*(1.0*dt);
        if (es > subir.a)
        {
            subir = vec4(mix(buscar.rgb,tinta.rgb,tinta.a),es);
        }
    }
    //la base siempre se pinta primero
    vec3 col = mix(subir.rgb,base.rgb,base.a);
    float alpha = base.a+subir.a*(1.0-base.a);
    gl_FragColor = vec4(col,alpha)*gl_Color;
}
