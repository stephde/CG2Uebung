#version 150

const float EPSILON  = 1e-6;

struct Material
{
	vec4 sr; // vec3 specular, float reflectance
	vec4 dr; // vec3 diffuse, float roughness
};

// Schlick's Approximation of the Fresnel Factor
float fresnel(in float VdotH, in float r)
{
	// r: reflectance
	// Task_5_2 - ToDo Begin
	
	float f = r + pow(1 - VdotH, 5) * (1 - r);
	
	return f; 
	
	// Task_5_2 - ToDo End
}

// Beckmann's distribution for roughness
float roughness(in float NdotH, in float r)
{
	// r: roughness
	// Task_5_2 - ToDo Begin

	float v = 1 / (r*r * pow (NdotH, 4)) * exp( (NdotH * NdotH -1) / (r*r * NdotH*NdotH));
		
	return v;
	
	// Task_5_2 - ToDo End
}

// Geometric attenuation accounts for the shadowing and 
// self masking of one microfacet by another.
float geom(in float NdotH, in float NdotV, in float VdotH, in float NdotL)
{
	// Task_5_2 - ToDo Begin

	float g = min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH);
	
    return g;
	
	// Task_5_2 - ToDo End
}

vec3 CookTorrance(in vec3 V, in vec3 N, in vec3 L, in Material m, in vec3 R, in vec3 ambient)
{
	vec3 H = normalize(L + V);

	float VdotH = clamp(dot(V, H), 0.0, 1.0);
	float NdotV = clamp(dot(N, V), 0.0, 1.0);
	float NdotH = clamp(dot(N, H), 0.0, 1.0);
	float NdotL = clamp(dot(N, L), 0.0, 1.0);

	// Task_5_2 - ToDo Begin
	
	// hint: R is reflection (e.g., ray in envmap)
	float rs =  	geom(NdotH, NdotV, VdotH, NdotL) 
				*   fresnel(VdotH, m.sr.w) 
				*   roughness(NdotH, m.sr.w) 
				/ 	(NdotV * NdotL);

	vec3 r = /*ambient */ dot(N,L) * (m.sr.xyz * rs + m.dr.xyz);	

	

	return r;// * rs + r;

	// Task_5_2 - ToDo End
}
