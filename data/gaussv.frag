#version 150

uniform sampler2D source;
uniform vec2 size;

in vec2 v_uv;
out vec4 fragColor;

float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );

const float sigma = 4.0;

void main()
{
	float u = v_uv.x;
	float v = v_uv.y;

	// Task_5_4 - ToDo Begin

	vec4 sum = vec4(0.0);// = texture( source, vec2(u,v) ) * weight[0];
    for (int i=0; i<5; i++) {
		sum += texture( source,  vec2(u, v + i)) * weight[i];
    }
    
    fragColor = texture( source, vec2(u,v) );// * sum; 
	
	// Task_5_4 - ToDo End
}
