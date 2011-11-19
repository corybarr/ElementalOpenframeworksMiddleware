#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxMidi.h"

// listen on port 12345
#define PORT 57000
#define SENDER_PORT 57001
#define NUM_MSG_STRINGS 20
#define HOST "127.0.0.1"
#define ECHO_HOST "192.168.0.134"

class testApp : public ofBaseApp, public ofxMidiListener
	{
	public:

		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

		ofTrueTypeFont		font;
		ofxOscReceiver	receiver;
		ofxOscSender sender;
		ofxOscSender echoSender;

		int				current_msg_string;
		string		msg_strings[NUM_MSG_STRINGS];
		float			timers[NUM_MSG_STRINGS];

		int				mouseX, mouseY;
		string			mouseButtonState;

		int port;
		int id;
		int value;
		double timestamp;
		char msg[255];

		ofxMidiIn	midiIn;
		ofxMidiOut  midiOut;

		void newMidiMessage(ofxMidiEventArgs &);
		void testApp::handleTouchOSCMessage(string componentOne, string componentTwo, ofxOscMessage m);
		vector<string> testApp::split(string s, char delim);

		void sendSceneChangeOSC(int sceneNum);
		void sendFXOSC(int cc, int value);
		void sendMidiEventOSC(bool noteOn, int noteNumber, int velocity);
		void dispatchOSCMessage(ofxOscMessage message);
};
