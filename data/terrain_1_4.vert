#version 140

// Task_1_4 - ToDo Begin

uniform mat4 transform;
uniform sampler2D height;
uniform float timef;
uniform int texPrecision;
uniform float cameraPos;

in vec3 a_vertex;

out float h;
out vec2 texCoord;

void main()
{
	// Note: start with a copy of your 1_3 shader and add the lambert term 
	// based on a static light source (use const variable)...

	// ToDo: you need to retrieve a normal, which can be done with 
	// a single cross product of two normalized vectors... so just
	// find two vectors that are sufficient for correct light impression.
	// (It is not required to be absolutely physically correct, but only
	// to depend on the terrains slope.
	float prec =256 * smoothstep(0.0, 1.0, cameraPos/25);
	vec4 v = texture(height, vec2(a_vertex.x/256, a_vertex.z/256.0));
	h = v.z;
	gl_Position = transform * vec4(a_vertex.x/256.0, h, a_vertex.z/256.0, 1.0);
	texCoord = vec2(a_vertex.x/prec, a_vertex.z/prec);
	
	// Task_1_4 - ToDo End
}
