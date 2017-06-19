#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    kinect.setRegistration(true);
    
    kinect.init();
    
    kinect.open();
    
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    rgb.allocate(kinect.width, kinect.height);
    hsv.allocate(kinect.width, kinect.height);
    hue.allocate(kinect.width, kinect.height);
    bri.allocate(kinect.width, kinect.height);
    sat.allocate(kinect.width, kinect.height);
    filtered.allocate(kinect.width, kinect.height);
    
    grayImg.allocate(kinect.width, kinect.height);
    
    ofSetFrameRate(60);
    
    // zero the tilt on startup
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    c_Hue = 107;
    r_Hue = 20;
    c_Sat = 192;
    r_Sat = 80;
    c_Value = 173;
    r_Value = 120;
    
    isInteract = false;
    c_Rad = 13;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100, 100, 100);
    
    kinect.update();
    
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        isInteract = false;
        
        // load color image from the kinect source
        rgb.setFromPixels(kinect.getPixels());
        rgb.mirror(false, true);
        
        //remove noise
        rgb.blurGaussian(1);
        rgb.erode();
        rgb.dilate();
        
        //duplicate the image
        hsv = rgb;
        
        filtered = rgb;
        
        //convert image from rgb to hsv;
        hsv.convertRgbToHsv();
        
        //store 3 channels
        hsv.convertToGrayscalePlanarImages(hue, sat, bri);
        
        //filter the color by hue value
        for(int i = 0; i < (kinect.width * kinect.height); i++){
            filtered.getPixels()[i] = ( ofInRange(hue.getPixels()[i], c_Hue-r_Hue, c_Hue+r_Hue) && ofInRange(sat.getPixels()[i], c_Sat-r_Sat, c_Sat+r_Sat) && ofInRange(bri.getPixels()[i], c_Value-r_Value, c_Value+r_Value) ) ? 255 : 0;
        }
        
        filtered.erode();
        filtered.dilate();
        filtered.blur(1);
        
        //depth Image
        grayImg = rgb;
        grayImg.blurGaussian(7);
        grayCV = cv::Mat(cv:: Size (grayImg.width, grayImg.height),CV_8U,grayImg.getPixels());
        if(circles.size() != 0){
            circles_alter = circles;
        }
        cv::HoughCircles(grayCV, circles, CV_HOUGH_GRADIENT, 1, grayCV.rows/8, 200, 35, 7, 200);
        
        if(circles.size() == 0){
            circles = circles_alter;
        }
        
        for( size_t i = 0; i < circles.size(); i++ ){
            rad = circles[i][2];
            if(rad < 19 && rad > 10){
                centreX = circles[i][0];
                centreY = circles[i][1];
                isInteract = ofInRange(rad, c_Rad-3, c_Rad+3);
            }
        }
        
        position.open("position.txt",ofFile::WriteOnly);
        position << isInteract << endl << centreX << endl << centreY << endl;
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255,255,255);
    
    //draw all cv images
    rgb.draw(0,0);
    hsv.draw(kinect.width,0);
    //hue.draw(0,kinect.height);
    //sat.draw(kinect.width,kinect.height);
    //bri.draw(0,kinect.height*2);
    filtered.draw(kinect.width*2, 0);
    //contourPlanet.draw(kinect.width, kinect.height);
    
    grayImg.draw(0, kinect.height);
    
    /*ofSetColor(0, 255, 0);
     ofFill();
     for (int i=0; i<contourOrbit.nBlobs; i++) {
     ofCircle(contourOrbit.blobs[i].centroid.x, contourOrbit.blobs[i].centroid.y, 40);
     }*/

    /// Draw the circles detected
    ofSetColor(0,0,255);
    ofFill();
    for( size_t i = 0; i < circles.size(); i++ )
    {
        ofDrawCircle(circles[i][0], circles[i][1], circles[i][2]);
        stringstream infoStream;
       
        infoStream << "Radius: " << circles[i][2] << endl;
        ofDrawBitmapString(infoStream.str(), circles[i][0], circles[i][1]);
    }
    
    
    stringstream infoStream;
    infoStream << "Central Hue: " << c_Hue << endl
    << "Range Hue: " << r_Hue << endl
    << "Central Sat: " << c_Sat << endl
    << "Range Sat: " << r_Sat << endl
    << "Central Value: " << c_Value << endl
    << "Range Value: " << r_Value << endl
    << isInteract << endl;
    ofDrawBitmapString(infoStream.str(), kinect.width * 2 + 50, kinect.height);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'c':
            kinect.setCameraTiltAngle(0); // zero the tilt
            kinect.close();
            break;
            
        case '1':
            kinect.setLed(ofxKinect::LED_GREEN);
            break;
            
        case '2':
            kinect.setLed(ofxKinect::LED_YELLOW);
            break;
            
        case '3':
            kinect.setLed(ofxKinect::LED_RED);
            break;
            
        case '4':
            kinect.setLed(ofxKinect::LED_BLINK_GREEN);
            break;
            
        case '5':
            kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
            break;
            
        case '0':
            kinect.setLed(ofxKinect::LED_OFF);
            break;
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case 'w':
            c_Rad++;
            break;
            
        case 's':
            c_Rad--;
            break;
            
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
