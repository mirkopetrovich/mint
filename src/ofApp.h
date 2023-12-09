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
    void allocate_fb();
    
   
    ofShader shaderX, shaderY;
    
    ofImage fondo;
    
    ofxBox2d box2d;
    
    vector    <shared_ptr<CustomParticle>> micelio_player_1;
    vector    <shared_ptr<CustomParticle>> micelio_player_2;
    vector    <shared_ptr<CustomParticle>> micelio_player_3;
    vector    <shared_ptr<ofxBox2dEdge>>    edges;
    vector    <ofPolyline>                  lines;
    
    
    ofFbo fb_player_1, fb_player_2, fb_player_3, fb_blur_X1, fb_blur_Y1, fb_blur_X2, fb_blur_Y2, fb_blur_X3, fb_blur_Y3;
    
    ofxPanel gui1, gui2, gui3, gui4;

    ofParameter<float> random, blur, gravedadX, gravedadY;
    ofParameter<int> fade1,fade2,fade3;
    
    float gravX, gravY;
    
   
    
  
    
};
