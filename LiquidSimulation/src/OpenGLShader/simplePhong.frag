#version 330
in vec4 vColor;
in vec3 viewNormal;
in vec3 viewEyeDirection;
in vec3 viewLightDirection;

out vec4 fColor;

void main()
{
    vec3 lightColor = vec3(1.0, 0.8, 0.8);
    vec3 diffuseColor = vColor.rgb;
    vec3 ambientColor = vec3(0.2, 0.2, 0.2) * diffuseColor;
    vec3 specularColor = vec3(1.3, 1.3, 1.3) * diffuseColor;

    vec3 n = normalize(viewNormal);
    vec3 l = normalize(viewLightDirection);
    float cosTheta = clamp(dot(n, l), 0.0, 1.0);

    vec3 e = normalize(viewEyeDirection);
    vec3 r = reflect(-l, n);
    float cosAlpha = clamp(dot(e, r), 0.0, 1.0);

    fColor = vec4(ambientColor + diffuseColor * lightColor * cosTheta + specularColor * lightColor * pow(cosAlpha, 6), 1.0);
}
