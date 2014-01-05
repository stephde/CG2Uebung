#version 140

// Task_2_1 - ToDo Begin

uniform mat4 viewProjectionTransform;
uniform mat4 viewProjectionTransformInv;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
uniform mat4 viewTransformInv;
uniform mat4 projectionTransformInv;
uniform mat4 transform;
uniform vec3 camPos;



in vec2 a_vertex;

out vec3 v_eye;
 
void main()
{	
	// ToDo: use the given xy coordinates to retrieve 
	// the vertices eye vector (basically the view frustums edges
	// pointing towards +z) that need to be passed to
	// the fragment stage for accessing the various projection
	// mappings. 
		
	v_eye = ( projectionTransformInv *  vec4(a_vertex, 1.0, 1.0)* viewTransform ).xyz;
// - camera.eye * projection

	// Task_2_1 - ToDo End
	
	// Note: z is already at z = 1.0 (back plane in NDC)
	
	gl_Position = vec4(a_vertex, 1.0, 1.0);
}
  