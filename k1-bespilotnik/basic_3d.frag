#version 330 core
out vec4 FragColor;

in vec3 chNormal;
in vec3 chFragPos;
in vec2 chUV;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform bool uHasSpecular;

uniform sampler2D uDiffMap1;
uniform sampler2D uSpecMap1;

struct PointLight {
    vec3 position;
    vec3 color;
    float diffuseStrength;
};
uniform PointLight uPointLights[6];

void main()
{
    vec3 result = vec3(0.0);

    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor; 

    // diffuse 
    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(uLightPos - chFragPos); // pravac u kome svjetlo dolazi do fragmenta (upadni ugao) pa se normalizuje
    float diff = max(dot(norm, lightDir), 0.0); // dot - racuna cos (odnosno skalarni proizvod) od upadnog ugla u normale, max - da vrijednost bude makar 0.0
    vec3 diffuse = diff * uLightColor; // cos(upadni, normala) * color

    // specular
    float specularStrength = 0.25;
    if (uHasSpecular) specularStrength = texture(uSpecMap1, chUV).r; // ako je crna r = 0, bijela r = 255 = 1.0
    vec3 viewDir = normalize(uViewPos - chFragPos); // ugao gledanja
    vec3 reflectDir = reflect(-lightDir, norm); // reflektovani vektor svjetlosti u odnosu na normalu povrsine
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16); // skalarno mnozenje reflektovanog i ugla gledanja, n = 32  (kolicina bjelila koja se pojavljuje) - manji broj bjelje, manja kuglica ali ostrija i obrnuto
    vec3 specular = specularStrength * spec * uLightColor; // kS * cos(uago gledanja, reflektujuci)

    result += ambient + diffuse + specular;

    // ------------------------------------------------------------------------------ Point Light
    for(int i = 0; i < 6; i++) {
        // ne uzimam ambijentalnu za svako svjetlo
        // float ambientStrength = 0.05;
        // vec3 ambient = ambientStrength * uLightColor;

        // diffuse 
        float diffuseStrength = 0.2;
        vec3 norm = normalize(chNormal);
        vec3 lightDir = normalize(uPointLights[i].position - chFragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * uPointLights[i].color * uPointLights[i].diffuseStrength;

        // specular
        float specularStrength = 0.3;
        if(uHasSpecular) specularStrength = texture(uSpecMap1, chUV).r;
        vec3 viewDir = normalize(uViewPos - chFragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
        vec3 specular = specularStrength * spec * uPointLights[i].color;

        result += diffuse + specular;
    }

    FragColor = texture(uDiffMap1, chUV) * vec4(result, 1.0); // kombinuje se diffMap sa result da bi se dobila ispravna boja 
}