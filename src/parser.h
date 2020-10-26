#pragma once
#include "ofMain.h"
#include "constants.h"
#include <regex>



bool parseSensorData(const std::string & data, int& device, int&mscounter, std::vector<float> & raw ){
    const std::string pttrn1 = "^sensors p(\\d) ((?:(?:[-+]?[0-9]*\\.?[0-9]+) ){8})(\\d+);(?:[\r\n]+)*$";
    const std::string pttrn2 = "([-+]?[0-9]*\\.?[0-9]+)";
    const std::regex re1(pttrn1);
    const std::regex re2(pttrn2);

    std::smatch match;
    std::smatch fmatch;


    if (std::regex_match(data, match, re1) && match.size() > 1) {
        device              = ofToInt(match.str(1));
        mscounter           = ofToInt(match.str(3));
        std::string fstring = match.str(2);

        raw.clear();
        raw.reserve(DATE_NUM_CHANNELS);
        int i = 0;
        while(std::regex_search(fstring, fmatch, re2)) {
            if(i >= DATE_NUM_CHANNELS)
                break;
            
            raw.push_back( ofToFloat( fmatch[1]));
            fstring = fmatch.suffix();
        }     
        
        if(raw.size() != DATE_NUM_CHANNELS ){
            ofLogWarning("Visualiser::threadedFunction") << "Wrong number of channels, " << raw.size();
            return false;
        } else {
            return true;
        }
    } else {
        ofLogWarning("parseSensorData") << "Could not parse: '" << data << "'";
        return false;
    }

}