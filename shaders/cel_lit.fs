#version 330

in vec3 fragPos;
in vec3 fragNormal;
in vec4 fragColor;

uniform vec4 colDiffuse;
uniform vec3 lightDir;
uniform float ambientStrength;

out vec4 finalColor;

void main() {
    vec3 n = normalize(fragNormal);
    vec3 l = normalize(-lightDir);
    float ndotl = max(dot(n, l), 0.0);

    float bands;
    if (ndotl > 0.85) bands = 1.0;
    else if (ndotl > 0.55) bands = 0.75;
    else if (ndotl > 0.25) bands = 0.5;
    else bands = 0.25;

    float lighting = max(ambientStrength, bands);
    vec3 base = colDiffuse.rgb * fragColor.rgb;
    finalColor = vec4(base * lighting, colDiffuse.a * fragColor.a);
}
