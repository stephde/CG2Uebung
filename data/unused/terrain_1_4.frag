#version 140

uniform sampler2D ground;
uniform sampler2D caustics;
uniform float timef;


in float h;
in vec2 texCoord;
out vec4 fragColor;

// Task_1_4 - ToDo Begin

// Note: start with a copy of your 1_3 shader and add the lambert term 
// based on a static light source (use const variable)...

void main()
{
	//fragColor = lambert * /*as before ... */;

	vec3 uv0 = vec3(fract(texCoord.x)/4+0.75, texCoord.y, 0.0);
	vec3 uv1 = vec3(fract(texCoord.x)/4+0.5, texCoord.y, 0.25);
	vec3 uv2 = vec3(fract(texCoord.x)/4+0.25, texCoord.y, 0.5);
	vec3 uv3 = vec3(fract(texCoord.x)/4, texCoord.y, 0.75);

	vec3 uvA = mix( mix(uv2, uv1, step(h, 0.5)), uv0, step(h, 0.3));
	vec3 uvB = mix( mix(uv3, uv2, step(h, 0.5)), uv1, step(h, 0.33));

	float i =(h-uvA.z);
	
	if( h> 0.2 && h<0.3)i = smoothstep(0.15, 0.25, h);
	if( h>= 0.3 && h < 0.5) i = smoothstep(0.35, 0.45, h);
	if( h>=0.6) i = smoothstep(0.65, 0.75, h);
	
	//float light = dot(normal, vec3(0.0, 1.0, 0.0));
	
	fragColor = mix(texture(ground, vec2(uvA.x, uvA.y)), texture(ground, vec2(uvB.x, uvB.y)), i);

	
	if(h < 0.3) fragColor = mix(	fragColor
					,texture(caustics, vec2(uvB.x/4 + timef/2, uvA.y/4)/2)
					,smoothstep(0.35,0.005,h));

	
	
	
	// Task_1_4 - ToDo End
}
