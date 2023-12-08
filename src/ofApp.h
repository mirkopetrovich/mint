#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxGui.h"

// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle {
public:
    ofColor color;
    
    CustomParticle(b2World * world, float x, float y) {
        setPhysics(1, 0, 10);
        setup(world, x, y, 1);
    }
        
    
    void draw() {
        float radius = getRadius();
        
        ofPushMatrix();
        ofTranslate(getPosition());
       // ofSetHexColor(0xFFFFFF);
        ofNoFill();
        ofDrawCircle(0, 0, radius);
        ofPopMatrix();
    }
};

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
    
    void draw_fb_player_1() ;
    void draw_fb_player_2() ;
    void draw_fb_player_3() ;
    
    ofShader shaderX;
    ofShader shaderY;
    ofImage fondo;
    ofxBox2d box2d;  // box2d world
    
    vector    <shared_ptr<CustomParticle>> micelio_player_1; // this is a custom particle the extends a circle
    vector    <shared_ptr<CustomParticle>> micelio_player_2;
    vector    <shared_ptr<CustomParticle>> micelio_player_3;
    
    
    ofFbo fb_player_1;
    ofFbo fb_player_2;
    ofFbo fb_player_3;
    ofFbo fb_mix;
    ofFbo fb_blur_X;
    ofFbo fb_blur_Y;
    
    ofxPanel gui;
    ofParameter<float> random;
    ofParameter<int> fade1,fade2;
    
  
    
};
