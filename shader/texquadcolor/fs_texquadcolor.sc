$input v_texcoord0,v_color0


#include "common.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
    vec4 sample = mul(texture2D(s_texColor, v_texcoord0),v_color0);
    gl_FragColor = vec4(sample.rgb,1.0);
}
