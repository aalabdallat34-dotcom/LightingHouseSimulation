#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in float Height;
in vec2 TexCoord;

uniform sampler2D waterTex;
uniform float time;

void main()
{
    vec2 uv = TexCoord;

    uv.x += time * 0.03;

    vec3 tex =
        texture(waterTex, uv).rgb;

    vec3 waterTint =
        vec3(0.2, 0.5, 0.75);

    tex *= waterTint;

    vec3 lightDir =
        normalize(vec3(0.4, 1.0, 0.3));

    float diff =
        max(dot(normalize(Normal), lightDir), 0.0);

    vec3 lighting =
        tex * (0.65 + diff);

    vec3 viewDir =
        normalize(vec3(0.0, 2.2, 6.5) - FragPos);

    vec3 reflectDir =
        reflect(-lightDir, normalize(Normal));

    float spec =
        pow(max(dot(viewDir, reflectDir),0.0),64.0);

    lighting +=
        vec3(1.0) * spec * 0.08;

    float foam =
        smoothstep(0.07,0.11,Height);

    lighting += foam * 0.04;

    FragColor = vec4(lighting,1.0);
}