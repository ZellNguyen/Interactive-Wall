#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxKinect kinect;
    ofxCvColorImage rgb, hsv;
    ofxCvGrayscaleImage hue, sat, bri, filtered;
    ofxCvGrayscaleImage grayImg;
    
    ofxCvContourFinder contourPlanet;
    
    float c_Hue, r_Hue, c_Sat, r_Sat, c_Value, r_Value, c_Rad;
    
    int angle;
    
    ofEasyCam easyCam;
    
    bool isInteract;
    
    vector<cv::Vec3f> circles, circles_alter;
    cv::Mat grayCV;
    
    float centreX, centreY, rad;
    
    ofFile position;
};
