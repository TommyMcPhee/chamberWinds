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

float beam(float coordinates, float location, float power){
    float adjustedLocation = pow(location, 0.5);
    float scale = 0.5 + abs(0.5 - adjustedLocation);
    return pow((scale - abs(coordinates - location)) * (1.0 / scale), power);
}

float beam2(vec2 normalizedVec, vec4 parameters){
    return pow(beam(normalizedVec.x, parameters.x, parameters.z) * beam(normalizedVec.y, parameters.y, parameters.w), 0.5 * (1.0 - (parameters.z * parameters.w)));
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

float newComponent(vec4 modulator, float feedback, vec2 centered){
    vec2 frequency = modulator.xy * centered;
    //vec2 oscillations = dualOscillate(frequency, centered);
    //float oscillations = oscillate(vec2Float(frequency), vec2Float(centered)) * vec2Float(dualOscillate(frequency, centered));
    float oscillations = oscillate(vec2Float(frequency), vec2Float(centered));
    return mix(oscillations, feedback, vec2Float(modulator.xy));
}

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 inverseNormalized = vec2(1.0) - normalized;
    vec2 position = vec2(pow(normalized.x * inverseNormalized.x, 0.5), pow(normalized.y * inverseNormalized.y, 0.5)) * 2.0;
    float positionPower = vec2Float(1.0 - position) * abs(0.5 - vec2Float(position));
    vec2 adjusted = mix(position, position * window, position);
    vec3 feedbackRGB = texture2DRect(tex0, texCoordVarying).rgb;
    vec3 feedbackHSB = rgb2hsb(feedbackRGB);
    float amplitudeComponent = newComponent(amplitude, vec2Float(1.0 - position), adjusted);
    vec2 amplitudeAdjusted = mix(position, adjusted, amplitudeComponent);
    float brightness = newComponent(pitch, feedbackHSB.r, adjusted);
    float inverseBrightness = 1.0 - brightness;
    float saturation = pow(newComponent(delta * inverseBrightness, feedbackHSB.g, adjusted), 0.5);
    float hue = newComponent(saturation * (1.0 - delta), feedbackHSB.b, adjusted);
    //float positionFeedback = newComponent(amplitude * 0.1, vec2Float(position), adjusted);
    //float positionFeedback = vec2Float(pow(position, 0.5 / amplitude.xy) * (1.0 - amplitude.xy));
    float positionFeedback = amplitudeComponent * vec2Float(pow(position, 1.0 / amplitude.zw) * amplitude.zw);
    vec3 color = mix(hsb2rgb(vec3(hue, saturation, brightness)) * positionFeedback, feedbackRGB, positionFeedback);
    float red = pow(color.r, 0.5);
    float green = pow(color.g, 0.5);
    float blue = pow(color.b, 0.5);
    float secondary = (((red * green) + (red * blue) + (green * blue)) / 3.0) - (red * green * blue);
    float white = red * green * blue;
    float nonWhite = 1.0 - white;
    outputColor = vec4(color, 1.0);
}