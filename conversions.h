#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <iostream>
#include <string>
#include <sstream>
#include <QtMath>
#include <QString>

using namespace std;

//static double EARTH_RADIUS = 6371009;

// Converts degrees to radians.
#define toRadians(angleDegrees) (angleDegrees * M_PI / 180.0)
// Converts radians to degrees.
#define toDegrees(angleRadians) (angleRadians * 180.0 / M_PI)

template <class T>
int numDigits(T number)
{
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

namespace DMS
{
static const char *DEG_SIM = "\u00B0";
//
//  Convert decimal degrees to degrees, minutes and seconds
//

inline std::string DegreesMinutesSeconds(double ang, unsigned int num_dec_places = 2)
{
    bool neg(false) ;
    if (ang < 0.0)
    {
        neg = true ;
        ang = -ang ;
    }

    int deg = (int)ang ;
    double frac = ang - (double)deg ;

    frac *= 60.0 ;

    int min = (int)frac ;

    frac = frac - (double)min ;

    // fix the DDD MM 60 case
    // TODO: nearbyint isn’t alway available (Visual C++,
    //       for example)
    double sec = nearbyint(frac * 600000.0) ;
    sec /= 10000.0 ;

    if (sec >= 60.0)
    {
        min++ ;
        sec -= 60.0 ;
    }

    std::ostringstream oss ;

    if (neg)
    {
        oss << "-" ;
    }

    //  TODO: allow user to define delimiters separating
    //        degrees, minutes, and seconds.
    oss.setf(std::ios::fixed, std::ios::floatfield);

    oss << deg << std::string(" ");// << DEG_SIM ;

    oss.fill('0') ;
    oss.width(2) ;
    oss << min << std::string(" ");// << "\'" ;
    if (num_dec_places == 0)
    {
        oss.width(2) ;
        oss.precision(0) ;
    }
    else
    {
        oss.width(num_dec_places + 3) ;
        oss.precision(num_dec_places) ;
    }
    oss << sec;//  << "\"" ;

    return oss.str() ;
}


inline std::string DegreesMinutesSecondsLat(double ang,
                                            unsigned int num_dec_places = 2)
{
    std::string lat(DegreesMinutesSeconds(ang, num_dec_places)) ;

    if (lat[0] == '-')
    {
        lat = std::string("S ") + lat;
        lat.erase(0, 1) ;
    }
    else
    {
        lat = std::string("N ") + lat ;
    }
    return lat ;
}


inline std::string DegreesMinutesSecondsLon(double ang,
                                            unsigned int num_dec_places = 2)
{
    std::string lon(DegreesMinutesSeconds(ang, num_dec_places)) ;

    if (lon[0] == '-')
    {
        lon = std::string("W ") + lon;
        lon.erase(0, 1) ;
    }
    else
    {
        lon = std::string("E ") + lon ;
    }

    return lon ;
}

float dms2deg(int d, int m, int s) // convert dms to deg
{
    float ans;
    ans = d + (m/60.0) + (s/360.0); // very simple formula
    return ans;
}

//
// Convert degrees, minutes and seconds to decimal degrees.
//
inline double DecimalDegrees(const std::string& dms)
{
    std::string tmp(dms) ;

    // need to handle the -0 MM SS case
    // TODO: handle leading spaces before potential minus sign
    bool neg(false) ;

    if ((tmp[0] == 'W') || (tmp[0] == 'S') ||
            (tmp[0] == '-'))
    {
        neg = true ;
    }

    tmp.erase(0, 1);

    auto angle = QString::fromLocal8Bit(tmp.c_str());

    auto deg = angle.mid(0, 2).toDouble();
    auto min = angle.mid(2, 2).toDouble();
    auto sec = angle.mid(4, 5).toDouble();

    double ang = deg + (min/60) + (sec/3600);

    if (neg)
    {
        ang = -ang ;
    }

    return ang ;
}

double DDMMmmmToDecimalLat(const wstring &coord, const wstring &hems)
{
    auto coorddegree = coord.substr(0,2);
    auto coordminute = coord.substr(2, 5);
    coordminute = coordminute.substr(0,2) + L"." + coordminute.substr(2, 3);
    double latcoordminute = stod(coordminute);
    latcoordminute = latcoordminute / 60;

    double result = std::stod(coorddegree + L".") + latcoordminute;
    if (hems == L"S")
    {
        result = -1*result;
    }
    return result;
}

double DDMMmmmToDecimalLon(const wstring &coord, const wstring &hems)
{
    auto coorddegree = coord.substr(0,3);
    auto coordminute = coord.substr(3, 5);
    coordminute = coordminute.substr(0,2) + L"." + coordminute.substr(2, 3);
    double latcoordminute = stod(coordminute);
    latcoordminute = latcoordminute / 60;
    double result = stod(coorddegree + L".") + latcoordminute;
    if (hems == L"W")
    {
        result = -1*result;
    }
    return result;
}

} ;


#endif // CONVERSIONS_H
