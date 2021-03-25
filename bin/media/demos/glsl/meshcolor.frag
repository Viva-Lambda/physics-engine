#version 410

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;

out vec4 FragColor;

uniform float ambientCoeff; // a good value is 0.1
uniform vec3 attC;          // x=c1, y=c2, z=c3
uniform float lightIntensity = 1.0;
// textures
// uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 diffColor;
//
float computeAttenuation(vec3 att, vec3 lpos, vec3 fpos);
vec3 getSurfaceNormal();
vec3 getLightDir(vec3 lpos, vec3 fpos);
vec3 getViewDir(vec3 vpos, vec3 fpos);
vec3 getDiffuseColor(vec3 ldir, vec3 normal, vec3 color);

vec3 getSpecColor(vec3 lightDir, vec3 normal);

void main() {
  // tangent space positions
  vec3 TbnLightPos = TBN * lightPos;
  // vec3 TbnViewPos = TBN * viewPos;
  vec3 TbnFragPos = TBN * FragPos;
  vec3 color = diffColor;
  // ambient term I_a × O_d × k_a
  vec3 ambient = color * ambientCoeff;
  // lambertian terms k_d * (N \cdot L) * I_p
  vec3 surfaceNormal = getSurfaceNormal();
  vec3 lightDirection = getLightDir(TbnLightPos, TbnFragPos);
  vec3 diffuseColor = getDiffuseColor(lightDirection, surfaceNormal, color);
  // attenuation term f_att
  // f_att = min(\frac{1}{c_1 + c_2{\times}d_L + c_3{\times}d^2_{L}} , 1)
  float attenuation = computeAttenuation(attC, TbnLightPos, TbnFragPos);
  vec3 diffuse = attenuation * diffuseColor * lightIntensity;
  // adding specular terms
  // specmap
  float gamma = 2.2;
  vec3 total = ambient + diffuse;
  FragColor = vec4(pow(total, vec3(1.0 / gamma)), 1.0);
}
float computeAttenuation(vec3 att, vec3 lpos, vec3 fpos) {
  float lfragdist = distance(lpos, fpos);
  float distSqr = lfragdist * lfragdist;
  float att1 = lfragdist * att.y;
  float att2 = distSqr * att.z;
  float result = att.x + att2 + att1;
  return min(1 / result, 1.0);
}
vec3 getSurfaceNormal() { return normalize(Normal); }
vec3 getLightDir(vec3 lpos, vec3 fpos) { return normalize(lpos - fpos); }
vec3 getViewDir(vec3 vpos, vec3 fpos) { return normalize(vpos - fpos); }
vec3 getDiffuseColor(vec3 ldir, vec3 normal, vec3 color) {
  float costheta = dot(ldir, normal);
  // opaque surfaces
  return max(costheta, 0.0) * color;
}
