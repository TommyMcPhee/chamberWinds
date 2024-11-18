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
		ofVec3f horn1, horn2, clarinet1, clarinet2, bassoon1, bassoon2, oboe1, oboe2, flute1, flute2;
};
