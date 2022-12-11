#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
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
		
	Surface(){}
};

/*Function prototypes*/
void readInputFile();