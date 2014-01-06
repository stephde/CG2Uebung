#version 140

uniform sampler2D water;

//in float h;
in float waterH;
in vec2 texCoord;
in vec4 pos; 
out vec4 fragColor;


void main()
{
	//fragColor = lambert * /*as before ... */;
	//if(h <= pos.y) discard;
	vec4 t = texture(water, vec2(texCoord.x, texCoord.y));
	vec3 color = mix(vec3(t.x, t.y, t.z), vec3(1.0), smoothstep(0.1, 1.25, waterH));
	fragColor = vec4(color,  0.55 );//smoothstep(0.0, 0.5, h+0.25));
}
