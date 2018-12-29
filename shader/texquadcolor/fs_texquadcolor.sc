$input v_texcoord0,v_color0


#include "common.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
        vec4 sample = texture2D(s_texColor, v_texcoord0);
	gl_FragColor = mul(v_color0,vec4(sample.rgb,1.0));
}
