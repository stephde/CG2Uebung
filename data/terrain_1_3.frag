#version 140

uniform sampler2D ground;

in float h;
in vec2 texCoord;
in vec3 normal;
in float shadow;
out vec4 fragColor;

// Task_1_3 - ToDo Begin

// Note: you can specify consts instead of uniforms or local variables...


void main()
{
	// Implement height based texturing using a texture atlas
	// with known structure ;)
	
	// Tip: start with getting the height from the vertex stage 
	// and think of a function that yields you the two texture "indices".
	// These "indices" can be used as offset when accessing the texture...
	// Tip: again, checkout step, clamp, mod, and smoothstep!

	// ...
	
	// Note: its ok if you habe minimal seams..., you need not to apply
	// mipmaps not to use safe margins to avoid those seems. 
	
	// ...
	
	// Note: feel free to scale the textures as it pleases you.
	
	// ...
	
	//float i = ... ;
	//fragColor = mix(texture2D(ground, uv0), texture2D(ground, uv1), i);

 	vec3 uv0 = vec3(fract(texCoord.x)/4+0.75, texCoord.y, 0.0);
	vec3 uv1 = vec3(fract(texCoord.x)/4+0.5, texCoord.y, 0.25);
	vec3 uv2 = vec3(fract(texCoord.x)/4+0.25, texCoord.y, 0.5);
	vec3 uv3 = vec3(fract(texCoord.x)/4, texCoord.y, 0.75);

	vec3 uvA = mix( mix(uv2, uv1, step(h, 0.5)), uv0, step(h, 0.3));
	vec3 uvB = mix( mix(uv3, uv2, step(h, 0.5)), uv1, step(h, 0.33));

	float i =(h-uvA.z)*2;
	
	if( h> 0.2 && h<0.3)i = smoothstep(0.15, 0.25, h);
	if( h>= 0.3 && h < 0.5) i = smoothstep(0.35, 0.45, h);
	if( h>=0.6) i = smoothstep(0.65, 0.75, h);
	
	float light = dot(normal, vec3(0.0, 1.0, 0.0));
	
	fragColor = vec4(vec3(shadow), 1.0) * mix(texture(ground, vec2(uvA.x, uvA.y)), texture(ground, vec2(uvB.x, uvB.y)), i);

	// Task_1_3 - ToDo End
}


