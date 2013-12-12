#version 140

// Feel free to copy/paste from Task_2_2 or modify...
// Task_2_3 - ToDo Begin

uniform mat4 transform;
uniform mat4 viewProjectionTransform;
uniform vec3 camPos;

in vec3 a_vertex;

out vec3 v_normal;
out vec3 v_eye;

void main()
{
	v_normal = a_vertex;

	// ToDo: Retrive the eye vector and pass to next stage
	
	v_eye = ( transform * (vec4(a_vertex, 1.0))).xyz - camPos;


	gl_Position =  viewProjectionTransform * transform * vec4(a_vertex, 1.0) ;
}

// Task_2_3 - ToDo End