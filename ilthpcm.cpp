#include "ilthpcm.h"

int main(int argc,char* argv[])
{
	/*Declarations*/
	int numStepsPerHour, numElements;
	Surface surface;
	vector<Layer> layers;
	vector<Weather> weatherData;

	/*Read data*/
	//Read input file
	readInputFile(numStepsPerHour, surface, layers, argv[1]);
	//Read weather data
	readWeatherData(weatherData, argv[2]);

	return 0;
}