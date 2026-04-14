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
    return inVec.x * inVec.y;
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

float oscillate(float frequencyComponent, float centeredComponent){
    return (cos(pow(frequencyComponent, 4.0) * TWO_PI * centeredComponent + (mod(centeredComponent, TWO_PI) * 0.5)) * 0.5 + 0.5) * frequencyComponent;
}

vec2 dualOscillate(vec2 frequencyVector, vec2 centeredVector){
    return vec2(oscillate(frequencyVector.x, centeredVector.x), oscillate(frequencyVector.y, centeredVector.y));
}

float newComponent(float modulator, float feedback, vec2 centered){
    vec2 frequency = vec2(abs(0.5 - modulator) * 2.0);
    vec2 oscillations = dualOscillate(frequency, centered);
    vec2 newComponents = vec2(mix(oscillations, vec2(feedback), modulator));
    return newComponents.x * newComponents.y;
}

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 inverseNormalized = vec2(1.0) - normalized;
    vec2 position = (vec2(0.5) - vec2(normalized.x * inverseNormalized.x, normalized.y * inverseNormalized.y)) * 2.0;
    vec2 adjusted = position * window;
    vec2 amplitudePosition = amplitude.xy * position;
    float amplitudeFloat = beam2(normalized, amplitude);
    vec2 deltaPosition = delta.xy * position;
    float deltaFloat = beam2(normalized, delta);
    vec2 pitchPosition = pitch.xy * position;
    float pitchFloat = beam2(normalized, pitch);
    vec2 inverseAmplitude = vec2(1.0) - amplitude.xy;
    vec3 feedbackHSB = rgb2hsb(texture2DRect(tex0, texCoordVarying).rgb);
    float brightness = newComponent(pitchFloat, feedbackHSB.r, adjusted);
    float saturation = newComponent(brightness * deltaFloat, feedbackHSB.g, adjusted);
    float hue = newComponent((1.0 - saturation) * amplitudeFloat, feedbackHSB.b, adjusted);
    vec3 color = hsb2rgb(vec3(hue, saturation, brightness));
    //vec3 color = vec3(0.0, 0.5, 0.1);
    outputColor = vec4(color, 1.0);
}