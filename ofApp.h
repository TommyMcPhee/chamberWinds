#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int sampleRate = 48000;
	static const int bufferSize = 256;
	ofSoundStreamSettings settings;
	ofSoundStream stream;
	void setup();
	void ofSoundStreamSetup();
	void audioIn(ofSoundBuffer& buffer);
	void draw();
	void refresh();
	void setUniforms();
	ofShader shader;
	ofFbo videoBuffer;
	float width, height;
	ofVec2f window;
	float minimumFloat, maxIteration;
	float iteration[6] = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 };
	float changeAnimation;
	float changeBrightness;
	float changeSaturation;
	array<float, bufferSize * 2> analysisAudio;
	array<float, bufferSize> corellation;
};