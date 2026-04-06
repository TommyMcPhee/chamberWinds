#version 150

//source for hsv/rgb functions : https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;

uniform vec2 activity;

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

float newComponent(float modulator, float feedback, vec2 centered){
    vec2 frequency = vec2(abs(0.5 - modulator) * 8.0);
    vec2 oscillations = vec2(oscillate(frequency.x, centered.x), oscillate(frequency.y, centered.y));
    vec2 newComponents = vec2(mix(oscillations, vec2(feedback), modulator));
    //log
    return newComponents.x * newComponents.y;
}

void main()
{
    float pixel = window.x * window.y;
    vec2 normalized = vec2(gl_FragCoord.xy / pixel);
    vec2 inverseNormalized = 1.0 - normalized;
    vec2 position = vec2(normalized.x * inverseNormalized.x, normalized.y * inverseNormalized.y) * 4.0;
    vec2 adjusted = position * window;
    vec2 seed = activity * adjusted;
    float seedFloat = seed.x * seed.y;
    

    vec3 feedbackRGB = texture2DRect(tex0, texCoordVarying).rgb;
    feedbackRGB = (0.25 - ((1.0 - feedbackRGB) * feedbackRGB)) * 4.0;
    vec3 feedbackHSB = rgb2hsb(feedbackRGB);

    float brightness = newComponent(seedFloat, feedbackHSB.z, adjusted);
    float saturation = newComponent(brightness, feedbackHSB.y, adjusted);
    float hue = newComponent(1.0 - saturation, feedbackHSB.x, adjusted);

    vec3 color = mix(hsb2rgb(vec3(hue, saturation, brightness)), feedbackRGB, activity.x * activity.y);

    //the output function was updated

    outputColor = vec4(color, 1.0);
}