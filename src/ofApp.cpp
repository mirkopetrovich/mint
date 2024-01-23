#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
    
    // llenamos el buffer de player_1
    
     for (int i=0; i<20; i++) avg.push_back(0);

    
    teta = 0;
 
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    //ofHideCursor();

#ifdef KINECT
    kinect.setRegistration(true);
    kinect.init(false,false,true);
    kinect.open();
    if (kinect.isConnected()) {
        ofLogNotice() << "kinect: " << kinect.getWidth() << "x" << kinect.getHeight();
    }
    kinect.setCameraTiltAngle(20); //25
#endif
    
    ofSetBackgroundAuto(true);
    ofSetVerticalSync(true);
    //ofBackground(0);
    ofDisableArbTex(); //Use GL_TEXTURE_2D textures.
    shaderX.load("shaderBlurX");
    shaderY.load("shaderBlurY");
    fondo_1.load("fondo-1.jpg");
    fondo_2.load("fondo-2.jpg");
    fondo_3.load("fondo-3.png");
    fondo_4.load("fondo-4.png");
    modo = 5; //inicia con fondo negro
    
    // --------------  box2d settings  ----------------------
    
    gY = -1.4;
    box2d.init();
    box2d.setGravity(0.0,gY);
    //box2d.createGround(0,50, 1920,50);
    //box2d.createBounds(0,0, 1921,513);
    //box2d.checkBounds(true);
    //box2d.setFPS(60.0);
    box2d.registerGrabbing();
    
    box2d_esporas.init();
    box2d_esporas.setGravity(0.0,1.);
    box2d_esporas.registerGrabbing();
    
    
    
    
    
    // -------------------------------------------------------
    //gravX = 0.0;
    
    // ------------------ gui settings -------------------------------
    gui1.setup();
        gui1.add(gravedadX.set("gravedad X",0.f,-2.0,2.0));
        gui1.add(gravedadY.set("gravedad Y",gY,-2.0,2.0));
        gui1.add(blur.set("blur",0.48,0.0,2.0));
        gui1.add(random.set("random",0.2, 0.0, 1.0));
        gui1.add(minimo.set("min",45, 0, 255));
        gui1.add(maximo.set("max",180, 0, 255));
        gui1.add(altura_micelio.set("altura micelio",-223, -250, 100));
        gui1.add(cerca.set("umbral cerca",240, 50, 255)); //UMBRAL SALA
        gui1.add(lejos.set("umbral lejos",20, 0, 150));   //UMBRAL SALA
      // gui1.add(cerca_2.set("umbral cerca 2",20, 50, 255));
      //  gui1.add(lejos_2.set("umbral lejos 2",0, 0, 150));
        
        
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
    
        gui5.setup("shroom");
        gui5.setPosition(880,10);
        gui5.add(frame_shroom.set("frame",0, 0, 29));

    // ----------------------------------------------------------------
    fb_x = 1920;
    fb_y = 720;
    offset_fb_y = 1200-fb_y;
    allocate_fb(); // framebuffer settings
    
    // ------------------------------------------------------------
    svg.load("anim.svg");         // carga svg en objeto de tipo ofxSVG
    for (int i=0; i<svg.getNumPath();i++) {
        paths.push_back(svg.getPathAt(i));
    }
    
    //--------------------------------------------------
    shrooms.loadSequence("Amanita/Amanita-", "png", 1, 30, 2);
    shrooms.preloadAllFrames();    //this way there is no stutter when loading frames
    shrooms.setFrameRate(30);
    //---------------------------------------------------
    
    carga_lineas(); // líneas horizonte guardadas en bin/data/lines.txt
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if ((frame_shroom<29) && (play_shroom)) frame_shroom++;
    
   /* glm::vec2 mouse(mouseX,mouseY);
    avg.pop_front();
    avg.push_back(mouse);
    glm::vec2 pl_1;
    int tam = avg.size();
    for (int i=0; i<tam ; i++) pl_1 +=avg[i];
    pl_1 = pl_1/tam;
    m1.x=pl_1.x;
    m1.y=pl_1.y;*/
    
    
   // mx_promedio = smooth(mouseX);
   
    
#ifdef KINECT
    kinect.update();
    
    if (kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getDepthPixels());
        grayImage.mirror(0,1);
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(cerca, true);
        grayThreshFar.threshold(lejos);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
       /* grayImage.blur(mouseY/100);
        grayImage.erode_3x3();
        grayImage.erode_3x3();
        grayImage.dilate_3x3();*/
       
       // grayImage.contrastStretch();
        contourFinder.setMinAreaRadius(minimo);
        contourFinder.setMaxAreaRadius(maximo);
        contourFinder.setThreshold(0);
        contourFinder.findContours(grayImage);
        contourFinder.setFindHoles(false);
        contourFinder.setSimplify(false);
      
    }
#endif
    
    /*if (gravX!=gravedadX) box2d.setGravity(gravedadX,gravY);
    gravX=gravedadX;*/
    if (gravedadY!=gY) {
        box2d.setGravity(gravedadX,gravedadY);
        gY=gravedadY;
    }
    
    
    tet = morphogenesis(micelio_player_1,150);
    if (tet) teta = 1;
    morphogenesis(micelio_player_2,100);
    morphogenesis(micelio_player_3,50);
    esporulacion(esporas,5000);
    
    ofVec2f mouse2(ofGetMouseX(), ofGetMouseY());
    float minDis = 50;
    
    for(auto &box : esporas) {
        float dis = mouse2.distance(box->getPosition());
        if(dis < minDis) box->addRepulsionForce(mouse2, ofRandom(5.));
        //box->addAttractionPoint(mouse, 1.0);
    }
    
   
    
    
    

    box2d.update();
    box2d_esporas.update();
   
    ofRemove(micelio_player_1, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(micelio_player_2, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(micelio_player_3, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(esporas, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    fb_player_1.begin();
    draw_fb_player(micelio_player_1);
    fb_player_1.end();

    fb_player_2.begin();
    draw_fb_player(micelio_player_2);
    fb_player_2.end();
    
    fb_player_3.begin();
    draw_fb_player(micelio_player_3);
    fb_player_3.end();
    
    fb_esporas.begin();
    ofClear(0,0,0,0);
    draw_fb_player(esporas);
    fb_esporas.end();
    
    altura = int(mouseY/768.*25);
    if (altura<0) altura=0;
    if (altura>25) altura=25;
    paths[altura].translate(ofPoint(400,700,0));
    polycallampa = paths[altura].getOutline()[0];
    paths[altura].translate(ofPoint(-400,-700,0));
   

    
}

int ofApp::morphogenesis(vector<shared_ptr<CustomParticle>> &micelio_player, int lifetime) {
    
    int ret= 0;
    bool borra_micelio = false;                                 // flag para limpiar micelio al fin del loop
    int size = micelio_player.size();                           // asignamos tamaño antes del loop ya que lo modificaremos
    
    if (size) {                                                 // si el micelio tiene hifas ejecuta el loop
        
        for (int i=0;i<size;i++) {
            ofVec2f bump;                                       // creamos vector 2f temporal para bump
            bump.set(ofRandom(-1,1),ofRandom(-1,1));            // le asignamos valores random
            bump *= random;                                     // multiplicamos for factor random
            bump += micelio_player[i]->getVelocity();           // sumamos velocidad de hifa[i]
            bump.normalize();                                   // normaliza
            
            if (size<lifetime)   {                                   // si es menor al máximo de hifas
                micelio_player[i]->setVelocity(bump.x,bump.y);  //
                
                if (ofRandom(0,1)<0.5) {
                    micelio_player[i]->addAttractionPoint(mouseX,mouseY-(ofRandom(3)*100),0.0001);
                }
                
                if (ofRandom(0,1)<0.003) {                      // factor de mitosis
                    auto nueva = make_shared<CustomParticle>(box2d.getWorld(), micelio_player[i]->getPosition().x,micelio_player[i]->getPosition().y);
                    nueva->setRadius(micelio_player[i]->getRadius());
                    micelio_player.push_back(nueva);
                }
            }
            else {
                borra_micelio = true;
                ret = 1;
            }
        }
        
        if (borra_micelio) micelio_player.clear();
        
    }
    return(ret);
}
   
int ofApp::esporulacion(vector<shared_ptr<CustomParticle>> &esporas, int lifetime) {
    
    int ret= 0;
    bool borra_esporas = false;                                 // flag para limpiar micelio al fin del loop
    int size = esporas.size();                           // asignamos tamaño antes del loop ya que lo modificaremos
    
    if (size) {                                                 // si el micelio tiene hifas ejecuta el loop
        
        for (int i=0;i<size;i++) {
            ofVec2f bump;                                       // creamos vector 2f temporal para bump
            bump.set(ofRandom(-1,1),ofRandom(-1,1));            // le asignamos valores random
            bump *= random;                                     // multiplicamos for factor random
            bump += esporas[i]->getVelocity();           // sumamos velocidad de hifa[i]
            bump.normalize();                                   // normaliza
            
            if (size<lifetime)   {                                   // si es menor al máximo de hifas
                esporas[i]->setVelocity(bump.x,bump.y);  //
                
              
                 //   esporas[i]->addAttractionPoint(mouseX,mouseY-(ofRandom(3)*100),0.01);
            
                
              /*  if (ofRandom(0,1)<0.003) {                      // factor de mitosis
                    auto nueva = make_shared<CustomParticle>(box2d.getWorld(), esporas[i]->getPosition().x,esporas[i]->getPosition().y);
                    nueva->setRadius(esporas[i]->getRadius());
                    esporas.push_back(nueva);
                }*/
            }
            else {
                borra_esporas = true;
                ret = 1;
            }
        }
        
        if (borra_esporas) esporas.clear();
        
    }
    return(ret);
}
        

        
   
    /* ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
     float minDis = ofGetMousePressed() ? 300 : 200;
     
         for(auto &circle : micelio_player_1) {
         float dis = mouse.distance(circle->getPosition());
         
             circle->addAttractionPoint(mouse.x,mouse.y, 0.0006);
             
     }*/

int ofApp::smooth(int valor) {
    
    avg.pop_front();
    avg.push_back(valor);
    int promedio;
    int tam = avg.size();
    for (int i=0; i<tam ; i++) promedio+=avg[i];
    promedio = promedio/tam;
    return (promedio);
    
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
                        edge->addVertex(x, y-offset_fb_y);
                    }
                }
                edge->create(box2d.getWorld());
                edges.push_back(edge);
            }
        }
}


//--------------------------------------------------------------
void ofApp::draw(){
  
    
    if (!color_fondo) ofBackground(0,0,0);
    else ofBackground(0,0,255);
    
    ofSetColor(255);
    
    
    ofPushMatrix();
    ofTranslate(-100,altura_micelio);
    if (modo==1) fondo_1.draw(0,0);
    if (modo==2) fondo_2.draw(0,0);
    if (modo==3) fondo_3.draw(0,0);
    if (modo==4) fondo_4.draw(0,0);
    ofPopMatrix();
    
    ofSetColor(230,200,0);
    
    for (auto &line : lines) {
        
        line.draw();
        
   }
   
    if (lineas) {                    // Tecla 'l' dibuja línea horizonte
        for (auto & edge : edges) {
            ofPushMatrix();
            ofTranslate(0,offset_fb_y);
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
    shaderX.setUniform1f("texwidth", fb_x);
    fb_player_1.draw(0,0);
    shaderX.end();
    fb_blur_X1.end();
    
    fb_blur_Y1.begin();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fb_blur_X1.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", fb_y);
    fb_blur_Y1.draw(0,0);
    shaderY.end();
    fb_blur_Y1.end();
   
    fb_blur_X2.begin();
    ofSetColor(255);
    shaderX.begin();
    shaderX.setUniformTexture("tex0", fb_player_2.getTexture(),1);
    shaderX.setUniform1f("blurAmnt", blur);
    shaderX.setUniform1f("texwidth", fb_x);
    fb_player_2.draw(0,0);
    shaderX.end();
    fb_blur_X2.end();
    
    fb_blur_Y2.begin();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fb_blur_X2.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", fb_y);
    fb_blur_Y2.draw(0,0);
    shaderY.end();
    fb_blur_Y2.end();
    
    fb_blur_X3.begin();
    ofSetColor(255);
    shaderX.begin();
    shaderX.setUniformTexture("tex0", fb_player_3.getTexture(),1);
    shaderX.setUniform1f("blurAmnt", blur);
    shaderX.setUniform1f("texwidth", fb_x);
    fb_player_3.draw(0,0);
    shaderX.end();
    fb_blur_X3.end();
    
    fb_blur_Y3.begin();
    ofSetColor(255);
    shaderY.begin();
    shaderY.setUniformTexture("tex0", fb_blur_X3.getTexture(),1);
    shaderY.setUniform1f("blurAmnt", blur);
    shaderY.setUniform1f("texheight", fb_y);
    fb_blur_Y3.draw(0,0);
    shaderY.end();
    fb_blur_Y3.end();
    
    
    ofSetColor(255,255,255,255);
    fb_esporas.draw(0,0); // 0,688
    
    ofSetColor(255,255,255,fade1);
    fb_blur_Y1.draw(0,offset_fb_y); // 0,688
    ofSetColor(255,255,255,fade2);
    fb_blur_Y2.draw(0,offset_fb_y); // 0,688
    ofSetColor(255,255,255,fade3);
    fb_blur_Y3.draw(0,offset_fb_y); // 0,688
  
    ofSetColor(255,255,255,255);
    
   
   

#ifdef KINECT
    
    if (kontorno) {
        ofPushMatrix();
        ofTranslate(160,0);
        ofScale(2.5);
        
        //grayImage.draw(0,0);
        //contourFinder.draw();
       
        ofSetHexColor(0xFFFF00);
        ofNoFill();
        int n = contourFinder.size();
        for (int i=0; i <n; i++) {
            /*ofPolyline convexHull = toOf(contourFinder.getFitQuad(i));
             convexHull.draw();
             ofSetHexColor(0xFF00FF);
             ofFill();
             for (int i=0;i<int(convexHull.size());i++) {
             ofDrawCircle(convexHull.getVertices()[i].x,convexHull.getVertices()[i].y,7);
             }
             
             // defects of the convex hull
             vector<cv::Vec4i> defects = contourFinder.getConvexityDefects(i);
             ofSetHexColor(0xFFFF00);
             for(int j = 0; j < defects.size(); j++) {
             ofDrawLine(defects[j][0], defects[j][1], defects[j][2], defects[j][3]);
             }*/
        }
        if (contourFinder.size()==1) {
            centroid1 = toOf(contourFinder.getCentroid(0));
            ofFill();
            ofSetColor(cyanPrint);
            ofDrawCircle(centroid1.x,240, 10);
            ofSetColor(255);
            ofDrawBitmapString("player 1",centroid1.x-20,240);
        }
        
        if (contourFinder.size()==2) {
            centroid1 = toOf(contourFinder.getCentroid(0));
            ofFill();
            ofSetColor(cyanPrint);
            ofDrawCircle(centroid1.x,240, 10);
            ofSetColor(255);
            ofDrawBitmapString("player 1",centroid1.x-20,240);
            centroid2 = toOf(contourFinder.getCentroid(1));
            ofSetColor(magentaPrint);
            ofDrawCircle(centroid2.x,240, 10);
            ofSetColor(255);
            ofDrawBitmapString("player 2",centroid2.x-20,240);
        }
        
        if (contourFinder.size()==3) {
            centroid1 = toOf(contourFinder.getCentroid(0));
            ofFill();
            ofSetColor(cyanPrint);
            ofDrawCircle(centroid1.x,240, 10);
            ofSetColor(255);
            ofDrawBitmapString("player 1",centroid1.x-20,240);
            centroid2 = toOf(contourFinder.getCentroid(1));
            ofSetColor(magentaPrint);
            ofDrawCircle(centroid2.x,240, 10);
            ofSetColor(255);
            ofDrawBitmapString("player 2",centroid2.x-20,240);
            centroid3 = toOf(contourFinder.getCentroid(2));
            ofSetColor(yellowPrint);
            ofDrawCircle(centroid3.x,240, 10);
            ofSetColor(255);
            ofDrawBitmapString("player 3",centroid3.x-20,240);
        }
        
        
        
        
        
            /*ofSetColor(yellowPrint);
            ofDrawBitmapString(ofToString(i),centroid);
        
            
            ofVec2f balance = toOf(contourFinder.getBalance(i));
            ofPushMatrix();
            ofTranslate(centroid.x, centroid.y);
            ofScale(5, 5);
            ofSetHexColor(0xFF0000);
            ofDrawLine(0, 0, balance.x, balance.y);
            ofPopMatrix();*/
        
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
        grayImage.draw(0,0,tamk.x,tamk.y);
        ofPopMatrix();
        ofSetColor(255);
    }
    
#endif
    
    if (info) {
        ofPushMatrix();
        ofTranslate(1440,10);
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
       //ofDrawBitmapString("CX: "+ ofToString(centroid.x),0,100);
        ofDrawBitmapString("gY: "+ ofToString(gravedadY),0,110);
        ofDrawBitmapString("TEST: "+ ofToString(teta),0,120);
        ofPopMatrix();
    }
    
    
    //float percent = ofMap(mouseX, 0, ofGetWidth(), 0, 1.0, true);
    ofVec2f punto(0,0);
    if (edges.size()>0) punto=edges[0]->getPointAtPercent(mouseX/1200.);
    
    ofNoFill();
    ofSetHexColor(0xEE00CC);
    ofDrawCircle(punto.x,punto.y+offset_fb_y,20);
    //ofDrawCircle(mx_promedio,offset_fb_y,20);
    ofSetHexColor(0xFFFFFF);
    
    
    
    if (play_shroom)
        shrooms.getFrame(frame_shroom)->draw(punto.x-48,mouseY,100,150);//shrooms.getFrame(frame_shroom)->draw(punto.x-48,punto.y+offset_fb_y-165,100,150);
    //polycallampa.draw();
    
    if (gui) {
        gui1.draw();
        gui2.draw();
        gui3.draw();
        gui4.draw();
        gui5.draw();
    }
}

void ofApp::allocate_fb(){
    
    
    //  -------------- framebuffer settings -----------------------
    
    fb_esporas.allocate(1920,1200,GL_RGBA);
    fb_esporas.begin();
    ofClear(0,0,0,255);
    fb_esporas.end();
    
    fb_player_1.allocate(fb_x,fb_y,GL_RGBA);
    fb_player_1.begin();
    ofClear(0,0,0,0);
    fb_player_1.end();
    
    fb_player_2.allocate(fb_x,fb_y,GL_RGBA);
    fb_player_2.begin();
    ofClear(0,0,0,0);
    fb_player_2.end();
    
    fb_player_3.allocate(fb_x,fb_y,GL_RGBA);
    fb_player_3.begin();
    ofClear(0,0,0,0);
    fb_player_3.end();
    
    fb_blur_X1.allocate(fb_x,fb_y,GL_RGBA);
    fb_blur_X1.begin();
    ofClear(0,0,0,0);
    fb_blur_X1.end();
    
    fb_blur_Y1.allocate(fb_x,fb_y,GL_RGBA);
    fb_blur_Y1.begin();
    ofClear(0,0,0,0);
    fb_blur_Y1.end();
    
    fb_blur_X2.allocate(fb_x,fb_y,GL_RGBA);
    fb_blur_X2.begin();
    ofClear(0,0,0,0);
    fb_blur_X2.end();
    
    fb_blur_Y2.allocate(fb_x,fb_y,GL_RGBA);
    fb_blur_Y2.begin();
    ofClear(0,0,0,0);
    fb_blur_Y2.end();
    
    fb_blur_X3.allocate(fb_x,fb_y,GL_RGBA);
    fb_blur_X3.begin();
    ofClear(0,0,0,0);
    fb_blur_X3.end();
    
    fb_blur_Y3.allocate(fb_x,fb_y,GL_RGBA);
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
    if (key == 'y') play_shroom=true;
    if (key == 'k') kontorno=!kontorno ;
    if (key == 'g') gui=!gui ;
    if (key == 'i') info=!info ;
    if (key == 'l') lineas=!lineas ;
    if (key == 'f') ofToggleFullscreen();
    if (key == 'q') color_fondo=!color_fondo;
    if (key == 'o') silueta=!silueta;
    if (key == 't') simpli=!simpli;
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
        for (int i=0;i<10;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-offset_fb_y); //
            particle->setRadius(0.3);
            particle->color.set(255,255,255,255);
            micelio_player_1.push_back(particle);
            
        }
    }
    
    if(key == 'x') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<5;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-offset_fb_y);
            particle->setRadius(0.3);
            micelio_player_2.push_back(particle);
        }
    }
    
    if(key == 'c') {
        //ofSetBackgroundAuto(false);
        for (int i=0;i<3;i++) {
            auto particle = make_shared<CustomParticle>(box2d.getWorld(), mouseX ,mouseY-offset_fb_y);
            particle->setRadius(0.3);
            micelio_player_3.push_back(particle);
        }
    }
    
if(key == 'v') {
    //ofSetBackgroundAuto(false);
    for (int i=0;i<50;i++) {
        auto particle = make_shared<CustomParticle>(box2d_esporas.getWorld(), mouseX ,mouseY);
        particle->setRadius(0.05);
        esporas.push_back(particle);
    }
}
        
        // want to save out some line...
        if(key == ' ') {
            ofstream f;
            f.clear();
            f.open(ofToDataPath("lines_.txt").c_str());
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
        edge->addVertex(lines.back()[i]+ofPoint(0,-offset_fb_y)); //0,-688
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
