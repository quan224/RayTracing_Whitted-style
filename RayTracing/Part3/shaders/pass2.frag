#version 330 core

in vec3 pix;
out vec4 fragColor;

uniform sampler2D texPass0;
//以下无意义，只是之前测试用的东西
//uniform sampler2D texPass1;
//uniform sampler2D texPass2;
//uniform sampler2D texPass3;
//uniform sampler2D texPass4;
//uniform sampler2D texPass5;
//uniform sampler2D texPass6;

void main() {
   // gl_FragData[0] = vec4(texture2D(texPass0, pix.xy*0.5+0.5).rgb, 1.0);
    fragColor = vec4(texture2D(texPass0, pix.xy*0.5+0.5).rgb, 1.0);
}