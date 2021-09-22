#version 410
// simple phong shading with shadow mapping

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

in vec3 TbnLightPos;
in vec3 TbnViewPos;
in vec3 TbnFragPos;

out vec4 FragColor;

// void main(){
//    FragColor = vec4(diffColor,1);
//}

uniform float ambientCoeff = 0.1; // a good value is 0.1
uniform float shininess = 32.0;
uniform float lightIntensity;

uniform vec3 attC;      // x=c1, y=c2, z=c3
uniform vec3 diffColor; // update

float computeAttenuation(vec3 att, float lfragdist) {
  float distSqr = lfragdist * lfragdist;
  float att1 = lfragdist * att.y;
  float att2 = distSqr * att.z;
  float result = att.x + att2 + att1; // att[0] = att.x
  return min(1 / result, 1.0);
}

vec3 getSurfaceNormal() { return normalize(Normal); }
vec3 getLightDir() { return normalize(TbnLightPos - TbnFragPos); }
vec3 getViewDir() { return normalize(TbnViewPos - TbnFragPos); }
vec3 getDiffuseColor(vec3 ldir, vec3 normal, vec3 color) {
  float costheta = dot(ldir, normal);
  // opaque surfaces
  return max(costheta, 0.0) * color;
};

void main() {
  // tangent space positions
  // vec3 TbnViewPos = TBN * viewPos;
  vec3 color = diffColor;
  // ambient term I_a × O_d × k_a
  vec3 ambient = color * ambientCoeff;
  // lambertian terms k_d * (N \cdot L) * I_p
  vec3 surfaceNormal = getSurfaceNormal();
  vec3 lightDirection = getLightDir();
  vec3 diffuseColor = getDiffuseColor(lightDirection, surfaceNormal, color);
  vec3 viewDir = getViewDir();
  vec3 reflectDir = reflect(-lightDirection, surfaceNormal);
  vec3 halfwayDir = normalize(lightDirection + viewDir);
  float spec = pow(max(dot(surfaceNormal, halfwayDir), 0.0), 32.0);
  vec3 specular = vec3(0.2) * spec;
  // attenuation term f_att
  // f_att = min(\frac{1}{c_1 + c_2{\times}d_L + c_3{\times}d^2_{L}} , 1)
  float lfragdist = distance(TbnLightPos, TbnFragPos);
  float attenuation = computeAttenuation(attC, lfragdist);
  vec3 diffuse = diffuseColor;
  // adding specular terms
  // specmap
  float gamma = 2.2;
  vec3 total = (ambient + diffuse + specular);
  FragColor = vec4(pow(total, vec3(1.0 / gamma)), 1.0);
}
