#version 140

// Task_2_2 - ToDo Begin

// uniform mat4 ...;
// ...

uniform mat4 transform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
uniform mat4 viewTransformInv;
uniform mat4 projectionTransformInv;

in vec3 a_vertex;

out vec3 v_normal;
out vec3 v_eye;

void main()
{
	v_normal = a_vertex;

	// ToDo: Retrive the eye vector and pass to next stage
	
	v_eye = ( projectionTransformInv * viewTransformInv * (vec4(a_vertex, 1.0))).xyz;


	gl_Position = transform * vec4(a_vertex, 1.0);
}

// Task_2_2 - ToDo End