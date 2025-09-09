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
	bool midpoint = false;
	float frameRate, width, height, activity = 0.0, activityIncrement;
	ofVec2f window;
	ofVec4f pitch, tone;
};