#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	static const int sampleRate = 48000;
	static const int channels = 2;
	void cin_refresh();
	std::array<int, 4> sample_rates = {44100, 48000, 88200, 96000};
	void print_array_value(int index, int value);
	float epsilon_float;
	int buffer_size;
	std::unique_ptr<float[]> input_buffer, input_mono;
	std::array<int, 6> buffer_sizes = {64, 128,256, 512, 1024, 2048};
	void setup();
	//revisit which actually need to be atomic
	std::array<std::atomic<float>, 2> compared_amplitude, compared_delta, compared_pitch, amplitude_form, delta_form, pitch_form;
	//
	void draw();
	ofSoundStreamSettings streamSettings;
	ofSoundStream stream;
	void ofSoundStreamSetup(ofSoundStreamSettings &settings);
	int calculate_index(int inA, int inB);
	float mix(float inA, float inB, float mix);
	void audioIn(ofSoundBuffer &buffer);
	std::array<float, channels> in_z1, in_dc, in_amplitude_root, in_amplitude, in_delta, in_cross_count, in_pitch, ring_phase, out_z1, out_dc, out_amplitude_root, out_amplitude, out_delta, out_cross_count, out_pitch;
	void average(float &value, float new_value);
	std::array<bool, channels> in_cross, out_cross;
	void analysis(int channel, float z1, float z0, float &dc, float &amplitude_root, float &amplitude, float &delta, bool &cross, float &cross_count, float &pitch);
	std::array<float, 2> comparison(std::array<float, 2> stereo);
	const std::array<float, 2> phase_offsets = {0.0, PI};
	float oscillate(float &phase, float increment);
	std::array<float, 2> average_in_amplitude, average_in_delta, average_in_pitch, average_out_amplitude, average_out_delta, average_out_pitch;
	float sample_count = 0.0, reciprocal_sample_count, amplitude_phase = 0.0, delta_phase = 0.0, pitch_phase = 0.0, amplitude_frequency = 0.0, delta_frequency = 0.0, pitch_frequency = 0.0;
	float channel_oscillate(std::array<float, 2> average_in, float phase, float increment);
	float smallest_float = 1.0 / 16777216.0;
	float increment_form(std::atomic<float> &form, float compared);
	void audioOut(ofSoundBuffer &buffer);
	void refresh();
	void setUniforms();
	ofShader shader;
	ofFbo videoBuffer, videoBuffer1;
	bool midpoint = false;
	float frameRate, width, height;
	ofVec2f window, vec2_amplitude, vec2_delta, vec2_pitch;
};