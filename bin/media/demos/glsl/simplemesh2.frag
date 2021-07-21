#version 410
// simple phong shading with shadow mapping

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;


uniform vec3 diffColor; // update

void main(){
    FragColor = vec4(diffColor,1);
}
