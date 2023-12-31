#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxGui.h"
#include "ofxSvg.h"
#include "ofxImageSequence.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

//#define KINECT



// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle {
public:
    ofColor color;
    int alpha;
    
    CustomParticle(b2World * world, float x, float y) {
        setPhysics(1, 0, 10);
        setup(world, x, y, 1);
    }
        
    
    void draw() {
        float radius = getRadius();
        
        ofPushMatrix();
        ofTranslate(getPosition());
        ofSetColor(color);
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
    
   
    void allocate_fb();
    void morphogenesis(vector <shared_ptr<CustomParticle>> &micelio_player) ;
    void draw_fb_player(vector <shared_ptr<CustomParticle>> &micelio_player) ;
    void carga_lineas();
    
   
    ofShader shaderX, shaderY;
    
    ofImage fondo_1,fondo_2,fondo_3,fondo_4;
    
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
    ofParameter<float> tamano1,tamano2,tamano3;
    ofParameter<int> minimo,maximo,altura_micelio;
    ofParameter<int> cerca_1,lejos_1,cerca_2,lejos_2;
    
    ofxSvg svg;
    vector<ofPath> paths;
    ofPolyline polycallampa;
    int altura;
    int modo;
    int nip,pip; //vis Kinect
    int fb_x,fb_y,offset_fb_y;
    ofxImageSequence shrooms;
    
    float gravX, gravY;
    
   
    bool vsync,lineas,info,gui,kontorno,tracker,color_fondo;
    
#ifdef KINECT
    ofxKinect kinect;
    ofxCvColorImage mirror_kinect;
    ofxCvGrayscaleImage mirror;
    ofxCvGrayscaleImage grayImage;        // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear;   // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar;    // the far thresholded image
    ofxCvGrayscaleImage grayImage2;       // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear2;  // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar2;   // the far thresholded image
    ofxCv::ContourFinder contourFinder;
#endif

    
  
    
};
