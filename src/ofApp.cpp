#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofDisableArbTex();
    shaderX.load("shaderBlurX");
    shaderY.load("shaderBlurY");
    fondo.load("fondo.jpg");
    ofSetCircleResolution(50);
    ofEnableAlphaBlending();
    ofSetBackgroundAuto(true);
    
    pos.set(860,206);
    
    fbo1.allocate(1920,512,GL_RGBA);
    fbo1.begin();
    ofClear(0,0,0,0);
    fbo1.end();
    
    fbo2.allocate(1920,512,GL_RGBA);
    fbo2.begin();
    ofClear(0,0,0,0);
    fbo2.end();
    
    fbo3.allocate(1920,512,GL_RGBA);
    fbo3.begin();
    ofClear(0,0,0,0);
    fbo3.end();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    
    vel.x +=ofRandom(-1,1);
    vel.y +=ofRandom(-1,1);
    vel *=0.02;
    vel.normalize();
    
    pos += vel;
    
    fbo1.begin();
    ofEnableAlphaBlending();
    ofSetColor(255,255,255,255);
    ofDrawCircle(pos.x,pos.y,1);
    fbo1.end();
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float blur = ofMap(mouseX, 0, ofGetWidth(), 0, 2, true);
    
    ofEnableAlphaBlending();
    ofSetColor(255);
    
    fondo.draw(0,0,1920,1200);
 
    fbo2.begin();
    ofEnableAlphaBlending();
    ofSetColor(255);
    shaderX.begin();
    shaderX.setUniformTexture("tex0", fbo1.getTexture(),1);
    shaderX.setUniform1f("blurAmnt", blur);
    shaderX.setUniform1f("texwidth", 1920);
    fbo1.draw(0,0);
    shaderX.end();
    fbo2.end();
    
    fbo3.begin();
    ofEnableAlphaBlending();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fbo2.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", 1512);
    fbo2.draw(0,0);
    shaderY.end();
    fbo3.end();
   
    
    fbo3.draw(0,688);
    
    ofDrawBitmapString("blur: " + ofToString(blur),20,20);
    
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
