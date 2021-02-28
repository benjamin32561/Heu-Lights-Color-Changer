#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "WinHttpHandler.h"

rgb All::screenshot()
{
	// get the device context of the screen
	HDC hScreenDC = GetDC(NULL);
	BITMAPINFOHEADER  bi;

	// and a device context to put it in
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	int width = GetDeviceCaps(hScreenDC, HORZRES);
	int height = GetDeviceCaps(hScreenDC, VERTRES);

	// maybe worth checking these are positive values
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

	// get a new bitmap
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

	cv::Mat image;
	image.create(height, width, CV_8UC4);

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	GetDIBits(hMemoryDC, hBitmap, 0, (UINT)height, image.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteDC(hMemoryDC);
	DeleteDC(hScreenDC);

	rgb toRet;
	toRet.b = 0;
	toRet.g = 0;
	toRet.r = 0;

	int totalPix = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j+=4)//checking only every 4th pixel
		{
			cv::Vec4b colors = image.at<cv::Vec4b>(i, j);
			toRet.r += colors[2];
			toRet.g += colors[1];
			toRet.b += colors[0];
			totalPix++;
		}
	}
	toRet.b /= totalPix;
	toRet.g /= totalPix;
	toRet.r /= totalPix;
	return toRet;
}

void All::send()
{
	std::string data = buildJson(screenshot());
	std::string msg_light_bulb = "PUT /api/oiSqZiULDYz-VuO43lSA6nsG2IHjQW2wWsYOVgTP/lights/12/state/ HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ";
	std::string msg_light_strip = "PUT /api/oiSqZiULDYz-VuO43lSA6nsG2IHjQW2wWsYOVgTP/lights/11/state/ HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ";
	msg_light_bulb += std::to_string(data.size()) + "\r\n\r\n" + data;
	msg_light_strip += std::to_string(data.size()) + "\r\n\r\n" + data;
	sendData(msg_light_bulb);
	sendData(msg_light_strip);
}

std::string All::buildJson(rgb color)
{
	int bri = 0, sat = 0, hue = 0;
	RGBToHSL(color, hue, sat, bri);
	//bri = 150;
	std::string data = "{\"on\":true, \"bri\":" +
		std::to_string(bri) + ", \"sat\":" + std::to_string(sat) +
		", \"hue\":" + std::to_string(hue) + "}";
	return data;
}

float All::Min(float fR, float fG, float fB)
{
	float fMin = fR;
	if (fG < fMin)
	{
		fMin = fG;
	}
	if (fB < fMin)
	{
		fMin = fB;
	}
	return fMin;
}

float All::Max(float fR, float fG, float fB)
{
	float fMax = fR;
	if (fG > fMax)
	{
		fMax = fG;
	}
	if (fB > fMax)
	{
		fMax = fB;
	}
	return fMax;
}

void All::RGBToHSL(rgb color, int& H, int& S, int& L)
{
	int r = color.r;
	int g = color.g;
	int b = color.b;

	float fR = r / 255.0;
	float fG = g / 255.0;
	float fB = b / 255.0;


	float fCMin = Min(fR, fG, fB);
	float fCMax = Max(fR, fG, fB);


	L = 50 * (fCMin + fCMax);

	if (fCMin == fCMax)
	{
		S = 0;
		H = 0;
		return;
	}
	else if (L < 50)
	{
		S = 100 * (fCMax - fCMin) / (fCMax + fCMin);
	}
	else
	{
		S = 100 * (fCMax - fCMin) / (2.0 - fCMax - fCMin);
	}

	if (fCMax == fR)
	{
		H = 60 * (fG - fB) / (fCMax - fCMin);
	}
	if (fCMax == fG)
	{
		H = 60 * (fB - fR) / (fCMax - fCMin) + 120;
	}
	if (fCMax == fB)
	{
		H = 60 * (fR - fG) / (fCMax - fCMin) + 240;
	}
	if (H < 0)
	{
		H = H + 360;
	}
}

void All::sendData(std::string data)
{
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(IP);
	int status = ::connect(clientSocket, (struct sockaddr*)&sa, sizeof(sa));
	if (status == INVALID_SOCKET)
		std::cout << "Cant connect to server" << std::endl;
	::send(clientSocket, data.c_str(), data.size() + 1, 0);
	char d = NULL;
	std::string a = "";
	while (d != ']')
	{
		recv(clientSocket, &d, 1, 0);
		a += d;
	}
	closesocket(clientSocket);
}
