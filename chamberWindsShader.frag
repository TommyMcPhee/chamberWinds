#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;
uniform vec3 instruments[10];

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    outputColor = vec4(instruments[1].r, 0.0, 0.0, 1.0);
}