//
//  ofxPSBlend.h
//
//  Created by Akira Hayasaka on 5/14/12.
//  Copyright (c) 2012 ﾟ･:,｡ﾟ･:,｡★ﾟ･:,｡ﾟ･:,｡☆ﾟ･:,｡ﾟ･:,｡★ﾟ･:,｡ﾟ･:,｡☆. All rights reserved.
//
#pragma once

#include "ofMain.h"

#define STRINGIFY(x) #x

static string psBlendFragShaderGL2 = "#version 120\n \
    #extension GL_ARB_texture_rectangle : enable\n" STRINGIFY(
                                                                                                                                                                                                    
    uniform sampler2DRect base;
    uniform sampler2DRect blendTgt;
    uniform int mode;
                                                                                                                                                                                                    
    void main()
    {
        vec4 baseCol = texture2DRect(base, gl_TexCoord[0].st);
        vec4 blendCol = texture2DRect(blendTgt, gl_TexCoord[0].st);
        vec4 result;
        // baseCol.rgb *= baseCol.a;
        // blendCol.rgb *= blendCol.a;
        if (mode == 0) // Normal
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result = blendCol;
            }
        }
        else if (mode == 1) // Multiply
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result = baseCol * blendCol;
            }
        }
        else if (mode == 2) // Screen
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result.r = (1.0 - ((1.0 - baseCol.r) * (1.0 - blendCol.r)));
                result.g = (1.0 - ((1.0 - baseCol.g) * (1.0 - blendCol.g)));
                result.b = (1.0 - ((1.0 - baseCol.b) * (1.0 - blendCol.b)));
            }
        }
        else if (mode == 3) // Overlay
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result.r = baseCol.r < 0.5 ? (2.0 * baseCol.r * blendCol.r) : (1.0 - 2.0 * (1.0 - baseCol.r) * (1.0 - blendCol.r));
                result.g = baseCol.g < 0.5 ? (2.0 * baseCol.g * blendCol.g) : (1.0 - 2.0 * (1.0 - baseCol.g) * (1.0 - blendCol.g));
                result.b = baseCol.b < 0.5 ? (2.0 * baseCol.b * blendCol.b) : (1.0 - 2.0 * (1.0 - baseCol.b) * (1.0 - blendCol.b));
            }
        }
        else if (mode == 4) // Difference
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result = abs(baseCol - blendCol);
            }
        }
        result.a = max(1.0, baseCol.a + blendCol.a); // 1.0;
        gl_FragColor = result;
    }
);

static string psBlendVertShaderGL3 = "#version 150\n" STRINGIFY(
                                                                
    uniform mat4 projectionMatrix;
    uniform mat4 modelViewMatrix;
    uniform mat4 textureMatrix;
    uniform mat4 modelViewProjectionMatrix;

    in vec4  position;
    in vec2  texcoord;
    in vec4  color;
    in vec3  normal;

    out vec2 texCoordVarying;

    void main()
    {
        texCoordVarying = texcoord;
        gl_Position = modelViewProjectionMatrix * position;
    }
);

static string psBlendFragShaderGL3 = "#version 150\n" STRINGIFY(

    in vec2 texCoordVarying;
    uniform sampler2DRect base;
    uniform sampler2DRect blendTgt;
    uniform int mode;
    out vec4 fragColor;

    void main()
    {
        vec4 baseCol = texture(base, texCoordVarying.st);
        vec4 blendCol = texture(blendTgt, texCoordVarying.st);
        vec4 result;
        // baseCol.rgb *= baseCol.a;
        // blendCol.rgb *= blendCol.a;
        if (mode == 0) // Normal
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result = blendCol;
            }
        }
        else if (mode == 1) // Multiply
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result = baseCol * blendCol;
            }
        }
        else if (mode == 2) // Screen
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result.r = (1.0 - ((1.0 - baseCol.r) * (1.0 - blendCol.r)));
                result.g = (1.0 - ((1.0 - baseCol.g) * (1.0 - blendCol.g)));
                result.b = (1.0 - ((1.0 - baseCol.b) * (1.0 - blendCol.b)));
            }
        }
        else if (mode == 3) // Overlay
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result.r = baseCol.r < 0.5 ? (2.0 * baseCol.r * blendCol.r) : (1.0 - 2.0 * (1.0 - baseCol.r) * (1.0 - blendCol.r));
                result.g = baseCol.g < 0.5 ? (2.0 * baseCol.g * blendCol.g) : (1.0 - 2.0 * (1.0 - baseCol.g) * (1.0 - blendCol.g));
                result.b = baseCol.b < 0.5 ? (2.0 * baseCol.b * blendCol.b) : (1.0 - 2.0 * (1.0 - baseCol.b) * (1.0 - blendCol.b));
            }
        }
        else if (mode == 4) // Difference
        {
            if (blendCol.a <= 0.0) {
                result = baseCol;
            } else {
                result = abs(baseCol - blendCol);
            }
        }
        result.a = max(1.0, baseCol.a + blendCol.a); // 1.0;
        fragColor = result;
    }
);

class ofxPSBlend
{
    
public:
    
    void setup(int width, int height)
    {
        base.allocate(width, height, GL_RGBA, ofFbo::maxSamples());
        quad.getVertices().resize(4);
        quad.getTexCoords().resize(4);
        quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        this->width = width;
        this->height = height;
        loadShader();
    }
    
    void allocate(int width, int height)
    {
        base.allocate(width, height, GL_RGBA, ofFbo::maxSamples());
        quad.getVertices().resize(4);
        quad.getTexCoords().resize(4);
        quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        this->width = width;
        this->height = height;
    }
    
    void loadShader()
    {
        if (ofIsGLProgrammableRenderer())
        {
            psBlendShader.setupShaderFromSource(GL_VERTEX_SHADER, psBlendVertShaderGL3);
            psBlendShader.setupShaderFromSource(GL_FRAGMENT_SHADER, psBlendFragShaderGL3);
            psBlendShader.bindDefaults();
        }
        else
        {
            psBlendShader.setupShaderFromSource(GL_FRAGMENT_SHADER, psBlendFragShaderGL2);
        }
        psBlendShader.linkProgram();
    }
    
    void begin()
    {
        ofPushStyle();
        ofPushMatrix();
        base.begin();
        ofClear(0, 0, 0);
    }
    
    void end()
    {
        base.end();
        ofPopStyle();
        ofPopMatrix();
    }
    
    void draw(ofTexture& blendTgt, int blendMode = 0)
    {
        psBlendShader.begin();
        psBlendShader.setUniformTexture("base", base, 1);
        psBlendShader.setUniformTexture("blendTgt", blendTgt, 2);
        psBlendShader.setUniform1i("mode", blendMode);
        drawPlane(width, height);
        psBlendShader.end();
    }
    
    string getBlendMode(int blendMode)
    {
        string blendName = "no such blend hiii";
        if (blendMode == 0)
        {
            blendName = "BlendNormal";
        }
        else if (blendMode == 1)
        {
            blendName = "BlendMultiply";
        }
        else if (blendMode == 2)
        {
            blendName = "BlendAverage";
        }
        else if (blendMode == 3)
        {
            blendName = "BlendAdd";
        }
        else if (blendMode == 4)
        {
            blendName = "BlendSubstract";
        }
        else if (blendMode == 5)
        {
            blendName = "BlendDifference";
        }
        else if (blendMode == 6)
        {
            blendName = "BlendNegation";
        }
        else if (blendMode == 7)
        {
            blendName = "BlendExclusion";
        }
        else if (blendMode == 8)
        {
            blendName = "BlendScreen";
        }
        else if (blendMode == 9)
        {
            blendName = "BlendOverlay";
        }
        else if (blendMode == 10)
        {
            blendName = "BlendSoftLight";
        }
        else if (blendMode == 11)
        {
            blendName = "BlendHardLight";
        }
        else if (blendMode == 12)
        {
            blendName = "BlendColorDodge";
        }
        else if (blendMode == 13)
        {
            blendName = "BlendColorBurn";
        }
        else if (blendMode == 14)
        {
            blendName = "BlendLinearLight";
        }
        else if (blendMode == 15)
        {
            blendName = "BlendVividLight";
        }
        else if (blendMode == 16)
        {
            blendName = "BlendPinLight";
        }
        else if (blendMode == 17)
        {
            blendName = "BlendHardMix";
        }
        else if (blendMode == 18)
        {
            blendName = "BlendReflect";
        }
        else if (blendMode == 19)
        {
            blendName = "BlendGlow";
        }
        else if (blendMode == 20)
        {
            blendName = "BlendPhoenix";
        }
        else if (blendMode == 21)
        {
            blendName = "BlendHue";
        }
        else if (blendMode == 22)
        {
            blendName = "BlendSaturation";
        }
        else if (blendMode == 23)
        {
            blendName = "BlendColor";
        }
        else if (blendMode == 24)
        {
            blendName = "BlendLuminosity";
        }
        return blendName;
    }
    
protected: // Dragon!
    
    void drawPlane(float _width, float _height)
    {
        quad.setVertex(0, ofVec3f(0,0,0));
        quad.setVertex(1, ofVec3f(_width,0,0));
        quad.setVertex(2, ofVec3f(_width,_height,0));
        quad.setVertex(3, ofVec3f(0,_height,0));
        
        quad.setTexCoord(0, ofVec2f(0,0));
        quad.setTexCoord(1, ofVec2f(_width,0));
        quad.setTexCoord(2, ofVec2f(_width,_height));
        quad.setTexCoord(3, ofVec2f(0,_height));
        
        quad.draw();
    }
    
    ofFbo base;
    ofShader psBlendShader;
    float width;
    float height;
    ofVboMesh quad;
    
};
