#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	shader.load("chamberWindsShader");
	videoBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	videoBuffer.clear();
	videoBuffer.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer.end();
	//minimumFloat = std::numeric_limits<float>::min();
	minimumFloat = 0.01;
	maxIteration = minimumFloat;
}

//--------------------------------------------------------------
void ofApp::draw() {
	refresh();
	videoBuffer.begin();
	shader.begin();
	videoBuffer.draw(0, 0);
	setUniforms();
	shader.end();
	videoBuffer.end();
	videoBuffer.draw(0, 0);
}

void ofApp::refresh() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	videoBuffer.allocate(width, height);
	window.set(width, height);
	ofClear(0, 0, 0, 255);
}

void ofApp::setUniforms() {
	for (int a = 0; a < 6; a++) {
		if (iteration[a] > maxIteration) {
			maxIteration += minimumFloat;
		}
		iteration[a] += maxIteration;
		iteration[a] *= abs(ofRandomf());
	}
	shader.setUniform2f("window", window);
	shader.setUniform1fv("iteration", iteration, 6);
	cout << iteration[0] << endl;
}