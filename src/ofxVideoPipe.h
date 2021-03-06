#pragma once

#include "ofMain.h"

class ofxVideoPipe : public ofThread {
    class PPMHeader {
    public:
        PPMHeader();
        PPMHeader(PPMHeader &other);
        int dataSize();
        bool good();
        void reset();
        string type;
        int width;
        int height;
        int depth;
        int channels;
        stringstream error;
    };
    
    class PPMFrame : public ofBuffer {
    public:
        PPMFrame(){};
        void reset();
        void set(const char * data){ ofBuffer::set(data, dataSize()); }
        void writeTo(ofPixels & pixels);
        int dataSize(){ return header.dataSize(); }
        bool good(){ return header.good(); }
        string errors(){ return header.error.str(); }
        int getWidth(){ return header.width; }
        int getHeight(){ return header.height; }
        PPMHeader header;
    };


public:
    struct onSizeChangedData {
        onSizeChangedData(int w, int h) : width(w), height(h) {};
        int width;
        int height;
    };
    
    class ReadError : public std::runtime_error {
    public:
        ReadError() : std::runtime_error("Error reading from pipe") {};
        ReadError (const string& message) : std::runtime_error(message) {};
    } readError;
    
    ofxVideoPipe() : isFrameChanged(false), isPipeOpen(false), filename("") {};
    
    void open(string _filename);
    void close();
    void draw(int x, int y);
    void threadedFunction();
    void update();
    
    bool isFrameNew();
    ofPixelsRef getPixelsRef();
    void updatePixels();
    void setFrameRate(float targetRate);
    
    int getWidth(){ return currentFrame.getWidth(); }
    int getHeight(){ return currentFrame.getHeight(); }
    
    ofEvent< onSizeChangedData > onSizeChanged;
    
private:
    void readFrame () throw();
    string readLine() throw(ReadError);
    void readHeader() throw(ReadError);
    
    void idle();
    
    int openPipe();
    void closePipe();
    
    PPMFrame currentFrame;
    string lastLine;
    ofPixels pixels;
    ofImage frameImage;
    ofFile pipe;
    string filename;
    bool isFrameChanged;
    bool isPixelsChanged;
    bool isFrameRateSet;
    bool isPipeOpen;
    int prevMillis, millisForFrame;
    
    enum openPipeResult {
        OPEN_PIPE_SUCCESS = 0,
        OPEN_PIPE_INIT_FAIL = -1,
        OPEN_PIPE_FD_FAIL = -2,
        OPEN_PIPE_SELECT_FAIL = -3,
        OPEN_PIPE_TIMEOUT = -4
    };
    
    void handlePipeReadError() throw(ReadError);
};