#pragma once
#include "ofMain.h"
#include "constants.h"

#include <regex>

struct SensorData {
    int device;
    double mscounter;
    float raw[DATE_NUM_CHANNELS];
};

struct OutputData {
    int device;
    float loss;
    float embedding[Z_DIM];
};


// static bool parseSensorData(const std::string & _msg, SensorData &data  ){
//     const std::string pttrn1 = R"(^sensors p(\d) ((?:(?:[-+]?[0-9]*\.?[0-9]+) ){8})(.+).*$)";
//     const std::string pttrn2 = R"(([-+]?[0-9]*\.?[0-9]+))";
//     const std::regex re1(pttrn1);
//     const std::regex re2(pttrn2);

//     std::string msg = _msg.substr(0, _msg.find(";", 0));

//     std::smatch match;
//     std::smatch fmatch;
//     if (std::regex_match(msg, match, re1) ) {
//         data.device              = ofToInt(match.str(1));
//         data.mscounter           = ofFromString<double>(match.str(3));

//         std::string fstring = match.str(2);

//         int i = 0;
//         while(std::regex_search(fstring, fmatch, re2)) {
//             if(i == DATE_NUM_CHANNELS)
//                 break;
            
//             data.raw[i++] = ofFromString<double>(fmatch[1]);
//             fstring = fmatch.suffix();
//         }     

//         if( i != DATE_NUM_CHANNELS ){
//             ofLogWarning("Visualiser::threadedFunction") << "Wrong number of channels, " << i;
//             return false;
//         } else {
//             return true;
//         }
//     } else {
//         ofLogWarning("parseSensorData") << "Could not parse: '" << msg << "'";
//         return false;
//     }
// }



// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s, const char* t)
{
    return ltrim(rtrim(s, t), t);
}

static bool parseSensorData(const std::string & msg, SensorData &data  ){
    std::string rs(msg.begin(), msg.begin() + msg.find_first_of(";") + 1);
    const char* _ws = " \t\n\r\f\v";
    rs = trim(rs, _ws);
    if (!rs.empty() &&
            rs[rs.length()-1] == ';')
    {
        // remove ";"
        rs.pop_back();

        vector<string> strings;
        istringstream f(rs);
        string s;
        while (getline(f, s, ' '))
        {
            strings.push_back(s);
        }

        // 1 topic + 1 id + 8 sensors + 1 timestamp == 11
        // check topic again?
        if (strings.size() == 11
                && strings[0] == "sensors")
        {
            // timestamp is the last in list
            data.mscounter = ofFromString<double>(strings[strings.size()-1]);
//                    data.timestamp = ofGetElapsedTimeMicros();

            for (int i=0; i<8; i++)
            {
                data.raw[i] = ofFromString<double>(strings[2+i]);
            }

            // need to check size?
  
            if (strings[1].find("p1") != std::string::npos)
            {
                data.device = 1;
            }
            else if (strings[1].find("p2") != std::string::npos)
            {
                data.device = 2;
            }
            else if (strings[1].find("p3") != std::string::npos)
            {
                data.device = 3;
            }
            else if (strings[1].find("p4") != std::string::npos)
            {
                data.device = 4;
            }
            else if (strings[1].find("p5") != std::string::npos)
            {
                data.device = 5;
            }
            else if (strings[1].find("p6") != std::string::npos)
            {
                data.device = 6;
            } else {
                ofLogError() << "malformatted string: " << strings[1];
                return false;
            }
            return true;
        }
        else
        {
            ofLogError() << "malformatted string: " << rs;
            return false;
        }
    } else {
        ofLogError() << "malformatted string: " << rs;
        return false;
    }
}

