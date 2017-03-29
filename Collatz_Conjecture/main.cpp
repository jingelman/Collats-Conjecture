
#include <vector>
#include <limits>
#include <random>

#include "Image.h"

#define PI 3.14159265

#define VECTORSIZE 40

#define WIDTH 600
#define HEIGHT 600

#define LINELENGTH 10

#define DEFAULTANGLE 90
#define ANGLECHANGE 15

#define MAXVALUE 500
#define MINVALUE 20

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

int randomGenerator(const int min, const int max)
{
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

int findMax(const vector<vector<uint32_t>> &vec)
{
	int maxVal = 0;

	for(int i = 0; i < vec.size(); ++i)
	{
		if (maxVal < vec[i][0])
			maxVal = vec[i][0];
	}

	return maxVal;
}

void fillWithRandom(vector<uint32_t> &vec)
{
	int startVal, size = vec.size();

	for (int i = 0; i < size; ++i)
	{
		startVal = randomGenerator(MINVALUE, MAXVALUE);
		vec[i] = startVal;
	}
}

void BresenhamsLineAlgorithm(TGAImage img, const Colour c,  int x0,  int y0,  int x1,  int y1)
{
	const bool steep = fabs(y1 - y0) > fabs(x1 - x0);

	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	
	float deltax = x1 - x0;
	float deltay = fabs(y1 - y0);

	if(abs(deltax) < std::numeric_limits<float>::epsilon())	
		return;
	
	float deltaerr = fabs(deltay / deltax);
	float error = deltaerr - 0.5f;

	const int ystep = (y0 < y1) ? 1 : -1;

	int y = y0;

	for(int x = x0; x < x1; ++x)
	{
		if (steep && (y > 0 && y < WIDTH && x > 0 && x < HEIGHT))
			img.setPixel(c, y, x);

		else if (x > 0 && x < WIDTH && y > 0 && y < HEIGHT)
			img.setPixel(c, x, y);
		
		error += deltaerr;
		if(error >= 0.5f)
		{
			y += ystep;
			error -= 1.0f;
		}
	}
}

void drawCollatzConjecture(TGAImage &img, const vector<uint32_t> &vec)
{
	Colour lineColor;
	lineColor.r = randomGenerator(0, 255);
	lineColor.g = randomGenerator(0, 255);
	lineColor.b = randomGenerator(0, 255);
	lineColor.a = 1;

	int angle = DEFAULTANGLE;
	int xstart = WIDTH/2;
	int ystart = HEIGHT/2;
	int xend, yend;

	for(int i = vec.size()-1; i >= 1; --i)
	{
		if(vec[i] < vec[i-1])
		{
			angle += ANGLECHANGE;
		}
		else
		{
			angle -= ANGLECHANGE;
		}

		xend = xstart+round(LINELENGTH * cos(angle*PI / 180));
		yend = ystart+round(LINELENGTH * sin(angle*PI / 180));

		BresenhamsLineAlgorithm(img, lineColor, xstart, ystart, xend, yend);
		
		xstart = xend;
		ystart = yend;
	}
}

int main()
{
	vector<vector<uint32_t>> cc(VECTORSIZE);
	vector<uint32_t> n(VECTORSIZE);

	fillWithRandom(n);
		
	//#pragma omp parallel
	for (uint32_t i= 0; i < VECTORSIZE; ++i)
	{
		while (n[i] != 1)
		{
			cc[i].push_back(n[i]);
			n[i] = n[i] % 2 ? 3 * n[i] + 1 : 0.5*n[i];
		}
	}

	TGAImage *img = new TGAImage(WIDTH, HEIGHT);
	const char imageName[20] = "result.tga";

	Colour color;
	color.r = color.g = color.b = color.a = 255;

	for (int j = 0; j < WIDTH; ++j)
	{
		for (int i = 0; i < HEIGHT; ++i)
		{
			img->setPixel(color, i, j);
		}
	}
	
	//int maxVal = findMax(cc);

	for(const auto &v : cc)
	{
		drawCollatzConjecture(*img, v);
	}

	
	img->WriteImage(imageName);

	delete img;

	return 0;
}