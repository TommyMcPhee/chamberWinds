#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int sampleRate = 48000;

	int bufferSize;

	void cin_refresh();
	void setup();
	void draw();
	ofSoundStreamSettings streamSettings;
	ofSoundStream stream;
	void ofSoundStreamSetup(ofSoundStreamSettings &settings);
	void audioIn(ofSoundBuffer &buffer);
	void audioOut(ofSoundBuffer &buffer);
	float dc, amplitude;
	

	void refresh();
	void setUniforms();
	ofShader shader;
	ofFbo videoBuffer, videoBuffer1;
	bool midpoint = false;
	int bufferSizeDecider;
	float frameRate, width, height, activity = 0.0, activityIncrement;
	ofVec2f window;
	ofVec4f pitch, tone;
};