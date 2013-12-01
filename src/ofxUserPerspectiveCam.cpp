/*
*  ofxUserPerspectiveCam.h
*
* Copyright 2013 (c) Harpreet Sareen [harpreets] http://github.com/harpreets
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "ofxUserPerspectiveCam.h"

UserPerspectiveCam::UserPerspectiveCam(ofVec3f userPosition, float camZNear, float camZFar){
    setNearClip(camZNear);
    setFarClip(camZFar);
    setUserPosition(userPosition);
    setupCamParams();

    #ifdef DEBUG        
        cout<<"------ User Perspective Camera Initialised -------"<<"\n"
            <<"Near: "<<_camZNear<<", Far: "<<_camZFar<<"\n"
            <<"Left: "<<_camTopLeftNear.x<<", Right: "<<_camBottomRightNear.x<<"\n"
            <<"Bottom: "<<_camBottomRightNear.y<<", Top: "<<_camTopLeftNear.y<<"\n";
    #endif

}

/*
 Near clip for the camera.
 Usually kept low to keep a higher viewing volume
*/
void UserPerspectiveCam::setNearClip(float camZNear){
    this->_camZNear = camZNear;
}

/*
 Far clip for the camera. 
 In the scenario of real world cam simulation, far clip can be kept same as the device tracking range but to be avoid keeping too large to avoid depth buffering issue in OpenGL
*/
void UserPerspectiveCam::setFarClip(float camZFar){
    this->_camZFar = camZFar;
}

/*
 Method to establish the viewing volume/frustum and changing the perspective of the scene as per user's position
*/
void UserPerspectiveCam::begin(ofRectangle rect){
    saveGlobalMatrices();
  
    //Establish the viewing volume with glFrsutum (for assymetric projection). Left/Right and Bottom/Top coordinates are as per the z-near position of the position of the frusutm
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
          glFrustum(_left, _right, _bottom, _top, _camZNear, _camZFar);
    
    glMatrixMode(GL_MODELVIEW);
    
    glLoadIdentity();
    
    //Move the scene back into the real world view portal window by moving the camera opposite to the frusutm skewing shift
    if (_camTopLeftScreen.z < 0){
        gluLookAt(_wcUserHead.x, _wcUserHead.y, _wcUserHead.z, _wcUserHead.x, _wcUserHead.y, _wcUserHead.z-1, 0, 1, 0);
    }else{
        gluLookAt(_wcUserHead.x, _wcUserHead.y, _camTopLeftScreen.z, _wcUserHead.x, _wcUserHead.y, _camTopLeftScreen.z-1, 0, 1, 0);
    }
    
    ofViewport(rect);
}

/*
 Method to end the custom projection of the scene. 
*/
void UserPerspectiveCam::end(){
    
    loadGlobalMatrices();
    
    #ifdef DEBUG
        ofDrawBitmapString("Head : " + ofToString(_wcUserHead), ofPoint(150, 50));
        ofDrawBitmapString("WCS TL: " + ofToString(_wcTopLeftScreen), ofPoint(150,70));
        ofDrawBitmapString("WCS TR: " + ofToString(_wcTopRightScreen), ofPoint(150,90));
        ofDrawBitmapString("WCS BL: " + ofToString(_wcBottomLeftScreen), ofPoint(150,110));
        ofDrawBitmapString("WCS BR: " + ofToString(_wcBottomRightScreen), ofPoint(150,130));
        ofDrawBitmapString("WCS MID: " + ofToString(_wcMidScreen), ofPoint(150,150));
        ofDrawBitmapString("Cam TL Screen: " + ofToString((int)_camTopLeftScreen.x) + ", " + ofToString((int)_camTopLeftScreen.y) + ", " + ofToString((int)_camTopLeftScreen.z), ofPoint(150,170));
        ofDrawBitmapString("Cam BR Screen: " + ofToString((int)_camBottomRightScreen.x) + ", " + ofToString((int)_camBottomRightScreen.y) + ", " + ofToString((int)_camBottomRightScreen.z), ofPoint(150,190));
        ofDrawBitmapString("Cam TL Near : " + ofToString(_camTopLeftNear), ofPoint(150,210));
        ofDrawBitmapString("Cam BR Near: " + ofToString(_camBottomRightNear), ofPoint(150,230));
        ofDrawBitmapString("Shifted Position: " + ofToString((int)_shiftedPosition.x) + ", " + ofToString((int)_shiftedPosition.y) + ", " + ofToString((int)_shiftedPosition.z), ofPoint(150,250));
        ofDrawBitmapString("Shifted Position Near: " + ofToString(_shiftedPositionNear), ofPoint(150,270));
    #endif
}

/*
 Setting the real world view portal window for the screen where the projection will be displayed. 
 Coordinates for the window and user's position have to be in the same frame of reference
 Assumptions:
 Kinect's coordinates for world to camera coordinates
 X -> Positive torwards right of the device ,
 Y -> Positive going above the device and negative going below the device,
 Z -> Positive from the device towards the user
*/
void UserPerspectiveCam::setViewPortalWindow(ofVec3f wcTopLeft, ofVec3f wcTopRight, ofVec3f wcBottomLeft, ofVec3f wcBottomRight){
    this->_wcTopLeftScreen = wcTopLeft; this->_wcTopRightScreen = wcTopRight;
    this->_wcBottomLeftScreen = wcBottomLeft; this->_wcBottomRightScreen = wcBottomRight;
    
    float somelength = (_wcBottomLeftScreen - _wcBottomRightScreen).length();
    this->_Xs = (_wcBottomLeftScreen - _wcBottomRightScreen) / ( (_wcBottomLeftScreen - _wcBottomRightScreen).length() );
    this->_Ys = (_wcBottomLeftScreen - _wcTopLeftScreen) / ( (_wcBottomLeftScreen - _wcTopLeftScreen).length() );
    this->_Zs = _Xs.cross(_Ys);
    
    this->_height = (_wcTopLeftScreen - _wcBottomLeftScreen).length();
    this->_width = (_wcBottomRightScreen - _wcBottomLeftScreen).length();
    
    setupCamParams();
}

/*
 Method to update the position of the user and calculate the updated cam positions
 Input Params: Position of the user
 Assumptions:
 Kinect's coordinates for world to camera coordinates
 X -> Positive torwards right of the device ,
 Y -> Positive going above the device and negative going below the device,
 Z -> Positive from the device towards the user 
*/
void UserPerspectiveCam::setUserPosition(ofVec3f userPosition){
    this->_wcUserHead = userPosition;
    this->_VPs = (_wcBottomLeftScreen - _wcUserHead);
    
    setupCamParams();
}

//Private methods

/* 
 Method to calculate and update the camera coordinates. The method takes into account the user's input world coordinates (eg: from Kinect) and calculates the
 left/right, bottom/top parameters for the projection matrix frustum. In addition, the shift from assymetric frustum position is calculated for use with viewing transformation
 later with gluLookAt or for translating the world equivalent to the shift produced by skewing of the frustum
 Assumptions: 
 Kinect's coordinates for world to camera coordinates
 X -> Positive torwards right of the device ,
 Y -> Positive going above the device and negative going below the device,
 Z -> Positive from the device towards the user
*/
void UserPerspectiveCam::setupCamParams(){
    this->_camTopLeftScreen = _wcTopLeftScreen - _wcUserHead;
    this->_camBottomRightScreen = _wcBottomRightScreen - _wcUserHead;
    
    //If Z component becomes negative on the above operation and is negated subsequently to prevent inversion
    this->_camTopLeftNear = (_camTopLeftScreen/ ( (_camTopLeftScreen.z < 0) ? (-1*_camTopLeftScreen.z) : (_camTopLeftScreen.z) )) * _camZNear;
    this->_camBottomRightNear = (_camBottomRightScreen/ ( (_camBottomRightScreen.z < 0) ? (-1*_camBottomRightScreen.z ) : (_camBottomRightScreen.z) )) * _camZNear;
    
    this->_d = _VPs.dot(_Zs);
    this->_L = _VPs.dot(_Xs);
    this->_B = _VPs.dot(_Ys);
    this->_T = _height - _B;
    this->_R = _width - _L;
    
    this->_left = -_L * _camZNear/_d;
    this->_right = _R * _camZNear/_d;
    this->_bottom = -_B * _camZNear/_d;
    this->_top = _T * _camZNear/_d;
}

/*
 Method to save the world and projection matrices on the stack before doing transformations for projecton
*/
void UserPerspectiveCam::saveGlobalMatrices(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
}

/*
 Method to load the world and projection matrices that were popped on the stack before transformations on the original ones
*/
void UserPerspectiveCam::loadGlobalMatrices(){
    glMatrixMode(GL_PROJECTION);
    //Add code here to check if there is anything on the stack or if any error occurs when not three
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    //Add code here to check if there is anything on the stack or if any error occurs when not three
    glPopMatrix();
}

