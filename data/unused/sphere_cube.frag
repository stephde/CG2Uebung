#version 140

uniform samplerCube source;

uniform float timef;

out vec4 fragColor;

in vec3 v_normal;
in vec3 v_eye;

// Task_2_3 - ToDo Begin


vec4 env(in vec3 eye)
{	
	// use texture function with the cubemap sampler
	return texture(source, eye); 
}

void main()
{
	vec3 n = normalize(v_normal);
	vec3 e = normalize(v_eye);
	
	vec3 r = reflect(vec3(e.x, e.y, e.z), n);
	vec3 q = refract(e, n, 0.97);
	
	float frsl = abs(dot(e,n));

	vec4 refl = env(r);
	vec4 refr = env(q);
	
	fragColor = mix(refr, refl, 1-frsl);//vec4(0.5,0.5,0.5,1.0);
}
// Task_2_3 - ToDo End