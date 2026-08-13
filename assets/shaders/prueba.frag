#version 110
uniform sampler2D textura;
uniform sampler2D blob;
uniform float timer_frame;
uniform vec4 mask_color;
void main()
{
    // lookup the pixel in the texture
    float escala_b = 1.0;
    vec4 pixel = texture2D(textura, gl_TexCoord[0].xy);
    vec4 pixel_b = texture2D(blob,gl_TexCoord[0].xy*escala_b);
    
    float factor = cos(timer_frame);
    vec3 rayas = 1.0-vec3(pixel_b*mask_color)*factor;
    vec4 color_final = (pixel)*(vec4(rayas,pixel.a));
    
    // multiply it by the color
    gl_FragColor = color_final;
}
