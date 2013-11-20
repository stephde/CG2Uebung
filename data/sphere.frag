#version 140

uniform sampler2D envmap;
uniform samplerCube cubemap;

uniform int mapping;
uniform float timef;

out vec4 fragColor;

in vec3 v_normal;
in vec3 v_eye;


// Task_2_2 - ToDo Begin

// Copy your env function from 2_1

const float pi = 3.1415926;

vec4 env(in vec3 eye)
{
	vec4 color;
	
	if(0 == mapping) 	// cube
	{
		// use texture function with the cubemap sampler
		color = texture(cubemap, eye);//  // ToDo
	}
	else if(1 == mapping) 	// polar
	{	
		// use texture function with the envmap sampler
		
		vec2 uv = vec2(0.5*(1/pi)*atan(eye.x, eye.y), (1/pi)*acos(-eye.z)) ;

		color = texture(envmap, uv);//vec4(0.0, 1.0, 0.0, 1.0); // ToDo
	}	
	else if(2 == mapping) 	// paraboloid
	{
		// use texture function with the envmap sampler

		float m = 2.0 + 2.0 * eye.z;
		vec2 uv = vec2(0.5 + eye.x*(1/m), 0.5 + eye.y*(1/m));
		color = texture(envmap, uv);//vec4(0.0, 0.0, 1.0, 1.0); // ToDo
	}
	else if(3 == mapping) 	// sphere
	{
		// use texture function with the envmap sampler
		
		float m = 2.0*sqrt(pow(eye.x, 2) + pow(eye.z, 2) + pow(1.0 - eye.y, 2));
		vec2 uv = vec2(0.5 - eye.x*(1/m), 0.5 + eye.z*(1/m));
		color = texture(envmap, uv);//vec4(1.0, 1.0, 0.0, 1.0); // ToDo
	}
	return color;
}

void main()
{
	vec3 n = normalize(v_normal);
	vec3 e = normalize(v_eye);
	
	vec3 r = reflect(e, n);
	vec3 q = refract(e, n, 0.5);
	
	// float frsl = ...;

	vec4 refl = env(r);
	vec4 refr = env(q);
	
	fragColor = refl;//vec4(0.5,0.5,0.5,1.0);
}

// Task_2_2 - ToDo End
