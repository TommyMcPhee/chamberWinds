#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;

uniform float iteration[3];

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 inverseNormalized = 1.0 - normalized;
    vec3 color = vec3(1.0);
    float position = pow(inverseNormalized.y, 2.0) * pow(normalized.x * inverseNormalized.x, 2.0) * normalized.y * (1.0 - (normalized.x * inverseNormalized.x));

    for(int index = 0; index < 3; index++){
        for(float increment = 1.0; increment < iteration[index]; increment++){
        color[index] *= pow(sin(position * increment * TWO_PI) * 0.5 + 0.5, 1.0 / (increment * iteration[index]));
        }
    }

    vec3 feedbackColor = texture2DRect(tex0, texCoordVarying).rgb;

    outputColor = vec4(mix(color, feedbackColor, 0.85), 1.0);
}