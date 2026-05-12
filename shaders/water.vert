#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 Normal;
out float Height;
out vec2 TexCoord;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float wave(vec2 p, vec2 dir, float freq, float speed, float amp)
{
    return sin(dot(p, dir) * freq + time * speed) * amp;
}

float getHeight(vec2 p)
{
    float h = 0.0;

    h += wave(p, normalize(vec2(1.0, 0.3)), 1.0, 0.7, 0.09);
    h += wave(p, normalize(vec2(-0.7, 1.0)), 0.8, 0.5, 0.07);

    h += wave(p, normalize(vec2(0.5, 0.8)), 3.0, 1.4, 0.025);
    h += wave(p, normalize(vec2(-1.0, 0.2)), 2.5, 1.1, 0.02);

    h += wave(p, normalize(vec2(0.2, -1.0)), 10.0, 2.0, 0.004);
    h += wave(p, normalize(vec2(-0.8, -0.3)), 14.0, 2.8, 0.003);

    return h;
}

void main()
{
    vec3 pos = aPos;

    float h = getHeight(pos.xz);

    pos.y += h;

    Height = h;

    float eps = 0.01;

    float hL = getHeight(pos.xz - vec2(eps, 0.0));
    float hR = getHeight(pos.xz + vec2(eps, 0.0));

    float hD = getHeight(pos.xz - vec2(0.0, eps));
    float hU = getHeight(pos.xz + vec2(0.0, eps));

    Normal = normalize(
        vec3(hL - hR, 2.0, hD - hU)
    );

    FragPos = vec3(model * vec4(pos, 1.0));

    TexCoord = aPos.xz * 6.0;

    gl_Position =
        projection *
        view *
        model *
        vec4(pos, 1.0);
}