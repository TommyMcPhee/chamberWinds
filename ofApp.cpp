#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	settings.setInListener(this);
	settings.sampleRate = sampleRate;
	settings.bufferSize = bufferSize;
	settings.numInputChannels = 1;
	settings.setApi(ofSoundDevice::Api::MS_DS);
	stream.setup(settings);
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

void ofApp::ofSoundStreamSetup() {

}

void ofApp::audioIn(ofSoundBuffer& buffer) {
	for(int a = 0; a < bufferSize; a++){
		analysisAudio[a + bufferSize] = analysisAudio[a];
		analysisAudio[a] = buffer[a];
		float sampleDifference = 0.0;
		for (int b = a; b < bufferSize + a; b++) {
			sampleDifference += analysisAudio[b - a] - analysisAudio[b];
		}
		corellation[a] = sampleDifference / (float)(bufferSize * 2.0);
		cout << corellation[a] << " " << a << endl;
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