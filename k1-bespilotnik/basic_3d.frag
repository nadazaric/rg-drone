#version 330 core
out vec4 FragColor;

in vec3 chNormal;
in vec3 chFragPos;
in vec2 chUV;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;

struct PointLight {
    vec3 position;
    vec3 color;
};
uniform PointLight uPointLights[5];

uniform sampler2D uDiffMap1;

void main()
{
    vec3 result = vec3(0.0);

    // ------------------------------------------------------------------------------ Directional Light
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;

    // diffuse 
    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(uLightPos - chFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // specular
    float specularStrength = 0.35;
//    if (uHasSpecular) specularStrength = texture(uSpecMap1, chUV).r;
    vec3 viewDir = normalize(uLightPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;

    result += ambient + diffuse + specular;

    // ------------------------------------------------------------------------------ Point Light
    for(int i = 0; i < 1; i++) {
        float ambientStrength = 0.05;
        vec3 ambient = ambientStrength * uLightColor;

        // diffuse 
        float diffuseStrength = 0.2;
        vec3 norm = normalize(chNormal);
        vec3 lightDir = normalize(uPointLights[i].position - chFragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * uPointLights[i].color * diffuseStrength;

        // specular
        float specularStrength = 0.3;
//        if(uHasSpecular) specularStrength = texture(uSpecMap1, chUV).r;
        vec3 viewDir = normalize(uViewPos - chFragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * uPointLights[i].color;

        result += diffuse + specular + ambient;
    }

    FragColor = texture(uDiffMap1, chUV) * vec4(result, 1.0);
}