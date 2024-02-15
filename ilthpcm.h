#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <limits>
#include <math.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

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
void defineMesh(vector<float> &x, vector<float> &dx, vector<float> &alpha, vector<Layer> layerVector, int noOfElements);
float solar(Weather weatherObject);
float longwave(Weather weatherObject, float Ts, float emissivity);
float convection(Weather weatherObject, float Ts);
void solve(vector<float>& Tnew, vector<float> a, vector<float> b, vector<float> c, vector<float> d, int noOfElements);
void stiffnessmat(vector<float>& a, vector<float>& b, vector<float>& c, vector<float> x, vector<float> dx, vector<float> alpha, float dt, int noOfElements);
void rhsvector(vector<float>& d, vector<float> T, vector<float> x, vector<float> dx, vector<float> alpha, float dt, float qrad, float xi, int noOfElements);
void WriteMEPDG(vector<float> x, vector<float> T, int N, float thickness, int Ni, int n, ofstream& fMEPDG, int timestamp);
void QuadraticFit(vector<float> x, vector<float> T, int N, vector<float> xi, vector<float>& Ti, int Ni, int n);
vector<float> pwl_value_1d ( int nd, vector<float> xd, vector<float> yd, int ni, vector<float> xi );