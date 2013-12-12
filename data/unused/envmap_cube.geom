#version 150

// Task_2_3 - ToDo Begin

uniform mat4 views[6];
uniform mat4 projinvs[6];

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec3 g_eye;

void main()
{
	// ToDo: for each cubemap face

	// use the gl_Layer to set the current cube map face to render to
	
	//gl_layer = 1...6;
	// retrieve the g_eye vector and pass to fragment stage
	
	// set  gl_Position, the input is available via 
	// gl_in[0].gl_Position to gl_in[2].gl_Position
	
	for (int i = 0; i < 6; i++) {
		gl_Layer = i;
		for (int j = 0; j < 3; j++) {
			gl_Position = gl_in[j].gl_Position;
			g_eye = (projinvs[i] * gl_Position * views[i]).xyz;
			EmitVertex();
		}
		EndPrimitive();
	}

	// finish up each vertex with EmitVertex();
	// and each primitive with EmitPrimitivie();
}

// Task_2_3 - ToDo End
