#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    font.load("verdana.ttf", 14, true, true);
    showState = 0;
    _showState = 0;
    bPlaying = false;
    bDebug = false;
    bFullscreen = true;
    
    // === OSC =============================

    // listen on the given port
    cout << "listening for osc messages on port: " << PORT << "\n";
    receiver.setup(PORT);
    
    // === Gradient =============================
    topPaletteImage.load( "top_grad_6.png" );
    bottomPaletteImage.load( "bottom_grad_4.png" );
    topPalettePixels = topPaletteImage.getPixels();
    bottomPalettePixels = bottomPaletteImage.getPixels();
    
    //color timers
    startTime = ofGetSystemTime();
    currentTime = startTime;
    lastTime = 0;
    colorTime = 0;
    int minutes = 6.5;
    imgWidth = topPaletteImage.getWidth();
    cycleDuration = 1000 * 60 * minutes;
    colorDuration = cycleDuration / imgWidth;
    colorPosition = 0;
    transSpeed = 0;
    cycles = 0;
    
    currentTopColor = ofColor( 0 );
    currentBottomColor = ofColor( 0 );
    fromTopColor = ofColor( 0 );
    toTopColor = ofColor( 0 );
    fromBottomColor = ofColor( 0 );
    toBottomColor = ofColor( 0 );

    // === Audio =============================
    idle_audio.load( "idle_audio_1.aiff" );
    idle_audio.setLoop( true );
    idle_volume = 0.0;
    idle_max_volume = .60;
    idle_audio.setVolume( idle_volume );
    idle_audio.play();

    tree_audio.load( "tree_audio.mp3" );
    tree_audio.setLoop( false );
    tree_volume = 0.0;
    tree_audio.setVolume( tree_volume );
    tree_max_volume = 1.0;
    
    // === FBO =============================
    
    fbo.allocate( 1600, 1600, GL_RGBA );
    treeTexture.allocate( 1600, 1600, GL_RGBA );

    
    //fbo.allocate( ofGetHeight(), ofGetHeight(), GL_RGBA );
    //treeTexture.allocate( ofGetHeight(), ofGetHeight(), GL_RGBA );
    
    /*points[ 0 ].x = 0; points[ 0 ].y = 0;
    points[ 1 ].x = ofGetHeight(); points[ 1 ].y = 0;
    points[ 2 ].x = ofGetHeight(); points[ 2 ].y = ofGetHeight();
    points[ 3 ].x = 0; points[ 3 ].y = ofGetHeight();
    
    points[ 0 ].x = 0; points[ 0 ].y = 925;
    points[ 1 ].x = 1600; points[ 1 ].y = 930;
    points[ 2 ].x = 1600; points[ 2 ].y = 2580;
    points[ 3 ].x = 0; points[ 3 ].y = 2520;*/
    
    points[ 0 ].x = 0; points[ 0 ].y = ofGetHeight() - tree_video.getHeight();
    points[ 1 ].x = ofGetWidth(); points[ 1 ].y = ofGetHeight() - tree_video.getHeight();
    points[ 2 ].x = ofGetWidth(); points[ 2 ].y = ofGetHeight();
    points[ 3 ].x = 0; points[ 3 ].y = ofGetHeight();
    
    ofBackground( 0 );
    
    // === Video =============================
    // show movies with alpha channels
    tree_video.setPixelFormat(OF_PIXELS_RGBA);
    tree_video.load("frame_blending_5.mov");
    tree_video.setLoopState( OF_LOOP_NONE );
    
    frame_w = tree_video.getWidth() / tree_video.getHeight() * fbo.getHeight();
    frame_h = fbo.getHeight();
    frame_x = -15; //( fbo.getWidth() - frame_w ) / 2;
    frame_y = ( fbo.getHeight() - frame_h );
    
    video_opacity = 255;
    
    cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;


}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetFullscreen( bFullscreen );
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);

        if ( m.getAddress() == "/show" ) {
            showState = m.getArgAsInt32( 0 );
            cout << "showState: " << ofToString( m.getArgAsInt32( 0 )) << endl;

        }
        
    }
    
    
    
    //tree state
    if ( bPlaying ) {
        
        tree_video.update();
        transSpeed = (float) colorTime / colorDuration;
        currentTime = ofGetSystemTime() - startTime;//how long the sketch has been running in m
        
        currentTopColor = fromTopColor.getLerped( toTopColor, transSpeed );
        currentBottomColor = fromBottomColor.getLerped( toBottomColor, transSpeed );
        
        if ( colorTime >= colorDuration ) {
            cycles ++;
            lastColorIndex = nextColorIndex;
            nextColorIndex ++;
            
            if ( nextColorIndex > imgWidth - 1 ) {
                nextColorIndex = 0;
            }
            
            fromTopColor = topPalettePixels.getColor( lastColorIndex, 0 );
            toTopColor = topPalettePixels.getColor( nextColorIndex, 0 );
            
            fromBottomColor = bottomPalettePixels.getColor( lastColorIndex, 0 );
            toBottomColor = bottomPalettePixels.getColor( nextColorIndex, 0 );
            
            colorPosition ++;
            
            if ( colorPosition > imgWidth ) {
                showState = 0;
                colorPosition = 0;
                cout << "palette ended" << endl;
                cout << "show state" << showState << endl;
            }
            
        }
        
        if ( cycles > 1 ) {
            colorTime = currentTime - ((cycles - 1 ) * colorDuration );
        }
        
        else {
            colorTime = currentTime;
        }
        
        if ( tree_video.getIsMovieDone() ) {
            showState = 0;
            cout << "video ended" << endl;

        }
        
        //cross fade audio
        if ( tree_volume < tree_max_volume ) {
            tree_volume += .005;
            tree_audio.setVolume( tree_volume );
            //cout << "tree_volume: " << tree_volume << endl;

        }
        if ( idle_volume > 0 ) {
            idle_volume -= .005;
            idle_audio.setVolume( idle_volume );
            //cout << "idle_volume: " << idle_volume << endl;

        }
    
    }
    
    //idle state
    else  {
        
        if ( transSpeed < 1.0 ) {
            transSpeed += .005;
        }
        
        else if ( transSpeed >= 1.0 ) {
            currentTopColor = ofColor( 0 );
            currentBottomColor = ofColor( 0 );
            fromTopColor = ofColor( 0 );
            toTopColor = ofColor( 0 );
            fromBottomColor = ofColor( 0 );
            toBottomColor = ofColor( 0 );
        }
        
        currentTopColor = fromTopColor.getLerped( toTopColor, transSpeed );
        video_opacity = (int) transSpeed * 255;
        
        if ( video_opacity <= 0 ) {
            tree_video.stop();
            tree_video.setPosition( 0.0 );
        }
        
        //cross fade audio
        if ( idle_volume < idle_max_volume ) {
            idle_volume += .005;
            idle_audio.setVolume( idle_volume );
            //cout << "idle_volume: " << idle_volume << endl;

            
        }
        if ( tree_volume > 0 ) {
            tree_volume -= .005;
            tree_audio.setVolume( tree_volume );
            //cout << "tree_volume: " << tree_volume << endl;

        }
        if ( tree_volume <= 0 ) {
            tree_audio.stop();
            tree_audio.setPosition( 0.0 );
        }
    }
    
    /////////play triggered
    if ( showState == 1 && _showState != 1) {
        
        cout << "playing" << endl;
        bPlaying = true;
        startTime = ofGetSystemTime();
        currentTime = startTime;
        lastTime = 0;
        colorTime = 0;
        colorPosition = 0;
        cycles = 0;
        lastColorIndex = 0;
        nextColorIndex = 0;
        
        tree_audio.setPosition( 0.0 );
        tree_audio.setVolume( 0.0 );
        tree_audio.play();
        
        tree_video.setPosition( 0.0 );
        video_opacity = 255;
        tree_video.play();
        
    }
    
    /////////stop triggered
    else if ( showState == 0 && _showState != 0 ) {
        cout << "stopping" << endl;
        bPlaying = false;
        idle_audio.play();
        idle_audio.setVolume( 0 );
        
        toTopColor = ofColor( 0 );
        toBottomColor = ofColor( 0 );
        transSpeed = 0.0;
        
        startTime = ofGetSystemTime();
        currentTime = startTime;
        lastTime = 0;
        colorTime = 0;
        colorPosition = 0;
        cycles = 0;
        lastColorIndex = 0;
        nextColorIndex = 0;
    }
    
    
    drawFbo();
    
    _showState = showState;
    

}

//--------------------------------------------------------------
void ofApp::drawFbo(){
    
    fbo.begin();
    ofClear(255,255,255, 0);
    
    ofEnableAlphaBlending();
    ofBackgroundGradient( currentTopColor, currentBottomColor, OF_GRADIENT_LINEAR );
    ofSetColor( 255, 255, 255, video_opacity );
    tree_video.draw( frame_x, frame_y, frame_w, frame_h );
    ofDisableAlphaBlending();
    
    
    if ( bDebug ) {
        
        ofSetColor( 255 );
        string buf;
        buf = "listening for osc messages on port" + ofToString(PORT);
        ofDrawBitmapString(buf, 10, 20);
        ofDrawBitmapString( "showState: " + ofToString( showState ), 10, 40 );
        
        ofSetColor( 255, 255, 255 );
        topPaletteImage.draw( 10, 60, 1000, 50 );
        bottomPaletteImage.draw( 10, 120, 1000, 50 );
        
        ofSetColor( currentTopColor );
        ofDrawRectangle( 5 + colorPosition, 50, 10, 60 );
        
        ofSetColor( currentBottomColor );
        ofDrawRectangle( 5 + colorPosition, 110, 10, 60 );
        
        
    }

    
    fbo.end();
    
    treeTexture = fbo.getTexture();

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    treeTexture.draw( points[ 0 ], points[ 1 ], points[ 2 ], points[ 3 ] );

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch( key ) {
        case 'f':
            bFullscreen = !bFullscreen;
            if ( bFullscreen ) {
                ofHideCursor();
            }
            else {
                ofShowCursor();
            }
            break;

        case 'd':
            bDebug = !bDebug;
            break;
            
        case ' ':
            bPlaying = !bPlaying;
            startTime = ofGetSystemTime();
            currentTime = startTime;
            lastTime = 0;
            colorTime = 0;
            colorPosition = 0;
            cycles = 0;
            lastColorIndex = 0;
            nextColorIndex = 0;
            
            break;
            
        case '9':
            showState = 1;
            break;
        
        case '0':
            showState = 0;
            break;
            
        case 'i':
            frame_y -= 5;
            cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;
            break;
            
        case 'k':
            frame_y += 5;
            cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;
            break;
            
        case 'l':
            frame_x += 5;
            cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;
            break;
            
        case 'j':
            frame_x -= 5;
            cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;
            break;
            
        case 'm':
            frame_w *= 1.05;
            frame_h *= 1.05;
            //frame_w = tree_video.getWidth() / tree_video.getHeight() * fbo.getHeight();
            //frame_h = fbo.getHeight();
            frame_x = ( fbo.getWidth() - frame_w ) / 2;
            frame_y = ( fbo.getHeight() - frame_h );
            cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;
            break;
            
        case 'n':
            frame_w *= .95;
            frame_h *= .95;
            //frame_w = tree_video.getWidth() / tree_video.getHeight() * fbo.getHeight();
            //frame_h = fbo.getHeight();
            frame_x = ( fbo.getWidth() - frame_w ) / 2;
            frame_y = ( fbo.getHeight() - frame_h );
            cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;
            break;
            
        case '1':
            cornerIndex = 0;
            break;
            
        case '2':
            cornerIndex = 1;
            break;
            
        case '3':
            cornerIndex = 2;
            break;
            
        case '4':
            cornerIndex = 3;
            break;
            
        case OF_KEY_LEFT:
            points[ cornerIndex ].x -= 5;
            cout <<"points[ " << cornerIndex << " ].x = " << points[ cornerIndex ].x << endl;
            break;
            
        case OF_KEY_RIGHT:
            points[ cornerIndex ].x += 5;
            cout <<"points[ " << cornerIndex << " ].x = " << points[ cornerIndex ].x << endl;
            break;
            
        case OF_KEY_UP:
            points[ cornerIndex ].y -= 5;
            cout <<"points[ " << cornerIndex << " ].y = " << points[ cornerIndex ].y << endl;
            break;
            
        case OF_KEY_DOWN:
            points[ cornerIndex ].y += 5;
            cout <<"points[ " << cornerIndex << " ].y = 3" << points[ cornerIndex ].y << endl;
            break;


    }
    


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
    /*frame_w = tree_video.getWidth() / tree_video.getHeight() * fbo.getHeight();
    frame_h = fbo.getHeight();
    frame_x = ( fbo.getWidth() - frame_w ) / 2;
    frame_y = ( fbo.getHeight() - frame_h );*/
    
        cout << "( x, y, w, h ): " << "( " << frame_x << ", " << frame_y << ", " << frame_w << ", " << frame_h << " )" << endl;
    
    
    if ( bFullscreen ) {
        points[ 0 ].x = 0; points[ 0 ].y = 925;
        points[ 1 ].x = 1600; points[ 1 ].y = 930;
        points[ 2 ].x = 1600; points[ 2 ].y = 2505;
        points[ 3 ].x = 0; points[ 3 ].y = 2475;
    }
    
    else {
        points[ 0 ].x = 0; points[ 0 ].y = ofGetHeight() - tree_video.getHeight();
        points[ 1 ].x = ofGetWidth(); points[ 1 ].y = ofGetHeight() - tree_video.getHeight();
        points[ 2 ].x = ofGetWidth(); points[ 2 ].y = ofGetHeight();
        points[ 3 ].x = 0; points[ 3 ].y = ofGetHeight();
    }
    

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
float ofApp::fadeAudio( ofSoundPlayer sound, float from_volume, float to_volume, float start_time, float duration ){
    
    
    
}