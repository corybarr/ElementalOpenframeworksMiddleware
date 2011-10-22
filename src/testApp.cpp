#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup( PORT );
	cout << "sending osc messages on port " << SENDER_PORT << "\n";
	sender.setup( HOST, SENDER_PORT );

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
	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
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

		if (m.getAddress() == "/noteon") {

			int noteNumber = m.getArgAsInt32(0);
			int velocity = m.getArgAsInt32(1);
			cout << "Note on detected, note: " << noteNumber
				<< " velocity: " << velocity << endl;
			midiOut.sendNoteOn (1, noteNumber, velocity);
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
	buf = "listening for osc messages on port" + ofToString( PORT );
	ofDrawBitmapString( buf, 10, 20 );

	// draw mouse state
	buf = "mouse: " + ofToString( mouseX, 4) +  " " + ofToString( mouseY, 4 );
	ofDrawBitmapString( buf, 430, 20 );
	ofDrawBitmapString( mouseButtonState, 580, 20 );

	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		ofDrawBitmapString( msg_strings[i], 10, 40+15*i );
	}



}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
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

void testApp::newMidiMessage(ofxMidiEventArgs& args) {
	//ofxMidiEventArgs.ch
	int x = 2;
	int y = x + x;

	int byteOne = args.byteOne;
	int byteTwo = args.byteTwo;
	string message_type = "unspecified";
	
	ofxOscMessage m_test;
	m_test.setAddress("/acw");
	if (args.channel == 15) {
		m_test.addStringArg("scenechange");
		if (args.status != 144)
			return;
		m_test.addIntArg( byteOne );
	} else {
		m_test.addStringArg("midievent");
		if (args.status == 144) {
			message_type = "note_on";
		} else if (args.status == 128) {
			message_type = "note_off";
		}
		m_test.addStringArg(message_type);
		m_test.addIntArg( byteOne );
		m_test.addIntArg( byteTwo );
	}
	cout << "Sending OSC message\n";
	sender.sendMessage( m_test );
}