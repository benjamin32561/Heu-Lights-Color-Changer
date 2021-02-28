#ifndef _WINHTTPHANDLER_H
#define _WINHTTPHANDLER_H

#include "WSAInitializer.h"
#include <atlimage.h>
#include "opencv.hpp"
#include <thread>

typedef struct rgb
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
}rgb;

#define IP "10.100.102.3"
#define PORT 80

class All
{
public:
	static rgb screenshot();
    static void send();
    static std::string buildJson(rgb color);
	static float Min(float fR, float fG, float fB);
	static float Max(float fR, float fG, float fB);
	static void RGBToHSL(rgb color, int& H, int& S, int& L);
	static void sendData(std::string data);
};

#endif
