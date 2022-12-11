#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <math.h>

using namespace std;

/*Class declarations*/
class Surface
{
	public:
		int surfaceCode;
		double isothermalAlbedo;
		double isothermalEmissivity;
		double T1therm;
		double T2therm;
		double albedoLow;
		double albedoHigh;
		
	Surface()
	{
		surfaceCode = -1;
		isothermalAlbedo = NAN;
		isothermalEmissivity = NAN;
		T1therm = NAN;
		T2therm = NAN;
		albedoLow = NAN;
		albedoHigh = NAN;
	}
};

class Layer
{
	public:
		int layerCode;
		int numLayerElements;
		double thickness;
		double solidMatrixThermalConductivity;
		double solidMatrixHeatCapacity;
		double solidMatrixDensity;
		double solidMatrixFraction;
		double PCMLowerTransitionTemp, PCMUpperTransitionTemp;
		double PCMSolidThermalConductivity, PCMLiquidThermalConductivity;
		double PCMSolidHeatCapacity, PCMLiquidHeatCapacity;
		double PCMDensity, PCMLatentHeat;

		Layer()
		{
		layerCode = -1;
		numLayerElements = -1;
		thickness = NAN;
		solidMatrixThermalConductivity = NAN;
		solidMatrixHeatCapacity = NAN;
		solidMatrixDensity = NAN;
		solidMatrixFraction = NAN;
		PCMLowerTransitionTemp = NAN;
		PCMUpperTransitionTemp = NAN;
		PCMSolidThermalConductivity = NAN;
		PCMLiquidThermalConductivity = NAN;
		PCMSolidHeatCapacity = NAN;
		PCMLiquidHeatCapacity = NAN;
		PCMDensity = NAN;
		PCMLatentHeat = NAN;
	}
};

class Weather
{
	public:
		int Year, Month, Day, Hour;
		double AirTemp;
		double WindSpeed;
		double PctSun;
		double DewPoint;
		double Latitude, Longitude;

	Weather()
	{
		Year = -1;
		Month = -1;
		Day = -1;
		Hour = -1;
		AirTemp = NAN;
		WindSpeed = NAN;
		PctSun = NAN;
		DewPoint = NAN;
		Latitude = NAN;
		Longitude = NAN;
	}
};

/*Function prototypes*/
void readInputFile(int &numStepsPerHour, Surface &surface, vector<Layer> &layers, string inputFile);
void readWeatherData(vector<Weather> &weatherData, string inputFile);