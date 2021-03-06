//#include "allocore/io/al_App.hpp"
#include "Gamma/Oscillator.h"
#include "Gamma/Envelope.h"
#include "Gamma/Delay.h"
#include "Gamma/Filter.h"
#include "Gamma/Noise.h"
#include "Gamma/SamplePlayer.h"
#include "header/al_AudioApp.hpp"
#include "header/Arduino.hpp"
#include "header/Sound.hpp"
using namespace al;

class MyApp : public AudioApp{
public:
	Arduino arduino[2];
	Values values;
	Synth fmSynth[2];
	int currentRead;
	int meanArray;
	float time;
	
	MyApp():
	values(2,2,3,"COM3",19200) // arduino, sensor, amount of values pr sensor, COMPORT, baudrate 
	//values(2,2,3,"/dev/tty.usbmodem1411",19200) // arduino, sensor, amount of values pr sensor, COMPORT, baudrate 
	{
		initAudio(44100,128,2,0);
		currentRead = 0;
		meanArray = 2500;
		time = 0;
	}

	// Audio callback
	virtual void onSound(AudioIOData& io){	
		gam::sampleRate(io.fps());
	
		//float index = ((arduino[1].angleX + 90) / 180.) * 20;
		//float ratio = ((arduino[1].angleX + 90) / 180.) * 5;
		//float fc = ((arduino[1].angleY + 90) / 180.) * 50+100 ;	
		
		//fmSynth[0].resetEnvelope(arduino[1].veloX, arduino[1].veloY, arduino[1].veloZ);
		
		while(io()){
			
			fmSynth[0].volume[currentRead] = fmSynth[0].currentVolume(arduino[1].veloX, arduino[1].veloY, arduino[1].veloZ);	
			currentRead++;
			if(currentRead == meanArray){
				currentRead = 0;		
			}
			float volume = fmSynth[0].averageFilter(fmSynth[0].volume, meanArray);
			//float out = fmSynth[0].modulate(fc, ratio, index) * fmSynth[0].env();
			float out = fmSynth[1].wavOsc(arduino[1].angleX, arduino[1].angleY);
			io.out(0) = out * volume;
			io.out(1) = out;// * volume;
		}
	} 

	// Graphics callbacks
	virtual void onAnimate(double dt){
		
		if(values.unpack()){
			for(int i = 0; i < 2; i++){
				arduino[i].getValues(i, values.arduino);
				arduino[i].setAngles(dt);
				arduino[i].velocity(dt);
			}
			//std::cout<<"acc: "<<arduino[0].acc[0]<<"     "<<arduino[0].acc[1]<<"     "<<arduino[0].acc[2]<<"\n";
			//std::cout<<arduino[0].angleX<<"     "<<arduino[0].angleY<<"\n";
		}
		
		
	}
};


int main(){
	MyApp().start();
}