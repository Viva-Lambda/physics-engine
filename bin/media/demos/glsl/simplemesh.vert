#version 410
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTan;
layout(location = 4) in vec3 aBiTan;

uniform mat4 view; // update 
uniform mat4 model; // update
uniform mat4 projection; // update

uniform vec3 viewPos;
uniform vec3 lightPos;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

out vec3 TbnLightPos;
out vec3 TbnViewPos;
out vec3 TbnFragPos;

void main(){
  FragPos = vec3(model * vec4(aPos, 1.0));
  TexCoord = aTexCoord;

  // start computing TBN matrix
  mat3 nmat = transpose(inverse(mat3(model)));

  // tan world
  vec4 tanW = model * vec4(aTan, 0.0);
  vec3 Tan = normalize(vec3(tanW));

  // compute norm
  vec4 norm4 = model * vec4(aNormal, 0.0);
  vec3 Normal = normalize(vec3(norm4));

  // make t perpendicular
  Tan = normalize(Tan - dot(Tan, Normal) * Normal);

  // compute bitangent
  vec3 BiTan = cross(Normal, Tan);

  // make tbn matrix
  mat3 TBN = transpose(mat3(Tan, BiTan, Normal));

  // compute tbn positions
  TbnLightPos = TBN * lightPos;
  TbnViewPos = TBN * viewPos;
  TbnFragPos = TBN * FragPos;

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
