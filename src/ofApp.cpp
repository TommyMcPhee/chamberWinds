#include "ofApp.h"
#include <GLFW/glfw3.h>

void ofApp::cin_refresh(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ofApp::print_array_value(int index, int value){
    cout << "[" << index << "]  " << value << endl;
}
//--------------------------------------------------------------
void ofApp::setup() {
	epsilon_float = std::numeric_limits<float>::epsilon();
	unsigned int device_list_size, out_device_index, in_device_index, sample_rate_index;
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

	GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
	GLFWvidmode pVidmode = *glfwGetVideoMode(pMonitor);
	static const int frameSamples = int(trunc(sampleRate * channels / pVidmode.refreshRate)) + 1;

	while(buffer_size < frameSamples / 2){
		buffer_size *= 2;
		if(buffer_size =  buffer_sizes[buffer_sizes.size() - 1]){
			break;
		}
	}
	if(buffer_size < buffer_sizes[0]){
		buffer_size = buffer_sizes[0];
	}
	streamSettings.bufferSize = buffer_size;
	input_buffer = std::make_unique<float[]>(channels * buffer_size);
	input_mono = std::make_unique<float[]>(buffer_size);
	cout << "This program automatically chosses the buffer size based on the monitor's refresh rate." << endl;
	cout << "Buffer size chosen:	" << buffer_size << endl;
	cout << "Please ensure your operating system and hardware is set to be compatible with this buffer size before proceeding." << "\n" << endl;

	unsigned int sample_rates_size = sample_rates.size();
    for(unsigned int a = 0; a < sample_rates_size; a++){
        print_array_value(a, sample_rates[a]);
    }

	cout << "Enter the index of the desired sample rate (chosen sample rate must be compatible with your API and device(s)):" << endl;
    cout << "Enter any integer greater than or equal to " << sample_rates_size << " to use your current default sample rate." << endl;
    
    if(std::cin >> sample_rate_index){
        
        if(sample_rate_index < sample_rates_size){
            streamSettings.sampleRate = sample_rates[sample_rate_index];
        }

    }
    else{
         cout << "Please enter an unsigned integer." << endl;
    	cin_refresh();
    }

	//ofSetVerticalSync(true);
	shader.load("chamberWindsShader");
	
	//videoBuffer.setDefaultTextureIndex(0);
	videoBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	//videoBuffer.clear();
	videoBuffer.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer.end();
	//videoBuffer1.setDefaultTextureIndex(2);

	//videoBuffer.getTextureReference(0);
	
	videoBuffer1.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	//videoBuffer1.clear();
	videoBuffer1.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer1.end();

	streamSettings.numInputChannels = channels;
	streamSettings.numOutputChannels = channels;
	streamSettings.setInListener(this);
	streamSettings.setOutListener(this);
	stream.setup(streamSettings);
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

int ofApp::calculate_index(int inA, int inB){
	return inA * channels + inB;
}

void ofApp::audioIn(ofSoundBuffer &buffer){
	for(int a = 0; a < buffer.getNumFrames(); a++){
		float mono_sample = 1.0;
		for(int b = 0; b < channels; b++){
			int index = calculate_index(a, b);
			input_buffer[index] = buffer[index];
			mono_sample += buffer[index];
		}
		mono_sample *= 0.5;
		input_mono[a] = mono_sample;
	}
}

void ofApp::audioOut(ofSoundBuffer &buffer){
	for(int a = 0; a < buffer.getNumFrames(); a++){
		activity = activity + epsilon_float;
		float test_pitch = TWO_PI * 0.003;
		float resonance = 1.0;
		float input_mono_sample = input_mono[a];
		for(int b = 0; b < channels; b++){
			int index = calculate_index(a, b);
			z2[b] = z1[b];
			z1[b] = z0[b];
			z0[b] = ((-1.0 * resonance * resonance * z2[b]) + (2.0 * resonance * cos(test_pitch) * z1[b]) + (ofRandomf())) * 0.25;
			buffer[index] = z0[b];
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	refresh();
	videoBuffer.begin();
	shader.begin();
	videoBuffer1.draw(0, 0);
	setUniforms();
	shader.end();
	videoBuffer.end();
	videoBuffer.draw(0,0);
	videoBuffer1.begin();
	videoBuffer.draw(0, 0);
	videoBuffer1.end();
}


void ofApp::refresh() {
	frameRate = ofGetFrameRate();
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	//scaling
	/*
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
		*/
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