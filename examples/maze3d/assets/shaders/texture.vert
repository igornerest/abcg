#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec4 lightPosWorldSpace;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;
out float fragAttenuation;

out vec2 fragTexCoord;
out vec3 fragPObj;
out vec3 fragNObj;

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;
  vec3 L = (viewMatrix * normalize(lightPosWorldSpace - modelMatrix * vec4(inPosition, 1.0))).xyz;

  fragL = L;
  fragV = -P;
  fragN = N;
  fragAttenuation = 1.0 - (distance(vec3(0, 0, 0), P) / 2.5);

  fragTexCoord = inTexCoord;
  fragPObj = inPosition;
  fragNObj = inNormal;
  
  gl_Position = projMatrix * vec4(P, 1.0);
}
