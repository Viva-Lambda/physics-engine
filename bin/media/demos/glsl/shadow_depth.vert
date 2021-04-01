#version 330
/**
 * from learnopengl.com
 * https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/3.1.3.shadow_mapping_depth.vs
 * */
layout(location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() { gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0); }
