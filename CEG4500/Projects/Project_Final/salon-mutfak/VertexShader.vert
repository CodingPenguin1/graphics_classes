varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 vertex;
varying vec4 position;


void main() {
    ShadowCoord = gl_TextureMatrix[7] * gl_Vertex;

    // first transform the normal into eye space and normalize the results
    normal = normalize(gl_NormalMatrix * gl_Normal);
    vertex = gl_Vertex;

    // transform vertex coordinates into eye space
    position = gl_ModelViewMatrix * gl_Vertex;
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
}
