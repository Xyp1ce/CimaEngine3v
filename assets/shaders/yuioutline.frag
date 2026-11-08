#version 110
// Dibuja el frame del sprite con un resplandor pulsante alrededor de la silueta
// y un borde interior  (como si el aura iluminara al personaje).

uniform sampler2D texture;
uniform vec4  frameRect;     // uv mínimo (0) y máximo (1) del frame actual
uniform vec2  texel;         // 1.0 / tamaño de la textura
uniform float time;
uniform vec3  glowColor;
uniform float glowRadius;    // en píxeles de la textura
uniform float glowStrength;  // 0 = sin efecto
uniform float rimStrength;

float alphaAt(vec2 uv)
{
    vec2 inside = step(frameRect.xy, uv) * step(uv, frameRect.zw);
    return texture2D(texture, uv).a * inside.x * inside.y;
}

// ruido barato para que el resplandor "flamee" en vez de ser un borde fijo
float hash(vec2 p) { return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); }
float noise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i),                  hash(i + vec2(1.0, 0.0)), f.x),
            mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);
}

void main()
{
    vec2 uv   = gl_TexCoord[0].xy;
    vec4 base = texture2D(texture, uv);
    float a   = alphaAt(uv);
    float pulse = 0.5 + 0.5 * sin(time * 3.0);
    float r     = glowRadius * (0.8 + 0.2 * pulse);
    // 16 direcciones x 2 distancias (los loops deben tener límites constantes en GLSL 1.10)
    float outer = 0.0;
    float inner = 0.0;
    for (int i = 0; i < 16; i++)
    {
        float ang = float(i) * 0.3926991;            // 2*pi / 16
        vec2  dir = vec2(cos(ang), sin(ang)) * texel;
        outer += alphaAt(uv + dir * r);
        outer += alphaAt(uv + dir * r * 0.5);
        inner += alphaAt(uv + dir * 3.0);
    }
    outer /= 32.0;
    inner /= 16.0;
    // el resplandor sube y se deforma con el tiempo
    vec2  px    = uv / texel;
    float flame = noise(vec2(px.x * 0.04, px.y * 0.025 + time * 2.5));
    float glow  = outer * (0.55 + 0.9 * flame) * (0.75 + 0.25 * pulse) * glowStrength;
    glow = clamp(glow, 0.0, 1.0) * (1.0 - a);
    // borde interior: píxeles del sprite que tienen vecinos transparentes
    float rim = a * (1.0 - inner) * rimStrength * (0.7 + 0.3 * pulse);
    vec3 spriteCol = mix(base.rgb, glowColor + 0.3, clamp(rim, 0.0, 0.85));

    // composición "sprite sobre resplandor" (alfa no premultiplicado)
    float outA = a + glow;
    vec3  outC = (spriteCol * a + glowColor * glow) / max(outA, 0.0001);

    gl_FragColor = vec4(outC, outA) * gl_Color;
}
