#version 330 core
in vec2 uv;
out vec4 FragColor;

uniform sampler2D textTexture;

void main() {
    vec4 tex = texture(textTexture, vec2(uv.x, 1.0 - uv.y));
    FragColor = vec4(tex.rgb, tex.a);
}
