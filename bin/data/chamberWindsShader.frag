#version 150

//source for hsv/rgb functions : https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

uniform vec2 window;
uniform vec4 amplitude;
uniform vec4 delta;
uniform vec4 pitch;

float vec2Float(vec2 inVec){
    return pow(inVec.x * inVec.y, 0.5);
}

vec3 rgb2hsb(vec3 c){
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

float unipolar(float inA){
    return inA * 0.5 + 0.5;
}

float oscillate(float frequencyComponent, float centeredComponent){
    return unipolar(cos(frequencyComponent * TWO_PI * centeredComponent + (mod(centeredComponent, TWO_PI) * 0.5)));
}

vec2 dualOscillate(vec2 frequencyVector, vec2 centeredVector){
    return vec2(oscillate(frequencyVector.x, centeredVector.x), oscillate(frequencyVector.y, centeredVector.y));
}

vec2 vec2Root(vec2 inVec){
    return vec2(pow(inVec.x, 0.5), pow(inVec.y, 0.5));
}

float vec2Pow(vec2 inVec, vec2 inPow){
    return vec2Float(vec2(pow(inVec.x, 1.0 / inPow.x), pow(inVec.y, 1.0 / inPow.y)));
}

float newComponent(vec2 modulator, float feedback, vec2 shapeMix, vec2 centered){
    vec2 frequency = modulator.xy * centered;
    float oscillation = oscillate(vec2Float(frequency), vec2Float(centered));
    vec2 oscillations = dualOscillate(frequency, centered);
    float oscillationMix = vec2Float(mix(vec2(oscillation), oscillations, vec2(pow(shapeMix.x, 0.5), pow(shapeMix.y, 0.5))));
    return mix(oscillationMix, feedback, vec2Float(modulator.xy));
}

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 inverseNormalized = vec2(1.0) - normalized;
    vec2 position = vec2(pow(normalized.x * inverseNormalized.x, 0.5), pow(normalized.y * inverseNormalized.y, 0.5)) * 2.0;
    vec2 positionSquared = position * position;
    vec2 adjusted = mix(position, position * window, position);
    vec3 feedbackRGB = texture2DRect(tex0, texCoordVarying).rgb;
    vec3 feedbackHSB = rgb2hsb(feedbackRGB);
    float brightness = pow(newComponent(pitch.xy, feedbackHSB.r, pitch.zw, adjusted) * vec2Pow(position, pitch.zw), vec2Float(pitch.zw));
    float inverseBrightness = 1.0 - brightness;
    float saturation = pow(newComponent(delta.xy * inverseBrightness, feedbackHSB.g, delta.zw, adjusted) * vec2Pow(position, delta.zw), 0.5 * vec2Float(vec2Root(delta.zw)));
    float hue = newComponent(saturation * (1.0 - delta.xy), feedbackHSB.b, delta.zw, adjusted);
    float positionDisplay = vec2Pow(positionSquared, 1.0 - amplitude.zw);
    float positionFeedback = vec2Pow(positionSquared, amplitude.zw);
    vec3 color = mix(hsb2rgb(vec3(hue, saturation, brightness)) * positionDisplay, feedbackRGB, positionFeedback);
    outputColor = vec4(color, 1.0);
}