#version 410

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in mat3 TBN;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform float ambientCoeff; // a good value is 0.1
uniform vec3 attC;          // x=c1, y=c2, z=c3
uniform float lightIntensity = 1.0;
// textures
uniform sampler2D shadowMap;
// uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 diffColor;
// implement computations
//
float computeAttenuation(vec3 att, vec3 lpos, vec3 fpos) {
  float lfragdist = distance(lpos, fpos);
  float distSqr = lfragdist * lfragdist;
  float att1 = lfragdist * att.y;
  float att2 = distSqr * att.z;
  float result = att.x + att2 + att1;
  return min(1 / result, 1.0);
}

vec3 getSurfaceNormal() { return normalize(Normal); }
vec3 getLightDir(vec3 lpos, vec3 fpos) { return normalize(lpos - fpos); };
vec3 getViewDir(vec3 vpos, vec3 fpos) { return normalize(vpos - fpos); };
vec3 getDiffuseColor(vec3 ldir, vec3 normal, vec3 color) {
  float costheta = dot(ldir, normal);
  // opaque surfaces
  return max(costheta, 0.0) * color;
};

float compute_shadow(vec4 fposLightSpace, vec3 TbnLightPos, vec3 TbnFragPos) {
  vec3 proj = fposLightSpace.xyz / fposLightSpace.w;
  proj = proj * 0.5 + 0.5;
  // closest depth from light's perspective
  float closest_depth = texture(shadowMap, proj.xy).r;
  // get current fragment's depth from lights perspective
  float current_depth = proj.z;

  // calculate bias
  vec3 surfaceNormal = getSurfaceNormal();
  vec3 lightDirection = getLightDir(TbnLightPos, TbnFragPos);

  float bias = max(0.05 * (1.0 - dot(surfaceNormal, lightDirection)), 0.005);
  // PCF
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(shadowMap, proj.xy + vec2(x, y) * texelSize).r;
      shadow += current_depth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  // keep the shadow at 0.0 when outside the far_plane region of the light's
  // frustum.
  if (proj.z > 1.0)
    shadow = 0.0;

  return shadow;
}

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
  float shadow = compute_shadow(FragPosLightSpace, TbnLightPos, TbnFragPos);
  // specmap
  float gamma = 2.2;
  vec3 total = ambient + (1.0 - shadow) * diffuse;
  FragColor = vec4(pow(total, vec3(1.0 / gamma)), 1.0);
}
