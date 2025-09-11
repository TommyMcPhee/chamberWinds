#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int sampleRate = 48000;
	static const int channels = 2;\

	int bufferSize;

	void setup();
	void draw();
	ofSoundStreamSettings streamSettings;
	void ofSoundStreamSetup(ofSoundStreamSettings &settings);
	void audioIn(ofSoundBuffer &buffer);

	void refresh();
	void setUniforms();
	ofShader shader;
	ofFbo videoBuffer;
	bool midpoint = false;
	int bufferSizeDecider;
	float frameRate, width, height, activity = 0.0, activityIncrement;
	ofVec2f window;
	ofVec4f pitch, tone;
};