#version 150

const float INFINITY = 1e+4;
const int SIZE = 10;
const float THRESHOLD = 0.66;

struct Sphere
{
	vec3 position;
	float radius;
};
struct Material
{
	vec4 sr; // vec3 specular, float reflectance
	vec4 dr; // vec3 diffuse, float roughness
};
struct Ray
{
	vec3 origin;
	vec3 direction;
};


Sphere blobs[16];
Material materials[16];

void cache(
	sampler2D positions
,	sampler2D materials0
,	sampler2D materials1)
{
	for(int i = 0; i < SIZE; ++i)
	{
		ivec2 uv = ivec2(i % 4, i / 4);
	
		vec4 v = texelFetch(positions, uv, 0);
		
		Sphere blob;
		blob.position = v.xyz;
		blob.radius = v.w;

		blobs[i] = blob;
		
		Material mat;
		mat.sr = texelFetch(materials0, uv, 0);
		mat.dr = texelFetch(materials1, uv, 0);
		
		materials[i] = mat;
	}
}

// sphere intersection
bool intersect(
    const in Sphere blob
,   const in Ray ray // needs to be normalized!
,   out float t0
,   out float t1)
{
	// Task_5_1 - ToDo Begin
	// implement a ray sphere intersection

	// hint: the case of single intersection with sphere can be neglected, so
	// only t > 0.0 intersections are relevant (if you like you can also
	// account for t == 0.0 intersections, which should have a rare occurrence).

	// hint: use ray.origin, ray.direction, blob.position, blob.radius
	// hint: make it fast (if you like)! ;)

	// ...
	
	//http://www.ccs.neu.edu/home/fell/CSU540/programs/RayTracingFormulas.htm
	//http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection
	//Compute A, B and C coefficients
    float a = dot(ray.direction, ray.direction);
    float b = 2 * dot(ray.direction, ray.origin - blob.position);
    float c = dot(blob.position, blob.position) + dot(ray.origin, ray.origin) 
				- 2 * dot(blob.position, ray.origin) - (blob.radius * blob.radius);

    //Find discriminant
    float disc = b * b - 4 * a * c;
    
    // if discriminant is negative there are no real roots, so return 
    // false as ray misses sphere
    if (disc < 0)
        return false;

    // compute q as described above
    float distSqrt = sqrt(disc);
    float q;
    if (b < 0)
        q = (-b - distSqrt)/2.0;
    else
        q = (-b + distSqrt)/2.0;

    // compute t0 and t1
    t0 = q / a;
    t1 = c / q;

    // make sure t0 is smaller than t1
    if (t0 > t1)
    {
        // if t0 is bigger than t1 swap them around
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // if t1 is less than zero, the object is in the ray's negative direction
    // and consequently the ray misses the sphere
    if (t1 < 0)
        return false;

	// Task_5_1 - ToDo End
	
	return true;
}

bool rcast(in Ray ray, out vec3 normal, out Material material, out float t)
{	
    // Task_5_1 - ToDo Begin
	
	// return normal for the nearest intersected sphere, as well as
	// the intersection parameter t (that should allow to retrieve the 
	// itnersection point I = ray.origin + ray.direction * t).
	// the function should return true, if at least one sphere was hit, 
	// and false if no when no sphere was hit.
	// (Task_5_2 - ToDo return material of nearest intersected sphere)

	t =  INFINITY;

	for(int i = 0; i < SIZE; ++i)
	{
		float t0; // = ?
		float t1; // = ?
	
		// ...

		if(intersect(blobs[i], ray, t0, t1) /* todo, more? */)
		{
			t = t0;
			vec3 ip = ray.origin + ray.direction * t0;
			normal = blobs[i].position - ip;
			// Task 5_2: 
			material = materials[i];
			return true;
		}
	}
	return false; // ? 
	// ToDo: End Task 5_1
}

bool rcast(in Ray ray, out float tmin, out float tmax, out float[SIZE] intersected)
{
	float t0 , t1;
	tmin = INFINITY, tmax = 0.0;
	bool intersection = false;

	for(int i = 0; i < SIZE; ++i)
	{
		intersected[i] = 0;
		if(intersect(blobs[i], ray, t0, t1))
		{
			if(tmin > t0) tmin = t0;
			if(tmax < t1) tmax = t1;
			intersection = true;
			intersected[i] = 1;
		}
	}

	return intersection;
}

// Task_5_3 - ToDo Begin

float energyAtPoint(in vec3 p, in float[SIZE] intersected)
{
	float sum = 0.0;
	for(int i=0; i< SIZE; i++)
	{
		//float value = 1 / length((p - blobs[i].position)*(p - blobs[i].position));
		//if(value > THRESHOLD)
			//sum += value;
		sum += intersected[i] * smoothstep(blobs[i].radius, 0.0, length(p - blobs[i].position));
	}

	return sum;
}



void interpolate(in float e, in vec3 p, out vec3 n, out Material m)
{
	vec3 tn;
	float energyPercentage, distance;
	for(int i=0; i < SIZE; ++i)
	{
		tn = normalize( abs(p - blobs[i].position));
		distance = length(p - blobs[i].position);
		energyPercentage = smoothstep(blobs[i].radius, 0, distance) / e;
		n += tn * energyPercentage;
		m.sr += materials[i].sr * energyPercentage;
		m.dr += materials[i].dr * energyPercentage;
	}
}

bool trace(in Ray ray, out vec3 normal, out Material material, out float t)
{
	// Task_5_3 - ToDo Begin
	// find nearest and farthest intersection for all metaballs 
	// hint: use for loop, INFINITE, SIZE, intersect, min and max...

	// ...
	float tmin;
	float tmax;
	float step = 0.001;

	float[SIZE] intersected;
	float energy;
	vec3 p;
	rcast(ray, tmin, tmax, intersected);

	while( tmin <= tmax)
	{
		p = ray.origin + tmin*ray.direction;
		energy = energyAtPoint(p, intersected);

		if(energy >= THRESHOLD)
		{
			interpolate(energy, p, normal, material);
			t = tmin;
			return true;
		}

		tmin += step + (THRESHOLD - energy) / 4;
	}

	// your shader should terminate!
	// return true if iso surface was hit, false if not
	return false; //intersection; 
}

// Task_5_3 - ToDo End
