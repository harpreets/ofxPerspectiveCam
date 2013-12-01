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


#ifndef __HolographicAR__UserPerspectiveCam__
#define __HolographicAR__UserPerspectiveCam__

#include <iostream>
#include "ofMain.h"

#define DEFAULT_CAM_NEARPOS 1.0
#define DEFAULT_CAM_FARPOS 1000.0
#define ORIGN 0

class UserPerspectiveCam : public ofCamera {
public:
    UserPerspectiveCam(ofVec3f userPosition = ofVec3f(ORIGN,ORIGN,ORIGN), float camZNear = DEFAULT_CAM_NEARPOS, float camZFar = DEFAULT_CAM_FARPOS);
    virtual void begin(ofRectangle viewport = ofGetWindowRect());
    virtual void end();
    
    //Setter Methods
    void setNearClip(float);
    void setFarClip(float);
    void setUserPosition(ofVec3f);
    void setViewPortalWindow(ofVec3f, ofVec3f, ofVec3f, ofVec3f);
    
    //Getter Methods
    float getNearClip();
    float getFarClip();
private:
    void saveGlobalMatrices();
    void loadGlobalMatrices();
    void setupCamParams();
    
    ofVec3f _wcTopLeftScreen, _wcTopRightScreen, _wcBottomLeftScreen, _wcBottomRightScreen,
            _camTopLeftScreen, _camBottomRightScreen,
            _camTopLeftNear, _camBottomRightNear ;
    ofVec3f _wcUserHead;
    float _camZNear, _camZFar;
    
    
    ofVec3f _Xs, _Ys, _Zs, _VPs;
    float _L, _R, _T, _B, _height, _width;
    float _left, _right, _bottom, _top;
    float _d;
};

#endif /* defined(__HolographicAR__UserPerspectiveCam__) */
