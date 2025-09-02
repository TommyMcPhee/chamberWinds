#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int sampleRate = 48000;
	void setup();
	void draw();
	void ofSoundStreamSetup(ofSoundStreamSettings &settings);


	void refresh();
	void setUniforms();
	ofShader shader;
	ofFbo videoBuffer;
	bool midpoint = 0;
	float frameRate, width, height, activity, activityIncrement;
	ofVec2f window;
	ofVec4f pitch, tone;
	float minimumFloat, maxIteration;
	float iteration[6] = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 };
};