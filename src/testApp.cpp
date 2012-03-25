#include "testApp.h"
#include <string>
#include <vector>

#define MIDI_SCENECHANGE_CHANNEL 15
#define MIDI_NOTE_FX_CHANNEL 14
#define MIDI_STATUS_NOTE_ON 144
#define MIDI_STATUS_NOTE_OFF 128

//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << RECEIVER_PORT << "\n";
	receiver.setup( RECEIVER_PORT );
	cout << "sending osc messages on port " << SENDER_PORT << "\n";
	sender.setup( HOST, SENDER_PORT );
	try {
		echoSender.setup( ECHO_HOST, SENDER_PORT);
	}
	catch (int e) {
		cout << "Setting up echoSender failed.";
		//echoSender = 0; 
	}

	current_msg_string = 0;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

	ofBackground( 30, 30, 130 );

	int midiInPort = 1;
	cout << "Listing MIDI in ports:\n";
	midiIn.listPorts();
	cout << "MIDI-in port is " << midiInPort << "\n";
	midiIn.openPort(midiInPort);
	midiIn.addListener(this);

	int midiOutPort = 2;
	cout << "Listing MIDI out ports:\n";
	midiOut.listPorts();
	cout << "MIDI-out port is " << midiOutPort << "\n";
	midiOut.openPort(midiOutPort);

}

//--------------------------------------------------------------
void testApp::update(){

	// hide old messages
	for ( int i=0; i < NUM_MSG_STRINGS; i++ )
	{
		if ( timers[i] < ofGetElapsedTimef() )
			msg_strings[i] = "";
	}

	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );

		cout << "getAddress: " << m.getAddress() << endl;

		// break up the address by path separators
		vector<string> pathComponents = split(m.getAddress(), '/');
		string componentOne = "";
		string componentTwo = "";
		string componentThree = "";

		if (pathComponents.size() > 1) {
			componentOne = pathComponents.at(1);
		}
		if (pathComponents.size() > 2) {
			componentTwo = pathComponents.at(2);
		}
		if (pathComponents.size() > 3) {
			componentThree = pathComponents.at(3);
		}

		if (m.getAddress() == "/noteon") {

			int noteNumber = m.getArgAsInt32(0);
			int velocity = m.getArgAsInt32(1);
			cout << "Note on detected, note: " << noteNumber
				<< " velocity: " << velocity << endl;
			midiOut.sendNoteOn (1, noteNumber, velocity);

		} else if (componentOne == "1" || componentOne == "2" || componentOne == "3" || componentOne == "4") {
			
			handleTouchOSCMessage(componentOne, componentTwo, componentThree, m);
		
		} else {
			int noteNumber = m.getArgAsInt32(0);
			cout << "Note off detected, note: " << noteNumber << endl;
			midiOut.sendNoteOff (1, noteNumber, 0);
		}

		// check for mouse moved message
		if ( m.getAddress() == "/mouse/position" )
		{
			// both the arguments are int32's
			mouseX = m.getArgAsInt32( 0 );
			mouseY = m.getArgAsInt32( 1 );
		}
		// check for mouse button message
		else if ( m.getAddress() == "/mouse/button" )
		{
			// the single argument is a string
			mouseButtonState = m.getArgAsString( 0 ) ;
		}
		else
		{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for ( int i=0; i<m.getNumArgs(); i++ )
			{
				// get the argument type
				msg_string += m.getArgTypeName( i );
				msg_string += ":";
				// display the argument - make sure we get the right type
				if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
					msg_string += ofToString( m.getArgAsInt32( i ) );
				else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
					msg_string += ofToString( m.getArgAsFloat( i ) );
				else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
					msg_string += m.getArgAsString( i );
				else
					msg_string += "unknown";
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = ( current_msg_string + 1 ) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

	}
}

//--------------------------------------------------------------
void testApp::draw(){

	string buf;
	buf = "listening for osc messages on port " + ofToString( RECEIVER_PORT );
	ofDrawBitmapString( buf, 10, 20 );

	buf = "sending osc messages on port " + ofToString( SENDER_PORT );
	ofDrawBitmapString( buf, 10, 50 );

	// draw mouse state
	buf = "Mouse coords via OSC: " + ofToString( mouseX, 4) +  " " + ofToString( mouseY, 4 );
	ofDrawBitmapString( buf, 400, 20 );
	ofDrawBitmapString( mouseButtonState, 580, 20 );

	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		ofDrawBitmapString( msg_strings[i], 10, 40+15*i );
	}

}

vector<string> testApp::split(string s, char delim) {
        stringstream temp (stringstream::in | stringstream::out);
        vector<string> elems(0);
        if(s.size() == 0 || delim == 0)
            return elems;
        for each(char c in s)
        {
            if(c == delim)
            {
                elems.push_back(temp.str());
                temp = stringstream(stringstream::in | stringstream::out);
            }
            else
                temp << c;
        }
    if(temp.str().size() > 0)
        elems.push_back(temp.str());
        
	return elems;
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	if ( key =='a' || key == 'A' ){
		cout << "Sending test OSC message\n";
		ofxOscMessage m;
		m.setAddress( "/test" );
		m.addIntArg( 1 );
		m.addFloatArg( 3.5f );
		m.addStringArg( "hello" );
		m.addFloatArg( ofGetElapsedTimef() );
		sender.sendMessage( m );
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::handleTouchOSCMessage(string componentOne, string componentTwo, string componentThree, ofxOscMessage m) {
	

	float value = m.getArgAsFloat(0);

	cout << "Touch OSC message:  " << componentOne << ",  " << componentTwo << ",  " << value << endl;

	if (componentOne == "1") {
		
		// use push buttons 1-9 for scene changes (there are 9 push buttons in the default layout for 1)
		for (int i = 1; i <= 9; i++) {
			string s = "push" + ofToString(i);
			if (componentTwo == s) {
				float value = m.getArgAsFloat(0);

				if (value == 1) {

					sendSceneChangeOSC(i - 1);  // !! scene indexing in Unity starts at zero
				}
			}
		}

		// use push button 10 for explosion!
		if (componentTwo == "push10" && m.getArgAsFloat(0) == 1) {
			sendFXOSC(12, 127);
		}

		// use rotary dials 1-11 on beatMachine iPad
		for (int i = 1; i <= 11; i++) {
			string s = "rotary" + ofToString(i);
			if (componentTwo == s) {
				float value = m.getArgAsFloat(0);

				sendFXOSC(i, (int)(value * 100));
			}
		}
	} else if (componentOne == "3") {
		// rotarys on beatMachine iPhone
		for (int i = 1; i <= 6; i++) {
			string s = "rotary" + ofToString(i);
			if (componentTwo == s) {
				float value = m.getArgAsFloat(0);

				sendFXOSC(i, (int)(value * 100));
			}
		}

		// use pan dials 1-11
		for (int i = 1; i <= 11; i++) {
			string s = "par" + ofToString(i);
			if (componentTwo == s) {
				float value = m.getArgAsFloat(0);

				sendFXOSC(i, (int)(value * 100));
			}
		}

		if (componentTwo == "insertbypass" && componentThree != "") {

			for (int i = 1; i <= 8; i++) {
				
				if (componentThree == ofToString(i)) {
					
					sendSceneChangeOSC(i - 1);  // !! scene indexing in Unity starts at zero
				}
			}
		}
	}
}

void testApp::newMidiMessage(ofxMidiEventArgs& args) {

	int byteOne = args.byteOne;
	int byteTwo = args.byteTwo;

	if (args.channel == MIDI_SCENECHANGE_CHANNEL) {
		if (args.status != MIDI_STATUS_NOTE_ON)
			return;
		sendSceneChangeOSC(byteOne);
	} else if (args.channel == MIDI_NOTE_FX_CHANNEL) {
		sendFXOSC(byteOne, byteTwo);
	} else {
		sendMidiEventOSC(args.status, byteOne, byteTwo);
	}
}

static string OSCSenderAddressString = "/acw";

void testApp::sendSceneChangeOSC(int sceneNum) {
	ofxOscMessage m_test;
	m_test.setAddress(OSCSenderAddressString);

	m_test.addStringArg("scenechange");
	m_test.addIntArg(sceneNum);

	dispatchOSCMessage(m_test);
}

void testApp::sendFXOSC(int cc, int value) {
	ofxOscMessage m_test;
	m_test.setAddress(OSCSenderAddressString);

	m_test.addStringArg("cc");
	m_test.addIntArg (cc);
	m_test.addIntArg (value);

	dispatchOSCMessage(m_test);
}

void testApp::sendMidiEventOSC(int status, int noteNumber, int velocity) {
	ofxOscMessage m_test;
	m_test.setAddress(OSCSenderAddressString);

	m_test.addStringArg("midievent");

	string message_type = "unspecified";
	if (status == MIDI_STATUS_NOTE_ON) {
		message_type = "note_on";
	} else if (status == MIDI_STATUS_NOTE_OFF) {
		message_type = "note_off";
	}

	m_test.addStringArg(message_type);
	m_test.addIntArg( noteNumber );
	m_test.addIntArg( velocity );

	dispatchOSCMessage(m_test);
}

void testApp::dispatchOSCMessage(ofxOscMessage message) {
	cout << "Sending OSC message\n";	
	try {
		sender.sendMessage( message );
		echoSender.sendMessage(message);
	}
	catch (int e) {
		cout << "Could not send OSC message to one of the OSC senders\n";
	}
}