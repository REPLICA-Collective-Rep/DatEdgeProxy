#pragma once
#include "ofMain.h"
#include "constants.h"

#include <regex>



static bool parseSensorData(const std::string & msg, SensorData &data  ){
    const std::string pttrn1 = "^sensors p(\\d) ((?:(?:[-+]?[0-9]*\\.?[0-9]+) ){8})(\\d+);(?:[\r\n]+)*$";
    const std::string pttrn2 = "([-+]?[0-9]*\\.?[0-9]+)";
    const std::regex re1(pttrn1);
    const std::regex re2(pttrn2);

    std::smatch match;
    std::smatch fmatch;
    if (std::regex_match(msg, match, re1) && match.size() > 1) {
        data.device              = ofToInt(match.str(1));
        data.mscounter           = ofToInt(match.str(3));

        std::string fstring = match.str(2);

        int i = 0;
        while(std::regex_search(fstring, fmatch, re2)) {
            if(i == DATE_NUM_CHANNELS)
                break;
            
            data.raw[i++] = ofToFloat( fmatch[1]);
            fstring = fmatch.suffix();
        }     
        
        if( i != DATE_NUM_CHANNELS ){
            ofLogWarning("Visualiser::threadedFunction") << "Wrong number of channels, " << i;
            return false;
        } else {
            return true;
        }
    } else {
        ofLogWarning("parseSensorData") << "Could not parse: '" << msg << "'";
        return false;
    }
}

static bool parseSensorData(const std::string & msg, int& device, int&mscounter, std::vector<float> & raw ){
    SensorData data;
    bool res = parseSensorData(msg, data );
    if(res){
        device    = data.device;
        mscounter = data.mscounter;
        copy(data.raw, data.raw+ DATE_NUM_CHANNELS , back_inserter(raw)); 
    }
    return res;
}