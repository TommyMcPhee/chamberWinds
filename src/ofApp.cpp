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
	for(int a = 0; a < channels; a++){
		in_z1[a] = 0.0;
		in_dc[a] = 0.0;
		in_amplitude[a] = 0.0;
		in_delta[a] = 0.0;
		in_pitch[a] = 0.0;
		ring_phase[a] = 0.0;
		out_z1[a] = 0.0;
		out_dc[a] = 0.0;
		out_amplitude[a] = 0.0;
		out_delta[a] = 0.0;
		out_pitch[a] = 0.0;
		amplitude_form[a] = 1.0;
		delta_form[a] = 1.0;
		pitch_form[a] = 1.0;
	}
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

	unsigned int sample_rates_size = sample_rates.size();
    for(unsigned int a = 0; a < sample_rates_size; a++){
        print_array_value(a, sample_rates[a]);
    }

	cout << "Enter the index of the desired sample rate (chosen sample rate must be compatible with your API and device(s)):" << endl;
    cout << "Enter any integer greater than or equal to " << sample_rates_size << " to use your current default sample rate." << endl;
    
    if(std::cin >> sample_rate_index){
        
        if(sample_rate_index < sample_rates_size){
			sample_rate = sample_rates[sample_rate_index];
            streamSettings.sampleRate = sample_rate;
        }

    }
    else{
         cout << "Please enter an unsigned integer." << endl;
    	cin_refresh();
    }

	GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
	GLFWvidmode pVidmode = *glfwGetVideoMode(pMonitor);
	static const int frameSamples = int(trunc(sample_rate * channels / pVidmode.refreshRate)) + 1;

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


	//ofSetVerticalSync(true);
	shader.load("chamberWindsShader");
	
	//videoBuffer.setDefaultTextureIndex(0);
	video_buffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	//videoBuffer.clear();
	video_buffer.begin();
	ofClear(0, 0, 0, 255);
	video_buffer.end();
	//videoBuffer1.setDefaultTextureIndex(2);

	//videoBuffer.getTextureReference(0);
	
	video_buffer1.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	//videoBuffer1.clear();
	video_buffer1.begin();
	ofClear(0, 0, 0, 255);
	video_buffer1.end();

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

float ofApp::mix(float inA, float inB, float mix){
	return (1.0 - mix) * inA + (mix * inB);
}

void ofApp::audioIn(ofSoundBuffer &buffer){
	for(int a = 0; a < buffer.getNumFrames(); a++){
		float mono_sample = 1.0;
		for(int b = 0; b < channels; b++){
			int index = calculate_index(a, b);
			float input_sample = buffer[index];
			input_buffer[index] = input_sample;
			mono_sample += input_sample;
		}
		mono_sample *= 0.5;
		input_mono[a] = mono_sample;
	}
}

void ofApp::average(float &value, float new_value){
    value = mix(value, new_value, reciprocal_sample_count);
}

void ofApp::analysis(int channel, float z1, float z0, float &dc, float &amplitude_root, float &amplitude, float &delta, bool &cross, float &cross_count, float &pitch){
	dc += z0;
	float dc_adjustment = dc / sample_count;
    float inverse_dc_magnitude = 1.0 - abs(dc_adjustment);
    float amplitude_root_increment = sqrt(abs(z0 - dc_adjustment) * inverse_dc_magnitude);
    amplitude_root += amplitude_root_increment;
    amplitude = pow(amplitude_root / sample_count, 2.0);
	delta = mix(delta, abs(z1 - z0) * 0.5, pow(reciprocal_sample_count, delta_form[channel]));
    float threshold = amplitude * inverse_dc_magnitude;
    bool crossed = false;
        
        if(cross){
            
            if(z0 < dc_adjustment - threshold){
                crossed = true;
            }

        }
        else{
            
            if(z0 > dc_adjustment + threshold){
                crossed = true;
            }

        }
        
        if(crossed){
            cross = !cross;
			pitch = mix(pitch, 1.0 / (sample_count - cross_count), pow(reciprocal_sample_count, pitch_form[channel]));
            cross_count = sample_count;
        }

}

std::array<float, 2> ofApp::comparison(std::array<float, 2> stereo){
	std::array<float, 2> compared;
	compared[0] = (stereo[0] + stereo[1]) * 0.5;
	compared[1] = abs(stereo[0] - stereo[1]);
	return compared;
}

float ofApp::oscillate(float &phase, float increment){
	phase += increment;
	phase = fmod(phase, 1.0);
	return sin(phase) * 0.5 + 0.5;
}

float ofApp::channel_oscillate(std::array<float, 2> average_in, float phase, float increment){
	return mix(average_in[0], oscillate(phase, increment * average_in[0]), average_in[1]);
}

float ofApp::increment_form(std::atomic<float> &form, float compared){
	float increment = (1.0 - compared) * smallest_float;
	form -= increment;
	form = ofClamp(form, 0.0, 1.0);
	return compared * form;
}

void ofApp::audioOut(ofSoundBuffer &buffer){
	for(int a = 0; a < buffer.getNumFrames(); a++){
		sample_count += 1.0;
		reciprocal_sample_count = 1.0 / sample_count;
		for(int b = 0; b < channels; b++){
			int index = calculate_index(a, b);
			analysis(b, in_z1[b], input_buffer[index], in_dc[b], in_amplitude_root[b], in_amplitude[b], in_delta[b], in_cross[b], in_cross_count[b], in_pitch[b]);		
		}
		float input_mono_sample = input_mono[a];
		average_in_amplitude = comparison(in_amplitude);
		average_in_delta = comparison(in_delta);
		average_in_pitch = comparison(in_pitch);
		for(int b = 0; b < channels; b++){
			float channel_amplitude = sqrt(channel_oscillate(average_in_amplitude, amplitude_phase, amplitude_frequency));
			float channel_delta = channel_oscillate(average_in_delta, delta_phase, delta_frequency);
			float channel_pitch = channel_oscillate(average_in_pitch, pitch_phase, pitch_frequency);
			float ring = oscillate(ring_phase[b], channel_delta * 0.5);
			float new_sample = sin(input_mono_sample * ring * HALF_PI / (channel_amplitude + smallest_float));
			float output_sample = mix(out_z1[b], mix(new_sample, out_z1[b], channel_delta) * pow(1.0 - channel_amplitude, 2.0), amplitude_form[b]) * (1.0 - amplitude_form[b]);
			analysis(b, out_z1[b], output_sample, out_dc[b], out_amplitude_root[b], out_amplitude[b], out_delta[b], out_cross[b], out_cross_count[b], out_pitch[b]);
			out_z1[b] = output_sample;
			int index = calculate_index(a, b);
			buffer[index] = output_sample;
		}
		average_out_amplitude = comparison(out_amplitude);
		average_out_delta = comparison(out_delta);
		average_out_pitch = comparison(out_pitch);
		amplitude_frequency = 1.0;
		delta_frequency = 1.0;
		pitch_frequency = 1.0;
		for(int b = 0; b < 2; b++){
			compared_amplitude[b] = abs(average_in_amplitude[b] - average_out_amplitude[b]);
			amplitude_frequency *= increment_form(amplitude_form[b], compared_amplitude[b]);
			compared_delta[b] = abs(average_in_delta[b] - average_out_delta[b]);
			delta_frequency *= increment_form(delta_form[b], compared_delta[b]);
			compared_pitch[b] = abs(average_in_pitch[b] - average_out_pitch[b]);
			pitch_frequency *= increment_form(pitch_form[b], compared_pitch[b]);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	refresh();
	video_buffer.begin();
	shader.begin();
	video_buffer1.draw(0, 0);
	setUniforms();
	shader.end();
	video_buffer.end();
	video_buffer.draw(0,0);
	video_buffer1.begin();
	video_buffer.draw(0, 0);
	video_buffer1.end();
}


void ofApp::refresh() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	video_buffer.allocate(width, height);
	video_buffer1.allocate(width, height);
	window.set(width, height);
	vec4_amplitude.set(compared_amplitude[0], compared_amplitude[1], 0.5, 0.5);
	vec4_delta.set(compared_delta[0], compared_delta[1], 0.5, 0.5);
	vec4_pitch.set(pitch_form[0], pitch_form[1], 0.5, 0.5);
	ofClear(0, 0, 0, 255);
}

void ofApp::setUniforms() {
	shader.setUniform2f("window", window);
	shader.setUniform4f("amplitude", vec4_amplitude);
	shader.setUniform4f("delta", vec4_delta);
	shader.setUniform4f("pitch", vec4_pitch);
}