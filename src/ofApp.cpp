#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetLogLevel(OF_LOG_NOTICE);
    
    ofSetFrameRate(60);
    ofSetBackgroundAuto(true);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofBackground(0);
    
    ofDisableArbTex(); //Use GL_TEXTURE_2D textures.
    /// GL_TEXTURE_RECTANGLE textures are more intuitive since they allow pixel
    /// based coordinates and are enabled by default.
    ///
    /// GL_TEXTURE_2D textures use normalised texture coordinates (a float value
    /// between 0 and 1 is used to express texture coordinates along width and
    /// height).
    ///
    /// GL_TEXTURE_2D textures are more widely supported and allow advanced features
    /// such as mipmaps and texture compression.
    
    shaderX.load("shaderBlurX");
    shaderY.load("shaderBlurY");
    fondo.load("fondo.jpg");

    
    
    // --------------  box2d settings  ----------------------
    box2d.init();
    box2d.setGravity(0,-1.4);
    //box2d.createGround(0,50, 1920,50);
    //box2d.createBounds(0,0, 1921,513);
    //box2d.checkBounds(true);
    //box2d.setFPS(60.0);
    box2d.registerGrabbing();
    // -------------------------------------------------------
    
    
    
    //  -------------- framebuffer settings -----------------------
    fb_player_1.allocate(1920,512,GL_RGBA);
    fb_player_1.begin();
    ofClear(0,0,0,0);
    fb_player_1.end();
    
    fb_player_2.allocate(1920,512,GL_RGBA);
    fb_player_2.begin();
    ofClear(0,0,0,0);
    fb_player_2.end();
    
    fb_player_3.allocate(1920,512,GL_RGBA);
    fb_player_3.begin();
    ofClear(0,0,0,0);
    fb_player_3.end();
    
    fb_mix.allocate(1920,512,GL_RGBA);
    fb_mix.begin();
    ofClear(0,0,0,0);
    fb_mix.end();
    
    
    fb_blur_X.allocate(1920,512,GL_RGBA);
    fb_blur_X.begin();
    ofClear(0,0,0,0);
    fb_blur_X.end();
    
    fb_blur_Y.allocate(1920,512,GL_RGBA);
    fb_blur_Y.begin();
    ofClear(0,0,0,0);
    fb_blur_Y.end();
    // ---------------------------------------------------------------
    
   
    // ------------------ gui settings -------------------------------
    gui.setup();
    gui.add(fade1.set("fade1",255, 0, 255));
    gui.add(fade2.set("fade2",255, 0, 255));
    gui.add(random.set("random",0.2, 0., 1.));
    // ----------------------------------------------------------------
    
    
    
    
    //settings temporales
   // ofSetCircleResolution(50);
  //  pos.set(860,206);

    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    int hifas_player_1 = micelio_player_1.size();
    int hifas_player_2 = micelio_player_2.size();
    int hifas_player_3 = micelio_player_3.size();
  
    for (int i=0;i<hifas_player_1;i++) {
        
        ofVec2f bump;
        bump.set(ofRandom(-1,1),ofRandom(-1,1));
        bump *= random;
        bump += micelio_player_1[i]->getVelocity();
        bump.normalize();
        micelio_player_1[i]->setVelocity(bump.x,bump.y);
        if (micelio_player_1.size()<80) { //reemplazar por hifas_player_1 ?????
            if (ofRandom(0,1)<0.001) {
                auto nueva = make_shared<CustomParticle>(box2d.getWorld(), micelio_player_1[i]->getPosition().x,micelio_player_1[i]->getPosition().y);
                //micelio_player_1[i]->addAttractionPoint(0,1200,100);
                micelio_player_1.push_back(nueva);
            }
        }
    }
    
   for (int i=0;i<hifas_player_2;i++) {
        
        ofVec2f bump; // puedo usar bump ??? test!!
        bump.set(ofRandom(-1,1),ofRandom(-1,1));
        bump *= random;
        bump += micelio_player_2[i]->getVelocity();
        bump.normalize();
        micelio_player_2[i]->setVelocity(bump.x,bump.y);
 
    }
    
    for (int i=0;i<hifas_player_3;i++) {
         
         ofVec2f bump; // puedo usar bump ??? test!!
         bump.set(ofRandom(-1,1),ofRandom(-1,1));
         bump *= random;
         bump += micelio_player_3[i]->getVelocity();
         bump.normalize();
         micelio_player_3[i]->setVelocity(bump.x,bump.y);
  
     }
    
    box2d.update();
   
    
    ofRemove(micelio_player_1, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(micelio_player_2, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(micelio_player_3, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    
    fb_player_1.begin();
    draw_fb_player_1();
    fb_player_1.end();

    fb_player_2.begin();
    draw_fb_player_2();
    fb_player_2.end();
    
   
   

    
}

void ofApp::draw_fb_player_1(){
 
    for(auto &particle : micelio_player_1) {
        particle->setRadius(0.3);
        particle->draw();
    }
}

void ofApp::draw_fb_player_2(){
 
    for(auto &particle : micelio_player_2) {
        particle->setRadius(1);
        particle->draw();
    }
}



//--------------------------------------------------------------
void ofApp::draw(){
    
    float blur = ofMap(mouseX, 0, ofGetWidth(), 0, 2, true);
    
    ofSetColor(255);
    fondo.draw(0,0,1920,1200);


    
    /*ofSetColor(255,255,255,fade1);
    fb_player_1.draw(0,688);
    ofSetColor(255,255,255,fade2);
    fb_player_2.draw(0,688);
    ofSetColor(255,255,255,255);
    ofNoFill();
    ofDrawCircle(mouseX,mouseY,10);*/


  

    fb_mix.begin();
    fb_player_1.draw(0,0);
    fb_player_2.draw(0,0);
    fb_mix.end();
 
    //BLUR
    
    fb_blur_X.begin();
    ofEnableAlphaBlending();
    ofSetColor(255);
    shaderX.begin();
    shaderX.setUniformTexture("tex0", fb_mix.getTexture(),1);
    shaderX.setUniformTexture("tex1", fb_mix.getTexture(),1);
    shaderX.setUniform1f("blurAmnt", blur);
    shaderX.setUniform1f("texwidth", 1920);
    fb_mix.draw(0,0);
    shaderX.end();
    fb_blur_X.end();
    
    fb_blur_Y.begin();
    ofEnableAlphaBlending();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fb_blur_X.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", 1512);
    fb_blur_Y.draw(0,0);
    shaderY.end();
    fb_blur_Y.end();
   
    
    fb_blur_Y.draw(0,688); // 0,688
    
    
    ofDrawBitmapString("blur: " + ofToString(blur),860,20);
    ofDrawBitmapString("y: "+ ofToString(mouseY),860,40);
    
    gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key == 'z') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<4;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-688); //
            //particle->addAttractionPoint(0,0,10);
            micelio_player_1.push_back(particle);
            
        }
    }
    
    if(key == 'x') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<10;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-688);
            micelio_player_2.push_back(particle);
        }
    }
    
    if(key == 'c') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<10;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-688);
            micelio_player_3.push_back(particle);
        }
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
