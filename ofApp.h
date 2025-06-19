#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void draw();
		void refresh();
		void setUniforms();
		ofShader shader;
		ofFbo videoBuffer;
		float width, height;
		ofVec2f window;
		float minimumFloat, maxIteration;
		float iteration[3] = { 2.0, 2.0, 2.0 };
};
