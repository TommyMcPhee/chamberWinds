#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;

uniform float iteration[6];

const vec3 zero=vec3(0.0);
const vec3 one=vec3(1.0);

vec3 hue(float H){
    H*=6.0;
    return clamp(vec3(abs(H-3.)-1.0,2.-abs(H-2.),2.-abs(H-4.)),zero,one);
}

vec3 hsb2rgb(vec3 hsb){
    vec3 rgb=vec3(mix(vec3(1.0),hue(hsb.r),hsb.g));
         rgb=vec3(mix(vec3(0.0),rgb,hsb.b));
    return clamp(rgb,vec3(0.0),vec3(1.0));
}

float scale(float quantity){
    return pow(quantity * 0.5 + 0.5, 2.0);
}

float oscillate(vec2 frequency){
    return scale(sin(pow(frequency.x, 2.0) * TWO_PI * gl_FragCoord.x)) * scale(sin(pow(frequency.y, 2.0) * TWO_PI * gl_FragCoord.y)) * pow((frequency.x + frequency.y) * 0.5, 0.5);
}

void main()
{
    float pixel = window.x * window.y;
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

    float hue = oscillate(vec2(iteration[0], iteration[1]) / 20.0);
    float saturation = oscillate(vec2(iteration[2], iteration[3]) / 20.0);
    float brightness = oscillate(vec2(iteration[4], iteration[5]) / 20.0);

    color = hsb2rgb(vec3(hue, saturation, brightness));

    outputColor = vec4(color, 1.0);
}