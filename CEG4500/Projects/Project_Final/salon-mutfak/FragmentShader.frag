varying vec3 normal;
varying vec4 position;
varying vec3 vertex;

uniform sampler2D ShadowMap;
varying vec4 ShadowCoord;

// Illumination constants
const float n = 2.0;
const float Ii = 1.0;
const float Ia = 1.0;
float base_color_red = gl_Color.r;
float base_color_green = gl_Color.g;
float base_color_blue = gl_Color.b;


void main() {
    vec3 norm = normalize(normal);

    // Light vector
    vec3 lightv = normalize(gl_LightSource[0].position.xyz);
    vec3 viewv = -normalize(position.xyz);
    vec3 halfv = normalize(lightv + viewv);
    if (dot(viewv, norm) < 0.0)
        norm = -norm;

    // Diffuse reflection
    vec4 diffuse = max(0.0, dot(lightv, norm)) * gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

    // Ambient reflection
    vec4 ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;

    // Specular reflection
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
    if (dot(lightv, viewv) > 0.0)
        specular = pow(max(0.0, dot(norm, halfv)), clamp(gl_FrontMaterial.shininess, 0.1, 10000.0)) * gl_FrontMaterial.specular * gl_LightSource[0].specular;

    // Phong Illumination
    vec3 light_ray = normalize(gl_LightSource[0].position.xyz - vertex);
    vec3 reflection = normal * (2.0 * dot(normal, light_ray)) - light_ray;
    vec4 intensity = Ii * (diffuse * dot(light_ray, normal) + specular * pow(dot(reflection, light_ray), n)) + ambient * Ia;

    // Determine color from Phong Illumination
    vec3 color = clamp(vec3(ambient + diffuse + specular), 0.0, 1.0);

    // Process shadows
    vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w;
    shadowCoordinateWdivide.z -= 0.0005;
    float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.st).z;
    float shadow = 1.0;
    if (ShadowCoord.w > 0.0)
        shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0;

    // Set color
    gl_FragColor = shadow * vec4(color, 1.0);
}
