#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxGui.h"
#include "ofxSvg.h"
#include "ofxImageSequence.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

#define KINECT



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
    
    static bool shouldRemoveEsporas(shared_ptr<ofxBox2dBaseShape> shape) {
        return !ofRectangle(0, 0, ofGetWidth(), 600).inside(shape.get()->getPosition());
    }
    
    
};

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		
    
   
    void gaussian_blur();
    void allocate_fb();
    int morphogenesis(vector <shared_ptr<CustomParticle>> &micelio_player, int lifetime) ;
    int esporulacion(vector <shared_ptr<CustomParticle>> &esporas, int lifetime) ;
    void draw_fb_player(vector <shared_ptr<CustomParticle>> &micelio_player) ;
    void carga_lineas();
    int smooth(int valor);
    
    void callampas(int seta, ofVec2f pos);
    
    void esporula(ofVec2f punto);
    void reporte();
    void gui_settings();
    void picture_in_picture();
    
    void start_micelio_1();
    void start_micelio_2();
    void start_micelio_3();
    
    static bool sort_x(glm::vec2 &a, glm::vec2 &b);
    static bool sort_y(glm::vec2 &a, glm::vec2 &b);
   
   
    ofShader shaderX, shaderY;
    
    ofImage fondo_1,fondo_2,fondo_3,fondo_4;
    
    ofxBox2d box2d;
    ofxBox2d box2d_esporas;
    
    vector    <shared_ptr<CustomParticle>> micelio_player_1;
    vector    <shared_ptr<CustomParticle>> micelio_player_2;
    vector    <shared_ptr<CustomParticle>> micelio_player_3;
    vector    <shared_ptr<CustomParticle>> esporas;
    vector    <shared_ptr<ofxBox2dEdge>>    edges;
    
    vector    <ofPolyline>                  lines;
    
    
    ofFbo fb_player_1, fb_player_2, fb_player_3, fb_blur_X1, fb_blur_Y1, fb_blur_X2, fb_blur_Y2, fb_blur_X3, fb_blur_Y3, fb_esporas, fb_esporomas;
    
    ofxPanel gui1, gui2, gui3, gui4, gui5;
    

    ofParameter<float> random, blur, gravedadX, gravedadY;
    ofParameter<int> fade1,fade2,fade3,fade_esporomas;
    ofParameter<float> tamano1,tamano2,tamano3;
    ofParameter<int> minimo,maximo,altura_micelio;
    ofParameter<int> cerca,lejos;
    
    int altura;
    int modo;
    int pip; //vis Kinect
    int fb_x,fb_y,offset_fb_y;
    int contador;
    
    int f_shroom, shroom;
    float pos_esporoma;
    ofxImageSequence Amanita;
    ofxImageSequence Cortinarius;
    ofxImageSequence Cortinariusx3;
    
    float gY;
    
    float inicio;
   
    bool vsync,lineas,info,gui,kontorno,color_fondo,silueta,simpli,puf, nada, comienzo, fadeout_1;
    
#ifdef KINECT
    ofxKinect kinect;
    ofxCvGrayscaleImage mirror;
    ofxCvGrayscaleImage grayImage;        // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear;   // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar;    // the far thresholded image

    ofxCv::ContourFinder contourFinder;
    ofVec2f centroid1,centroid2,centroid3;
    
    
#endif

    deque <glm::vec2> avg,avg2;
    glm::vec2 m1,m2;
    int mx,my;
    int mxx,myy;
    int tet, tet2, tet3;
    
    bool play_shroom;
    bool esporoma_status;
    
    int centroid_smooth;
    
    ofVec2f mano;
    
    ofVec2f punto;
    
    vector<glm::vec2> ord;
    
    int players;
  
    
};
