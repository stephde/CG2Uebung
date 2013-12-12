#version 140

// Task_1_3 - ToDo Begin

uniform mat4 transform;
uniform sampler2D height;

in vec3 a_vertex;

out float h;
out vec2 texCoord;
out vec3 normal;
out float shadow;

void main()
{
	// Note: should be similar to 1_2 vertex shader.
	// In addition, you need to pass the texture coords 
	// to the fragment shader...

	// ...
	

	vec4 v = texture(height, vec2(a_vertex.x/256, a_vertex.z/256));
	h = v.z;
	gl_Position = transform * vec4(a_vertex.x/300.0, h, a_vertex.z/300.0, 1.0);
	texCoord = vec2(a_vertex.x/64, a_vertex.z/64);

	
	float s = 0.003;

	vec4 h1 = texture(height, vec2((a_vertex.x), (a_vertex.z)));
	vec4 h2 = texture(height, vec2((a_vertex.x+s), (a_vertex.z)));
	vec4 h3 = texture(height, vec2((a_vertex.x), (a_vertex.z+s)));
	vec3 pos1 = vec3(a_vertex.x, h1.x, a_vertex.z);
	vec3 pos2 = vec3(a_vertex.x+s, h2.x, a_vertex.z);
	vec3 pos3 = vec3(a_vertex.x, h3.x, a_vertex.z+s);

	vec3 v1 = normalize(pos2 - pos1);
	vec3 v2 = normalize(pos3 - pos1);

	normal = normalize((cross(v1, v2)));
	shadow = abs(normal.y);
	
	// Task_1_3 - ToDo End
}
