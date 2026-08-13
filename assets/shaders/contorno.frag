#version 110


uniform sampler2D uTex;

uniform vec4 uOutlineColor;
//uniform float uEdgeThreshold;
//uniform float uEdgeSoftness;
uniform float uThickness;
uniform vec2 uTexSize;

void main()
{
    vec2 texel = uThickness/uTexSize;
    vec2 vUV = gl_TexCoord[0].xy;

// Lecturas del canal alfa en un vecindario 3x3
    vec4 a00 = texture2D(uTex, vUV + texel * vec2(-1.0, -1.0));
    vec4 a10 = texture2D(uTex, vUV + texel * vec2( 0.0, -1.0));
    vec4 a20 = texture2D(uTex, vUV + texel * vec2( 1.0, -1.0));

    vec4 a01 = texture2D(uTex, vUV + texel * vec2(-1.0,  0.0));
    vec4 a11 = texture2D(uTex, vUV + texel * vec2( 0.0,  0.0));
    vec4 a21 = texture2D(uTex, vUV + texel * vec2( 1.0,  0.0));

    vec4 a02 = texture2D(uTex, vUV + texel * vec2(-1.0,  1.0));
    vec4 a12 = texture2D(uTex, vUV + texel * vec2( 0.0,  1.0));
    vec4 a22 = texture2D(uTex, vUV + texel * vec2( 1.0,  1.0));

    // Sobel sobre alfa
    float gx = (-1.0 * a00.r) + (1.0 * a20.r) +
               (-20.0 * a01.r) + (20.0 * a21.r) +
               (-1.0 * a02.r) + (1.0 * a22.r);

    float gy = (-1.0 * a00.r) + (-1.0 * a10.r) + 
               (-20.0 * a20.r) + (20.0 *  a02.r) + 
               (1.0 * a12.r)  + (1.0 *  a22.r);

    float magnitude = sqrt(gx * gx + gy * gy);

    vec4 color = vec4(vec3(magnitude),1.0);
    if (color.r == 0.0 && color.g == 0.0 && color.b==0.0 )
        color.a=0.0;
    gl_FragColor = color*uOutlineColor;

}
