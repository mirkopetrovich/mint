#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetLogLevel(OF_LOG_NOTICE);
    
    ofSetFrameRate(60);
    ofSetBackgroundAuto(true);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    ofDisableArbTex(); //Use GL_TEXTURE_2D textures.

    shaderX.load("shaderBlurX");
    shaderY.load("shaderBlurY");
    fondo.load("fondo.jpg");

    // --------------  box2d settings  ----------------------
    box2d.init();
    box2d.setGravity(0.0,-1.4);
    //box2d.createGround(0,50, 1920,50);
    //box2d.createBounds(0,0, 1921,513);
    //box2d.checkBounds(true);
    //box2d.setFPS(60.0);
    box2d.registerGrabbing();
    // -------------------------------------------------------
    gravX = 0.0;
    gravY = -1.4;
    
    // ------------------ gui settings -------------------------------
    gui1.setup();
    gui1.add(gravedadX.set("gravedad X",0.0,-2.0,2.0));
    gui1.add(gravedadY.set("gravedad Y",-1.4,-2.0,2.0));
    gui1.add(blur.set("blur",1.5,0,2));
    gui1.add(random.set("random",0.2, 0.0, 1.0));
    
    gui2.setup("player 1");
    gui2.setPosition(220,10);
    gui2.add(fade1.set("fade",255, 0, 255));
    gui2.add(tamano1.set("radio",0.3, 0.0, 3.0));
    
    
    gui3.setup("player 2");
    gui3.setPosition(440,10);
    gui3.add(fade2.set("fade",255, 0, 255));
    gui3.add(tamano2.set("radio",0.4, 0.0, 3.0));
    
    gui4.setup("player 3");
    gui4.setPosition(660,10);
    gui4.add(fade3.set("fade",255, 0, 255));
    gui4.add(tamano3.set("radio",0.5, 0.0, 3.0));
    
    
    
    // ----------------------------------------------------------------
    
    allocate_fb(); // framebuffer settings
    
    // ------------------------------------------------------------
    
    
    svg.load("anim.svg");         // carga svg en objeto de tipo ofxSVG
    for (int i=0; i<svg.getNumPath();i++) {
        paths.push_back(svg.getPathAt(i));
    }
    
    //--------------------------------------------------
    //File format for the example frames is
    //frame01.png
    //this creates a method call where the parameters
    //prefix is frame, file type is png, from frame 1 to 11, 2 digits in the number
    shrooms.loadSequence("png/shrooms", "png", 5, 47, 2);
    shrooms.preloadAllFrames();    //this way there is no stutter when loading frames
    shrooms.setFrameRate(30);
    
    //---------------------------------------------------
    
    
    // load the lines we saved...
   ifstream f;
    f.open(ofToDataPath("lines.txt").c_str());
    vector <string> strLines;
    while (!f.eof()) {
        string ptStr;
        getline(f, ptStr);
        strLines.push_back(ptStr);
    }
    f.close();
    
    for (int i=0; i<strLines.size(); i++) {
        vector <string> pts = ofSplitString(strLines[i], ",");
        if(pts.size() > 0) {
            auto edge = make_shared<ofxBox2dEdge>();
            for (int j=0; j<pts.size(); j+=2) {
                if(pts[j].size() > 0) {
                    float x = ofToFloat(pts[j]);
                    float y = ofToFloat(pts[j+1]);
                    edge->addVertex(x, y-688);
                }
            }
            edge->create(box2d.getWorld());
            edges.push_back(edge);
        }
    }

}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (gravX!=gravedadX) box2d.setGravity(gravedadX,gravY);
    gravX=gravedadX;
    if (gravY!=gravedadY) box2d.setGravity(gravX,gravedadY);
    gravY=gravedadY;
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
            if (ofRandom(0,1)<0.005) {
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
    
    fb_player_3.begin();
    draw_fb_player_3();
    fb_player_3.end();
    
    altura = int(mouseY/768.*25);
    if (altura<0) altura=0;
    if (altura>25) altura=25;
    paths[altura].translate(ofPoint(400,700,0));
    polycallampa = paths[altura].getOutline()[0];
    paths[altura].translate(ofPoint(-400,-700,0));
   

    
}

void ofApp::draw_fb_player_1(){
 
    for(auto &particle : micelio_player_1) {
        particle->setRadius(tamano1);
        particle->draw();
    }
}

void ofApp::draw_fb_player_2(){
 
    for(auto &particle : micelio_player_2) {
        particle->setRadius(tamano2);
        particle->draw();
    }
}

void ofApp::draw_fb_player_3(){
 
    for(auto &particle : micelio_player_3) {
        particle->setRadius(tamano3);
        particle->draw();
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    //float blur = ofMap(mouseX, 0, ofGetWidth(), 0, 2, true);
    
   // float blur = 1.5;
    
    ofSetColor(255);
    fondo.draw(0,0,1920,1200);
    ofSetColor(230,200,0);
    
    for (auto &line : lines) {
        
        line.draw();
   }
   // for (auto & edge : edges) {
   //     edge->draw();
   //}


    //BLUR
    
    fb_blur_X1.begin();
    ofSetColor(255);
    shaderX.begin();
    shaderX.setUniformTexture("tex0", fb_player_1.getTexture(),1);
    shaderX.setUniform1f("blurAmnt", blur);
    shaderX.setUniform1f("texwidth", 1920);
    fb_player_1.draw(0,0);
    shaderX.end();
    fb_blur_X1.end();
    
    fb_blur_Y1.begin();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fb_blur_X1.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", 1512);
    fb_blur_Y1.draw(0,0);
    shaderY.end();
    fb_blur_Y1.end();
   
    fb_blur_X2.begin();
    ofSetColor(255);
    shaderX.begin();
    shaderX.setUniformTexture("tex0", fb_player_2.getTexture(),1);
    shaderX.setUniform1f("blurAmnt", blur);
    shaderX.setUniform1f("texwidth", 1920);
    fb_player_2.draw(0,0);
    shaderX.end();
    fb_blur_X2.end();
    
    fb_blur_Y2.begin();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fb_blur_X2.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", 1512);
    fb_blur_Y2.draw(0,0);
    shaderY.end();
    fb_blur_Y2.end();
    
    fb_blur_X3.begin();
    ofSetColor(255);
    shaderX.begin();
    shaderX.setUniformTexture("tex0", fb_player_3.getTexture(),1);
    shaderX.setUniform1f("blurAmnt", blur);
    shaderX.setUniform1f("texwidth", 1920);
    fb_player_3.draw(0,0);
    shaderX.end();
    fb_blur_X3.end();
    
    fb_blur_Y3.begin();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fb_blur_X3.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", 1512);
    fb_blur_Y3.draw(0,0);
    shaderY.end();
    fb_blur_Y3.end();
    
    
    ofSetColor(255,255,255,fade1);
    fb_blur_Y1.draw(0,688); // 0,688
    ofSetColor(255,255,255,fade2);
    fb_blur_Y2.draw(0,688); // 0,688
    ofSetColor(255,255,255,fade3);
    fb_blur_Y3.draw(0,688); // 0,688
    ofSetColor(255,255,255,255);
    
    polycallampa.draw();
    
    float percent = ofMap(mouseY, 0, ofGetWidth(), 0, 1.0, true);
    
    //
    ofVec2f punto(0,0);
    
    if (edges.size()>0) punto=edges[0]->getPointAtPercent(mouseX/1920.);
    
    shrooms.getFrameAtPercent(percent)->draw(punto.x,punto.y-220+688);

   
    
    
    ofDrawBitmapString("x: "+ ofToString(mouseX),1740,10);
    ofDrawBitmapString("y: "+ ofToString(mouseY),1740,20);
    ofDrawBitmapString("player 1: "+ ofToString(micelio_player_1.size()),1740,30);
    ofDrawBitmapString("player 2: "+ ofToString(micelio_player_2.size()),1740,40);
    ofDrawBitmapString("player 3: "+ ofToString(micelio_player_3.size()),1740,50);
    ofDrawBitmapString("lineas: "+ ofToString(edges.size()),1740,60);
    ofDrawBitmapString("x: "+ ofToString(punto.x),1740,70);
    ofDrawBitmapString("y: "+ ofToString(punto.y),1740,80);
    
    
    gui1.draw();
    gui2.draw();
    gui3.draw();
    gui4.draw();
}

void ofApp::allocate_fb(){
    
    
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
    
    fb_blur_X1.allocate(1920,512,GL_RGBA);
    fb_blur_X1.begin();
    ofClear(0,0,0,0);
    fb_blur_X1.end();
    
    fb_blur_Y1.allocate(1920,512,GL_RGBA);
    fb_blur_Y1.begin();
    ofClear(0,0,0,0);
    fb_blur_Y1.end();
    
    fb_blur_X2.allocate(1920,512,GL_RGBA);
    fb_blur_X2.begin();
    ofClear(0,0,0,0);
    fb_blur_X2.end();
    
    fb_blur_Y2.allocate(1920,512,GL_RGBA);
    fb_blur_Y2.begin();
    ofClear(0,0,0,0);
    fb_blur_Y2.end();
    
    fb_blur_X3.allocate(1920,512,GL_RGBA);
    fb_blur_X3.begin();
    ofClear(0,0,0,0);
    fb_blur_X3.end();
    
    fb_blur_Y3.allocate(1920,512,GL_RGBA);
    fb_blur_Y3.begin();
    ofClear(0,0,0,0);
    fb_blur_Y3.end();
    // ---------------------------------------------------------------
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
    
    if(key == ' ') {
        
        // want to save out some line...
        if(key == ' ') {
            ofstream f;
            f.clear();
            f.open(ofToDataPath("lines.txt").c_str());
            for (int i=0; i<lines.size(); i++) {
                for (int j=0; j<lines[i].size(); j++) {
                    float x = lines[i][j].x;
                    float y = lines[i][j].y;
                    f << x << "," << y << ",";
                }
                f << "\n";
            }
            f.close();lines.clear();
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
    
    lines.back().addVertex(x, y);

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    lines.push_back(ofPolyline());
    lines.back().addVertex(x, y);

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    auto edge = make_shared<ofxBox2dEdge>();
    
    lines.back().simplify();
    
    for (int i=0; i<lines.back().size(); i++) {
        edge->addVertex(lines.back()[i]+ofPoint(0,-688));
    }
    
    //edge->setPhysics(1, .2, 1);  // uncomment this to see it fall!
    edge->create(box2d.getWorld());
    edges.push_back(edge);
    
    
    
    
    /* drawing.setClosed(false);
    drawing.simplify();
    
    edgeLine.addVertexes(drawing);
    //polyLine.simplifyToMaxVerts(); // this is based on the max box2d verts
    edgeLine.setPhysics(0.0, 0.5, 0.5);
    edgeLine.create(box2d.getWorld());
        
    drawing.clear();*/

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
