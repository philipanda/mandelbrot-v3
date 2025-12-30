#version 330 core
#extension GL_ARB_gpu_shader_fp64 : enable


out vec4 o;
uniform dvec2 ScreenSize, PlaneMin, PlaneMax;
uniform uint MaxIter;
dvec2 sq(dvec2 z){
    return dvec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
}

double m(dvec2 c){
    dvec2 z = dvec2(0.0);
    uint n = 0u;
    while(dot(z,z) <= 4.0 && n < MaxIter){
        z=sq(z)+c;n++;
    }
    return double(n) / double(MaxIter);
}

void main(){
    dvec2 uv = dvec2(gl_FragCoord.xy) / ScreenSize;
    double z = m(mix(PlaneMin,PlaneMax,uv));
    o = vec4(z, z, z, 1);
}
