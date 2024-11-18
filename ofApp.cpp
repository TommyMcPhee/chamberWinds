#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	shader.load("chamberWindsShader");
	videoBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	videoBuffer.clear();
	videoBuffer.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
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
	shader.setUniform2f("window", window);
	ofClear(0, 0, 0, 255);
}

void ofApp::setUniforms() {
		shader.setUniform3f("horn1", horn1);
		shader.setUniform3f("horn2", horn2);
		shader.setUniform3f("clarinet1", clarinet1);
		shader.setUniform3f("clarinet2", clarinet2);
		shader.setUniform3f("bassoon1", bassoon1);
		shader.setUniform3f("bassoon2", bassoon2);
		shader.setUniform3f("oboe1", oboe1);
		shader.setUniform3f("oboe2", oboe2);
		shader.setUniform3f("flute1", flute1);
		shader.setUniform3f("flute2", flute2);
}
