#version 150

//source for hsv/rgb functions : https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;

uniform float activity;
uniform vec4 pitch;
uniform vec4 tone;

vec3 rgb2hsb(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

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

float oscillate(vec2 frequency, float modulator, vec2 centered){
    vec2 carrier = frequency * modulator;
    return cos(pow(carrier.x, 2.0) * TWO_PI * centered.x + phaseIncrement(carrier.x)) * cos(pow(carrier.y, 2.0) * TWO_PI * centered.y + phaseIncrement(carrier.y)) * 0.5 + 0.5;
}

void main()
{
    float pixel = window.x * window.y;
    vec2 normalized = (gl_FragCoord.xy / window) * 2.0 - 1.0;
    vec2 adjusted = normalized * window;
    vec2 inverseNormalized = 1.0 - normalized;
    float position = pow(inverseNormalized.y, 2.0) * pow(normalized.x * inverseNormalized.x, 2.0) * normalized.y * (1.0 - (normalized.x * inverseNormalized.x));

    vec3 feedbackRGB = texture2DRect(tex0, texCoordVarying).rgb;
    vec3 feedbackHSB = rgb2hsb(feedbackRGB);

    vec4 tonePitch = vec4(tone * pitch);
    vec4 inverseTonePitch = 1.0 - tonePitch;
    float brightness = mix(oscillate(vec2(inverseTonePitch.xy), 1.0, adjusted), feedbackHSB.z, inverseTonePitch.z) * activity;
    float inverseBrightness = 1.0 - brightness;
    float saturation = mix(oscillate(vec2(tonePitch.xy), brightness, adjusted), feedbackHSB.y, tonePitch.z) * brightness;
    float hue = mix(oscillate(vec2(pitch.xy), saturation, adjusted), feedbackHSB.x, pitch.z);

    vec3 color = hsb2rgb(vec3(hue, saturation, brightness));
    //vec3 color = vec3(tonePitch.x, tonePitch.y, brightness);

    outputColor = vec4(color, 1.0);
}