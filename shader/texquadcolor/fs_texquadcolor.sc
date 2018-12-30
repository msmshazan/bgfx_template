$input v_texcoord0,v_color0


#include "common.sh"

SAMPLER2D(s_texColor, 0);

vec4 toGrayscale(vec4 color)
{
  float average = (color.r + color.g + color.b) / 3.0;
  return vec4(average, average, average, 1.0);
}

vec4 colorize(vec4 grayscale, vec4 color)
{
    return (grayscale * color);
}

void main()
{
    vec4 texel = texture2D(s_texColor, v_texcoord0);
    vec4 sample = texel*v_color0;
    gl_FragColor = vec4(sample.rgb ,1.0);
}
