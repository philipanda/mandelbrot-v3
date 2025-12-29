#version 330 core
out vec4 o;
uniform vec2 ScreenSize, PlaneMin, PlaneMax;
uniform uint MaxIter;
vec2 sq(vec2 z){
    return vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
}

float m(vec2 c){
    vec2 z = vec2(0);
    uint n = 0u;
    while(dot(z,z) <= 4.0 && n < MaxIter){
        z=sq(z)+c;n++;
    }
    return float(n) / float(MaxIter);
}

void main(){
    vec2 uv = gl_FragCoord.xy / ScreenSize;
    float z = m(mix(PlaneMin,PlaneMax,uv));
    o = vec4(z, z, z, 1);
}
