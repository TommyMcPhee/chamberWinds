#version 150

//source for hsv/rgb functions : https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

uniform vec2 window;
uniform vec2 pitch;
uniform vec4 translate;

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

//needed as function?
float unipolar(float quantity){
    return quantity * 0.5 + 0.5;
}


float oscillate(float frequencyComponent, float centeredComponent){
    return unipolar(cos(pow(frequencyComponent, 4.0) * TWO_PI * centeredComponent + (mod(centeredComponent, TWO_PI) * 0.5))) * frequencyComponent;
}

vec2 dualOscillate(vec2 frequencyVector, vec2 centeredVector){
    return vec2(oscillate(frequencyVector.x, centeredVector.x), oscillate(frequencyVector.y, centeredVector.y));
    //return vec2(0.5);
}

float newComponent(float modulator, float feedback, vec2 centered){
    // why multiply by 8
    vec2 frequency = vec2(abs(0.5 - modulator));
    vec2 oscillations = dualOscillate(frequency, centered);
    
    vec2 newComponents = vec2(mix(oscillations, vec2(feedback), modulator));
    return newComponents.x * newComponents.y;
}

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 inverseNormalized = 1.0 - normalized;
    vec2 position = vec2(normalized.x * inverseNormalized.x, normalized.y * inverseNormalized.y) * 4.0;
    vec2 adjusted = position * window;
    vec2 seed = pitch * adjusted;
    float seedFloat = seed.x * seed.y;
/*  float feedbackH = rgb2hsb(texture2DRect(tex0, texCoordVarying).rgb).r;
    float feedbackS = rgb2hsb(texture2DRect(tex0, texCoordVarying).rgb).g;
    float feedbackB = rgb2hsb(texture2DRect(tex0, texCoordVarying).rgb).b;
*/
    vec3 feedbackHSB = rgb2hsb(texture2DRect(tex0, texCoordVarying).rgb);
    float brightness = newComponent(seedFloat, feedbackHSB.r, adjusted);
    float saturation = newComponent(brightness, feedbackHSB.g, adjusted);
    vec2 hueVector = dualOscillate(vec2(1.0) - saturation, adjusted);
    float hue = pow(hueVector.x * hueVector.y, 0.5);
    vec3 color = hsb2rgb(vec3(hue, saturation, brightness));
    outputColor = vec4(color, 1.0);
}