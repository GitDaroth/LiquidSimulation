#version 330
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 translation;
layout(location = 3) in vec3 scale;
layout(location = 4) in vec4 color;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 vColor;
out vec3 viewNormal;
out vec3 viewEyeDirection;
out vec3 viewLightDirection;

void main()
{
    vec3 transformedVertex = vec3(vertex.x * scale.x, vertex.y * scale.y, vertex.z * scale.z) + translation;
    gl_Position = projectionMatrix * viewMatrix * vec4(transformedVertex, 1.0);
    vColor = color;

    viewEyeDirection = -vec4(viewMatrix * vec4(transformedVertex, 1.0)).xyz;
    viewLightDirection = vec4(viewMatrix * vec4(1.0, 1.0, 1.0, 0.0)).xyz;
    viewNormal = vec4(viewMatrix * vec4(normal, 0.0)).xyz;
}
