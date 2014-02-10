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
	
	float f = r + pow(1.0 - VdotH, 5) * (1.0 - r);
	
	return f;
	
	// Task_5_2 - ToDo End
}

// Beckmann's distribution for roughness
float roughness(in float NdotH, in float r)
{
	// r: roughness
	// Task_5_2 - ToDo Begin
	float NdotH_2 = NdotH * NdotH;
	float r_2 = r * r;

	float v = exp((NdotH_2 - 1.0) / (r_2 * NdotH_2)) / (4.0 * r_2 * NdotH_2 * NdotH_2) ;
		
	return v;
	
	// Task_5_2 - ToDo End
}

// Geometric attenuation accounts for the shadowing and 
// self masking of one microfacet by another.
float geom(in float NdotH, in float NdotV, in float VdotH, in float NdotL)
{
	// Task_5_2 - ToDo Begin
	
	float g1 = (2.0 * NdotH * NdotV) / VdotH;
	float g2 = (2.0 * NdotH * NdotL) / VdotH;
	
	float g = min(min(g1, g2), 1.0);
	
    return g;
	
	// Task_5_2 - ToDo End
}

vec3 CookTorrance(in vec3 V, in vec3 n, in vec3 L, in Material m, in vec3 R, in vec3 ambient)
{
	vec3 H = normalize(L + V);
	
	vec3 N = normalize(-n);

	float VdotH = dot(V, H);
	float NdotV = dot(N, V);
	float NdotH = dot(N, H);
	float NdotL = dot(N, L);

	// Task_5_2 - ToDo Begin
	
	// hint: R is reflection (e.g., ray in envmap)
	float rs =  	geom(NdotH, NdotV, VdotH, NdotL)
				*   fresnel(VdotH, m.sr.w) 
				*   roughness(NdotH, m.dr.w*2)
				/ 	(NdotV * NdotL);

	vec3 r = NdotL * ((rs + m.sr.xyz) * m.dr.xyz); // mix(NdotL * ((rs + m.sr.xyz) * m.dr.xyz), (m.sr.xyz * m.dr.xyz));	

	

	return r;

	// Task_5_2 - ToDo End
}
