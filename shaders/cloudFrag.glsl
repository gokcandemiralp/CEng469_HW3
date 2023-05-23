#version 330 core

layout (std140) uniform Matrices{
    mat4 projectionMatrix;
    mat4 viewingMatrix;
    vec3 eyePos;
};

uniform int rayMarchSteps;

in vec4 modelingCoordinate;
out vec4 fragColor;

mat3 m = mat3(0.01, 1.61, 1.19, -1.59, 0.71, -0.95, -1.19, -0.97, 1.27);

float rand(vec3 seed) {
    return fract(sin(dot(seed, vec3(21.3461, 98.3461, 65.3461))) * 8734.61) * 2.0 - 1.0;
}

float perlin(vec3 p) {
    vec3 u = floor(p);
    vec3 v = fract(p);
    vec3 s = smoothstep(0.0, 1.0, v);
    
    float a = rand(u);
    float b = rand(u + vec3(1.0, 0.0, 0.0));
    float c = rand(u + vec3(0.0, 1.0, 0.0));
    float d = rand(u + vec3(1.0, 1.0, 0.0));
    float e = rand(u + vec3(0.0, 0.0, 1.0));
    float f = rand(u + vec3(1.0, 0.0, 1.0));
    float g = rand(u + vec3(0.0, 1.0, 1.0));
    float h = rand(u + vec3(1.0, 1.0, 1.0));
    
    return mix(mix(mix(a, b, s.x), mix(c, d, s.x), s.y),
               mix(mix(e, f, s.x), mix(g, h, s.x), s.y),
               s.z);
}

float triads(vec3 ray) {
    float ans = 0.;
    ans += 0.5714 * perlin(ray);
    ray = m * ray;
    ans += 0.2857 * perlin(ray);
    ray = m * ray;
    ans += 0.14285 * perlin(ray);
    return ans;
}

void main(void){
    vec3 gradient = vec3(modelingCoordinate) - eyePos;
    float depth = 0;
    vec4 sum = vec4(0);
    for(int i = 0 ; i<rayMarchSteps ; ++i){
        vec3 ray = eyePos + depth*gradient;
        if (5 < ray.y && ray.y < 105){
            float density = triads(ray*0.13);
            float altitudeCoefficient = clamp((105-ray.y)*(ray.y-5)*0.001,0,1);
            vec4 color = vec4(mix(vec3(0.5), vec3(1.0), density), density);
            color.w *= 0.41;
            color.rgb *= color.w;
            sum += color * (1.0 - sum.a) * altitudeCoefficient;
        }
        depth += max(0.1, 0.05*depth);
    }
    fragColor = vec4(sum.rgb,sum.w);
}
