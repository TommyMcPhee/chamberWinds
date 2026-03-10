#include "ofApp.h"
#include <GLFW/glfw3.h>

void ofApp::cin_refresh(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
//--------------------------------------------------------------
void ofApp::setup() {
	streamSettings.sampleRate = sampleRate;
	streamSettings.bufferSize = 2048;
	/*
	GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
	GLFWvidmode pVidmode = *glfwGetVideoMode(pMonitor);
	cout << pVidmode.refreshRate << "maxfps" << endl;

	static const int frameSamples = int(trunc(sampleRate * channels / pVidmode.refreshRate)) + 1;
	while(bufferSizeDecider < frameSamples / 2){
		bufferSizeDecider *= 2;
	}
	streamSettings.bufferSize = bufferSizeDecider;
	*/
	unsigned int device_list_size, out_device_index, in_device_index, buffer_size_index, sample_rate_index;
    auto device_list = stream.getDeviceList();
    device_list_size = device_list.size();
    ofSoundDevice out_device, in_device;
    cout << device_list << endl;
    cout << "Enter index of output device:" << endl;
    
    while(true){
        
        if(std::cin >> out_device_index && out_device_index < device_list_size ){
            break;
        }
        else{
            cout << "Please enter a valid ouput device index (unsigned integer) within the listed range." << endl;
            cin_refresh();
        }

    }
    
    out_device = device_list[out_device_index];
    streamSettings.setOutDevice(out_device);
    streamSettings.numOutputChannels = out_device.outputChannels;

	cout << "Enter index of input device:" << endl;
    
    while(true){
        if(std::cin >> in_device_index && in_device_index < device_list_size ){
            break;
        }
        else{
            cout << "Please enter a valid input device index (unsigned integer) within the listed range." << endl;
            cin_refresh();
        }
    }
    
    in_device = device_list[in_device_index];
    streamSettings.setInDevice(in_device);
    streamSettings.numInputChannels = in_device.inputChannels;

	shader.load("chamberWindsShader");
	videoBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	videoBuffer.clear();
	videoBuffer.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer.end();
	videoBuffer1.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	videoBuffer1.clear();
	videoBuffer1.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer1.end();

	streamSettings.setInListener(this);
	streamSettings.setOutListener(this);
	stream.setup(streamSettings);
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::audioIn(ofSoundBuffer &buffer){
	float average_input_sample;
	for(int a = 0; a < buffer.getNumFrames(); a++){
		average_input_sample = 0.0;
		for(int b = 0; b < buffer.getNumChannels(); b++){
			average_input_sample += buffer[a * bufferSize + b];
		}
		average_input_sample /= buffer.getNumChannels();
	}
	cout << average_input_sample << endl;
}

void ofApp::audioOut(ofSoundBuffer &buffer){
	for(int a = 0; a < buffer.getNumFrames(); a++){
		float output_sample = ofRandomf();
		for(int b = 0; b < buffer.getNumChannels(); b++){
			buffer[a * bufferSize + b] = output_sample;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	refresh();
	videoBuffer1.begin();
	shader.begin();
	videoBuffer.draw(0, 0);
	setUniforms();
	shader.end();
	videoBuffer1.end();
	videoBuffer1.draw(0, 0);
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
	videoBuffer1.allocate(width, height);
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