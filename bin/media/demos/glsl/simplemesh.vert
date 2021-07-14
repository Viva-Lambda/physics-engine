#version 410
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTan;
layout(location = 4) in vec3 aBiTan;

uniform mat4 view; // update 
uniform mat4 model; // update
uniform mat4 projection; // update

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main(){
  FragPos = vec3(model * vec4(aPos, 1.0));
  TexCoord = aTexCoord;

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}