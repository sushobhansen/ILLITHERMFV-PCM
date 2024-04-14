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
void readInputFile(int &numStepsPerHour, double &underrelax_factor, Surface &surface, vector<Layer> &layers, string inputFile);
void readWeatherData(vector<Weather> &weatherData, string inputFile);
void defineMesh(vector<double> &x, vector<double> &dx, vector<double> &alpha, vector<Layer> layerVector, int noOfElements);
double solar(Weather weatherObject);
double longwave(Weather weatherObject, double Ts, double emissivity);
double convection(Weather weatherObject, double Ts);
void solve(vector<double>& Tnew, vector<double> a, vector<double> b, vector<double> c, vector<double> d, int noOfElements);
void stiffnessmat(vector<double>& a, vector<double>& b, vector<double>& c, vector<double> x, vector<double> dx, vector<double> alpha, double dt, int noOfElements);
void rhsvector(vector<double>& d, vector<double> T, vector<double> x, vector<double> dx, vector<double> alpha, double dt, double qrad, double xi, int noOfElements);
void WriteMEPDG(vector<double> x, vector<double> T, int N, double thickness, int Ni, int n, ofstream& fMEPDG, int timestamp);
void QuadraticFit(vector<double> x, vector<double> T, int N, vector<double> xi, vector<double>& Ti, int Ni, int n);
vector<double> pwl_value_1d ( int nd, vector<double> xd, vector<double> yd, int ni, vector<double> xi );
double thermochromic_albedo(Surface surface, double Temperature);