#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void drawFbo();
    
    float fadeAudio( ofSoundPlayer sound, float from_volume, float to_volume, float start_time, float duration );
    
    
    int showState, _showState;
    bool bPlaying;
    bool bDebug;
    bool bFullscreen;
    
    // === OSC =============================

    ofTrueTypeFont font;
    ofxOscReceiver receiver;
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    // === Gradient =============================
    
    ofImage bottomPaletteImage;
    ofPixels bottomPalettePixels;
    ofColor currentBottomColor;
    ofColor fromBottomColor;
    ofColor toBottomColor;
    
    int imgWidth;
    int lastColorIndex;
    int nextColorIndex;
    
    ofImage topPaletteImage;
    ofPixels topPalettePixels;
    ofColor currentTopColor;
    ofColor fromTopColor;
    ofColor toTopColor;
    
    float transSpeed; // moves 0 - 1 to transition fromColor toColor
    
    //color timers
    long startTime;
    long currentTime;
    long lastTime;
    long colorTime;
    int colorDuration; //how long each color lasts in ms
    int cycles;
    
    int cycleDuration;
    int colorPosition;
    
    // === Audio =============================
    
    ofSoundPlayer idle_audio;
    ofSoundPlayer tree_audio;
    float idle_volume;
    float idle_max_volume;
    float tree_volume;
    float tree_max_volume;
    
    // === Video =============================
    
    ofVideoPlayer idle_video;
    ofVideoPlayer tree_video;
    
    int frame_x, frame_y, frame_w, frame_h;
    int video_opacity;
    
    
    // === FBO =============================
    
    ofTexture treeTexture;
    //ofImage image;
    
    ofPoint points[ 4 ];
    int cornerIndex = 0;
    
    ofFbo fbo;




    


};

