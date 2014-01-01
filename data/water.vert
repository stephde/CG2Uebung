#version 140

// Task_1_4 - ToDo Begin

uniform mat4 transform;
uniform sampler2D height;
uniform sampler2D water;
uniform sampler2D waterHeights;
uniform float timef;

in vec3 a_vertex;
 
out float h;
out float waterH;
out vec2 texCoord;
out vec4 pos;
//out int bDiscard;

void main()
{
	h = texture(height, vec2(a_vertex.x, a_vertex.z)).x;

	waterH = texture(waterHeights, vec2(a_vertex.x/256.0 + timef/10, a_vertex.z/256.0 + timef/100)).x;

	pos = vec4( a_vertex.x/64.0, 
					- 0.3 + mix( (100-mod(timef*100,100))/1200, 
							mod(timef*100,100)/1200, 
							step(mod(timef*100,100), 50))
					+ waterH/20, 
					a_vertex.z/64.0, 1.0);
	
	//bDiscard = h >= pos.y ? 1 : 0; 

	gl_Position = transform * pos;
	texCoord = vec2(a_vertex.x/64, a_vertex.z/64);
	
	// Task_1_4 - ToDo End
}
