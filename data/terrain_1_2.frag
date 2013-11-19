#version 140

uniform sampler2D texSampler;

out vec4 fragColor;

// Task_1_2 - ToDo Begin

in float h;

void main()
{
	// Apply a procedural texture here.

	// Tip: start with getting the height from the vertex stage 
	// and think of a function that yields iso lines.
	// Tip: checkout step, clamp, and mod

	float c = h - step( mod(round(h*1000), 50), 5);

	fragColor = vec4(c, c, c, 1.0);
	
	// Task_1_2 - ToDo End 
}

