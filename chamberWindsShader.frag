#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;

uniform float activity;
uniform vec4 pitch;
uniform vec4 tone;

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
    return quantity * 0.25 + 0.25;
}

float phaseIncrement(float inverseWavelength){
    return mod(inverseWavelength, TWO_PI) * 0.5;
}

float oscillate(vec2 frequency, vec2 centered){
    vec2 frequencyRatio = pow(vec2(1.0) - abs(frequency), 1.0 / abs(frequency));
    //issue?
    return (scale(cos(pow(frequency.x, 2.0) * TWO_PI * centered.x) + phaseIncrement(frequency.x)) * frequencyRatio.x) + (scale(cos(pow(frequency.y, 2.0) * TWO_PI * centered.y) + phaseIncrement(frequency.y)) * frequencyRatio.y);
}

void main()
{
    float pixel = window.x * window.y;
    vec2 normalized = (gl_FragCoord.xy / window) * 2.0 - 1.0;
    vec2 adjusted = normalized * window;
    vec2 inverseNormalized = 1.0 - normalized;
    vec3 color = vec3(1.0);
    float position = pow(inverseNormalized.y, 2.0) * pow(normalized.x * inverseNormalized.x, 2.0) * normalized.y * (1.0 - (normalized.x * inverseNormalized.x));

    vec3 feedbackColor = texture2DRect(tex0, texCoordVarying).rgb;

    float hue = oscillate(vec2(pitch.x, pitch.y), adjusted);
    vec4 tonePitch = vec4(tone * pitch);
    float saturation = oscillate(vec2(tonePitch.x, tonePitch.y), adjusted);
    float brightness = oscillate(vec2(activity - tonePitch.x, activity - tonePitch.y), adjusted);

    color = hsb2rgb(vec3(hue, saturation, brightness));

    outputColor = vec4(color, 1.0);
}