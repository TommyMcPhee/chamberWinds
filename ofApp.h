#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int sampleRate = 48000;
	static const int channels = 2;
	//static const int bufferSize = 2048;

	void cin_refresh();
	std::array<int, 4> sample_rates = {44100, 48000, 88200, 96000};
	void print_array_value(int index, int value);
	float epsilon_float;
	int buffer_size;
	std::unique_ptr<float[]> input_buffer, input_mono;
	std::array<int, 6> buffer_sizes = {64, 128,256, 512, 1024, 2048};
	void setup();
	void draw();
	ofSoundStreamSettings streamSettings;
	ofSoundStream stream;
	void ofSoundStreamSetup(ofSoundStreamSettings &settings);
	int calculate_index(int inA, int inB);
	std::atomic<float> activity = 0.0;
	void audioIn(ofSoundBuffer &buffer);
	std::array<float, 2> z2, z1, z0 = {0.0, 0.0};
	void audioOut(ofSoundBuffer &buffer);
	float dc, amplitude;
	

	void refresh();
	void setUniforms();
	ofShader shader;
	ofFbo videoBuffer, videoBuffer1;
	bool midpoint = false;
	float frameRate, width, height;
	ofVec2f window;
	ofVec4f pitch, tone;
};