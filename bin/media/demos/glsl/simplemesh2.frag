#version 410
// simple phong shading with shadow mapping
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 diffColor;

uniform float ambientCoeff = 0.1; // a good value is 0.1

void main() {
  // ambient
  vec3 ambient = ambientCoeff * lightColor;

  // diffuse color
  vec3 norm = normalize(Normal);

  vec3 lightDir = normalize(lightPos - FragPos);

  // cos(theta)
  float diff = max(dot(norm, lightDir), 0.0);

  // diffuse
  vec3 diffuse = diff * lightColor;

  vec3 result = (ambient + diffuse) * diffColor;
  FragColor = vec4(result, 1.0);
}
