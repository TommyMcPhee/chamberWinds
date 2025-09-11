#include "ofApp.h"
#include <GLFW/glfw3.h>

//--------------------------------------------------------------
void ofApp::setup() {
	const auto processor_count = std::thread::hardware_concurrency();
	cout << processor_count << "processors" << endl;
	
	streamSettings.sampleRate = sampleRate;
	streamSettings.numInputChannels = channels;
	GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
	GLFWvidmode pVidmode = *glfwGetVideoMode(pMonitor);
	cout << pVidmode.refreshRate << "maxfps" << endl;

	static const int frameSamples = int(trunc(sampleRate * channels / pVidmode.refreshRate)) + 1;
	while(bufferSizeDecider < frameSamples / 2){
		bufferSizeDecider *= 2;
	}
	
	streamSettings.bufferSize = bufferSizeDecider;
	shader.load("chamberWindsShader");
	videoBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	videoBuffer.clear();
	videoBuffer.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer.end();
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::audioIn(ofSoundBuffer &buffer){
	for(int a = 0; a < buffer.getNumFrames(); a++){
			//autocorrelation code
	}
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
	frameRate = ofGetFrameRate();
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	//scaling
	activityIncrement = pow(1.0 / (width * height), activity * 0.5 + 0.25);
	if(midpoint){
		activity -= activityIncrement;
	}
	else{
		activity += activityIncrement;
		if(activity > 1.0){
			midpoint = true;
		}
	}
	videoBuffer.allocate(width, height);
	window.set(width, height);
	tone.set(ofRandomf(), ofRandomf(), ofRandomf(), ofRandomf());
	pitch.set(ofRandomf(), ofRandomf(), ofRandomf(), ofRandomf());
	ofClear(0, 0, 0, 255);
}

void ofApp::setUniforms() {

	shader.setUniform1f("activity", activity);
	shader.setUniform2f("window", window);
	shader.setUniform4f("pitch", pitch);
	shader.setUniform4f("tone", tone); 
}