#version 330 core
out vec2 uv;
vec2 p[6] = vec2[](vec2(-1,-1), vec2(1,-1), vec2(-1,1), vec2(-1,1), vec2(1,-1), vec2(1,1));
vec2 t[6] = vec2[](vec2(0,0), vec2(1,0), vec2(0,1), vec2(0,1), vec2(1,0), vec2(1,1));

void main() {
    gl_Position = vec4(p[gl_VertexID], 0, 1);
    uv = t[gl_VertexID];
}
