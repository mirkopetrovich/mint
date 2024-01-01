#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableAntiAliasing();
    ofEnableSmoothing();

#ifdef KINECT
    kinect.setRegistration(true);
    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    kinect.open();
#ifdef DEBUG
    if (kinect.isConnected()) {
        ofLogNotice() << "kinect: " << kinect.getWidth() << "x" << kinect.getHeight();
    }
#endif
    kinect.setCameraTiltAngle(5); //25
#endif
    
    ofSetBackgroundAuto(true);
    ofSetVerticalSync(true);
    ofBackground(0);
    ofDisableArbTex(); //Use GL_TEXTURE_2D textures.
    shaderX.load("shaderBlurX");
    shaderY.load("shaderBlurY");
    fondo_1.load("fondo-1.jpg");
    fondo_2.load("fondo-2.jpg");
    fondo_3.load("fondo-3.png");
    fondo_4.load("fondo-4.png");
    modo = 1;
    
    // --------------  box2d settings  ----------------------
    box2d.init();
    box2d.setGravity(0.0,0);
    //box2d.createGround(0,50, 1920,50);
    //box2d.createBounds(0,0, 1921,513);
    //box2d.checkBounds(true);
    //box2d.setFPS(60.0);
    box2d.registerGrabbing();
    // -------------------------------------------------------
    gravX = 0.0;
    gravY = -1.4;
    // ------------------ gui settings -------------------------------
    {gui1.setup();
        gui1.add(gravedadX.set("gravedad X",0.0,-2.0,2.0));
        gui1.add(gravedadY.set("gravedad Y",0.0,-2.0,2.0));
        gui1.add(blur.set("blur",1.1,0.0,2.0));
        gui1.add(random.set("random",0.2, 0.0, 1.0));
        gui1.add(minimo.set("min",45, 0, 255));
        gui1.add(maximo.set("max",180, 0, 255));
        gui1.add(altura_micelio.set("altura micelio",0, -200, 200));
        
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
        gui4.add(tamano3.set("radio",0.5, 0.0, 3.0));} // GUI SETTINGS
    // ----------------------------------------------------------------
    
    allocate_fb(); // framebuffer settings
    
    // ------------------------------------------------------------
   /* svg.load("anim.svg");         // carga svg en objeto de tipo ofxSVG
    for (int i=0; i<svg.getNumPath();i++) {
        paths.push_back(svg.getPathAt(i));
    }*/
    
    //--------------------------------------------------
    /*shrooms.loadSequence("Amanita/Amanita-", "png", 1, 30, 2);
    shrooms.preloadAllFrames();    //this way there is no stutter when loading frames
    shrooms.setFrameRate(30);*/
    //---------------------------------------------------
    
    carga_lineas(); // líneas horizonte guardadas en bin/data/lines.txt
}

//--------------------------------------------------------------
void ofApp::update(){
    
#ifdef KINECT
    kinect.update();
    
    if (kinect.isFrameNew()) {
        mirror.setFromPixels(kinect.getDepthPixels());
        mirror.mirror(0,1);
        grayImage = mirror;
        grayImage2 = mirror;
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(220, true);
        grayThreshFar.threshold(80);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        
        grayThreshNear2 = grayImage2;
        grayThreshFar2 = grayImage2;
        grayThreshNear2.threshold(100, true);
        grayThreshFar2.threshold(40);
        cvAnd(grayThreshNear2.getCvImage(), grayThreshFar2.getCvImage(), grayImage2.getCvImage(), NULL);
        
        contourFinder.setMinAreaRadius(minimo);
        contourFinder.setMaxAreaRadius(maximo);
        contourFinder.setThreshold(0);
        contourFinder.findContours(grayImage);
        contourFinder.setFindHoles(false);
    }
#endif
    
    if (gravX!=gravedadX) box2d.setGravity(gravedadX,gravY);
    gravX=gravedadX;
    if (gravY!=gravedadY) box2d.setGravity(gravX,gravedadY);
    gravY=gravedadY;
    
    
    morphogenesis(micelio_player_1);
    morphogenesis(micelio_player_2);
    morphogenesis(micelio_player_3);

    box2d.update();
   
    ofRemove(micelio_player_1, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(micelio_player_2, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(micelio_player_3, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    fb_player_1.begin();
    draw_fb_player(micelio_player_1);
    fb_player_1.end();

    fb_player_2.begin();
    draw_fb_player(micelio_player_2);
    fb_player_2.end();
    
    fb_player_3.begin();
    draw_fb_player(micelio_player_3);
    fb_player_3.end();
    
    /*altura = int(mouseY/768.*25);
    if (altura<0) altura=0;
    if (altura>25) altura=25;
    paths[altura].translate(ofPoint(400,700,0));
    polycallampa = paths[altura].getOutline()[0];
    paths[altura].translate(ofPoint(-400,-700,0));*/
   

    
}

void ofApp::morphogenesis(vector<shared_ptr<CustomParticle>> &micelio_player) {
    
    int size = micelio_player.size();
    for (int i=0;i<size;i++) {
        ofVec2f bump;                                       // creamos vector 2f para bump
        bump.set(ofRandom(-1,1),ofRandom(-1,1));            // vector random
        bump *= random;                                     // multiplicamos for factor random
        bump += micelio_player[i]->getVelocity();           // sumamos velocidad de hifa[i]
        bump.normalize();                                   // normaliza
        micelio_player[i]->setVelocity(bump.x,bump.y);      // asignamos a velocidad de hifa[i]
        
        //micelio_player_1[i]->addAttractionPoint(mouseX,mouseY-(ofRandom(3)*100),0.0001);
        
       if (size<1000) {
            if (ofRandom(0,1)<0.001) {
                auto nueva = make_shared<CustomParticle>(box2d.getWorld(), micelio_player[i]->getPosition().x,micelio_player[i]->getPosition().y);
                nueva->setRadius(nueva->getRadius()*0.3);
                micelio_player.push_back(nueva);
            }
        }
    }
    
    /* ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
     float minDis = ofGetMousePressed() ? 300 : 200;
     
         for(auto &circle : micelio_player_1) {
         float dis = mouse.distance(circle->getPosition());
         
        
             circle->addAttractionPoint(mouse.x,mouse.y, 0.0006);
             
     }*/
    
}

void ofApp::draw_fb_player(vector <shared_ptr<CustomParticle>> &micelio_player) {
    for (auto &particle : micelio_player) {
        particle->draw();
    }
}

void ofApp::carga_lineas() {
    
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
void ofApp::draw(){
  
    ofBackground(0);
    ofSetColor(255);
    
    
    ofPushMatrix();
    ofTranslate(0,altura_micelio);
    if (modo==1) fondo_1.draw(0,0);
    if (modo==2) fondo_2.draw(0,0);
    if (modo==3) fondo_3.draw(0,0);
    if (modo==4) fondo_4.draw(0,0);
    ofPopMatrix();
    ;
    ofSetColor(230,200,0);
    
    for (auto &line : lines) {
        
        line.draw();
   }
   
    if (lineas) {                    // Tecla 'l' dibuja línea horizonte
        for (auto & edge : edges) {
            ofPushMatrix();
            ofTranslate(0,688);
            edge->draw();
            ofPopMatrix();
        }
    }


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
    
   /* polycallampa.draw();
    float percent = ofMap(mouseX, 0, ofGetWidth(), 0, 1.0, true);
    ofVec2f punto(0,0);
    if (edges.size()>0) punto=edges[0]->getPointAtPercent(mouseX/1200.);
    shrooms.getFrameAtPercent(percent)->draw(punto.x,punto.y-220+688,100,150);*/

#ifdef KINECT
    
    
    if (tracker) grayImage.draw(0,900,1920,300);
    
    if (kontorno) {
        ofPushMatrix();
        ofScale(2.);
        ofTranslate(200,0);
        contourFinder.draw();
        ofPopMatrix();
    }
    
    ofVec2f posk,tamk;
    if (pip!=0) {
        if (pip==1) {
            posk.set(1600,10);
            tamk.set(320,240);
        }
        if (pip==2) {
            posk.set(160,0);
            tamk.set(1600,1200);
        }
        ofPushMatrix();
        ofTranslate(posk);
        switch(nip) {
            case 0:
                kinect.draw(0,0,tamk.x,tamk.y);
                break;
            case 1:
                kinect.drawDepth(0,0,tamk.x,tamk.y);
                break;
            case 2:
                grayImage.draw(0,0,tamk.x,tamk.y);
                break;
            case 3:
                grayImage2.draw(0,0,tamk.x,tamk.y);
                break;
        }
        ofPopMatrix();
    }
    
#endif
    
    if (info) {
        ofPushMatrix();
        ofTranslate(10,180);
        ofDrawBitmapString("x: "+ ofToString(mouseX),0,10);
        ofDrawBitmapString("y: "+ ofToString(mouseY),0,20);
        ofDrawBitmapString("player 1: "+ ofToString(micelio_player_1.size()),0,30);
        ofDrawBitmapString("player 2: "+ ofToString(micelio_player_2.size()),0,40);
        ofDrawBitmapString("player 3: "+ ofToString(micelio_player_3.size()),0,50);
        ofDrawBitmapString("lineas: "+ ofToString(edges.size()),0,60);
#ifdef KINECT
        ofDrawBitmapString("Kinect W: "+ ofToString(kinect.getWidth()),0,70);
        ofDrawBitmapString("Kinect H: "+ ofToString(kinect.getHeight()),0,80);
#endif
        ofDrawBitmapString("key: "+ ofToString(modo),0,90);
        ofDrawBitmapString("PIP: "+ ofToString(nip),0,100);
        ofPopMatrix();
    }
    
    if (gui) {
        gui1.draw();
        gui2.draw();
        gui3.draw();
        gui4.draw();
    }
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
    
   
    
    if (key>47 && key<58) modo=key-48;
    if (key == 't') tracker=!tracker ;
    if (key == 'k') kontorno=!kontorno ;
    if (key == 'g') gui=!gui ;
    if (key == 'i') info=!info ;
    if (key == 'l') lineas=!lineas ;
    if (key == 'o') {
        nip++;
        nip%=4;
    }
    if (key == 'p') {
        pip++;
        pip%=3;
    }
  
            
    if(key == 's') {
        micelio_player_1.clear();
        fb_player_1.begin();
        ofClear(0,0,0,0);
        fb_player_1.end();
        fb_blur_X1.begin();
        ofClear(0,0,0,0);
        fb_blur_X1.end();
        fb_blur_Y1.begin();
        ofClear(0,0,0,0);
        fb_blur_Y1.end();
        
    }
    
    if(key == 'd') {
        micelio_player_2.clear();
        fb_player_2.begin();
        ofClear(0,0,0,0);
        fb_player_2.end();
        fb_blur_X2.begin();
        ofClear(0,0,0,0);
        fb_blur_X2.end();
        fb_blur_Y2.begin();
        ofClear(0,0,0,0);
        fb_blur_Y2.end();
        
    }
       
    if(key == 'f') {
        micelio_player_3.clear();
        fb_player_3.begin();
        ofClear(0,0,0,0);
        fb_player_3.end();
        fb_blur_X3.begin();
        ofClear(0,0,0,0);
        fb_blur_X3.end();
        fb_blur_Y3.begin();
        ofClear(0,0,0,0);
        fb_blur_Y3.end();
        
    }
       
                 
    
    if(key == 'a') {
        vsync=!vsync;
        ofSetVerticalSync(vsync);
    }
        
    if(key == 'z') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<4;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-688); //
            //particle->addAttractionPoint(0,0,10);
            particle->setRadius(1);
            micelio_player_1.push_back(particle);
            
        }
    }
    
    if(key == 'x') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<4;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-688);
            particle->setRadius(1);
            micelio_player_2.push_back(particle);
        }
    }
    
    if(key == 'c') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<10;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-688);
            particle->setRadius(1);
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
        edge->addVertex(lines.back()[i]+ofPoint(0,-688)); //0,-688
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
