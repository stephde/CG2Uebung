#version 140

uniform sampler2D envmap;
uniform samplerCube cubemap;

uniform int mapping;

out vec4 fragColor;

in vec3 v_eye;

// Task_2_1 - ToDo Begin
// Implement the four requested projection mappings.
 
const float c_pi = 3.1415926;

vec4 env(in vec3 eye)
{
	vec4 color;
	
	if(0 == mapping) 		// cube
	{
		// ToDo: use texture function with the cubemap sampler
		// vec4(0.0, 0.0, 0.0, 1.0);
		
		color = texture(cubemap, eye);
	}
	else if(1 == mapping) 	// polar
	{	
		// ToDo: use texture function with the envmap sampler
		// vec4(0.0, 1.0, 0.0, 1.0);
		
		// clamp so there wont be sharp color changes on the horizon
		vec2 uv = vec2(0.5*(1/c_pi)*atan(eye.x, eye.z), -1*clamp((2/c_pi)*asin(eye.y), 0.001, 1.0)) ;
		color = texture(envmap, uv);
	}	
	else if(2 == mapping) 	// paraboloid
	{
		// ToDo: use texture function with the envmap sampler
		// vec4(0.0, 0.0, 1.0, 1.0);
		
		float m = 2.0 + 2.0 * eye.y;
		vec2 uv = vec2(0.5 + eye.x*(1/m), 0.5 + eye.z*(1/m));
		if(eye.y>0.0) color = texture(envmap, uv);
	}
	else if(3 == mapping) 	// sphere
	{
		// ToDo: use texture function with the envmap sampler
		// vec4(1.0, 1.0, 0.0, 1.0);
		
		float m = 2.0*sqrt((eye.x*eye.x) + (eye.y*eye.y) + (1.0 - eye.z)*(1.0 - eye.z)); // x*x != pow(x, 2) WTF?
		vec2 uv = vec2(1.0 - (0.5 + eye.x*(1/m)), 1.0 - (0.5 + eye.y*(1/m)));
		color = texture(envmap, uv);		
	}
	return color;
}

// Task_2_1 - ToDo End

void main()
{
	vec3 eye = normalize(v_eye);
	vec4 color = env(eye);
	
	fragColor = color;
}
