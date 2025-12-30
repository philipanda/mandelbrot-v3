#version 330 core
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

uniform sampler2D textTexture;

void main() {
    vec4 tex = texture(textTexture, uv);
    FragColor = vec4(tex.rgb, tex.a);
}
