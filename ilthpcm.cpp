#include "ilthpcm.h"

int main(int argc,char* argv[])
{
	/*Declarations*/
	int nt, noOfElements;
	Surface surface;
	vector<Layer> layers;
	vector<Weather> weatherData;
	vector<float> x,dx,alpha,T,Tnew;
	vector<float> a,b,c,d;
	float solarrad, qirr, qconv, qrad;
	float dt, xi = -0.1; //dt in seconds, xi in C/m
	ofstream fout;

	//Variables for MEPDG format solution
	ofstream fMEPDG;
	vector<float> xPCC, TPCC;
	int timestamp;

	/*Read data*/
	//Read input file
	readInputFile(nt, surface, layers, argv[1]);
	//Read weather data
	readWeatherData(weatherData, argv[2]);

	//Define mesh (x and dx or each element, also diffusivity of each element)
	noOfElements = 0;
	for(int i=0;i<layers.size();i++){
		noOfElements += layers[i].numLayerElements;
	}
	
	defineMesh(x, dx, alpha, layers, noOfElements);

	a.assign(noOfElements,0.0);
	b.assign(noOfElements,0.0);
	c.assign(noOfElements,0.0);
	d.assign(noOfElements,0.0);

	dt = 3600.0/(float)nt;

	//Define stiffness matrix
	stiffnessmat(a, b, c, x, dx, alpha, dt, noOfElements);

	//Create output file
	fout.open(argv[3],ios::trunc);
	fMEPDG.open("ThermalPCC_ILTH.dat",ios::trunc);
	fMEPDG << fixed << setprecision(1) << showpoint;
	
	//Write headers in output file
	fout << "Year" << "," << "Month" << "," << "Day" << "," << "Hour" << ",";
	for(int j=0;j<noOfElements;j++){
		fout << x[j]*1000.0 << ",";
	}
	fout << endl;

	//Initialize temperature field
	T.assign(noOfElements,weatherData[0].AirTemp);
	Tnew.assign(noOfElements,0.0);

	//Begin loop for each weather case 
	for(int i=0;i<weatherData.size();i++){
		cout << "Analyzing hour " << i << " of " << weatherData.size() << endl;
		//Calculate solar energy ( assumed constant over the hour)
		solarrad = solar(weatherData[i]);

		//Iterate nt times to cover the hour
		for(int t=0;t<nt;t++){
			//Calculate surface energy balance
			qirr = longwave(weatherData[i], T[0], surface.isothermalEmissivity);
			qconv = convection(weatherData[i], T[0]);
			qrad = (solarrad*(1.0-surface.isothermalAlbedo) + qirr + qconv)/(layers[0].solidMatrixDensity*layers[0].solidMatrixHeatCapacity);

			//Define RHS [d]
			rhsvector(d, T, x, dx, alpha, dt, qrad, xi, noOfElements);

			//Solve the system of equations and store result in Tnew
			solve(Tnew, a, b, c, d, noOfElements);
			
			//Swap solution for new iteration
			cout << a[5] << "\t" << b[5] << endl;
			T = Tnew;
		}

		//Print current case to file
		fout << weatherData[i].Year << "," << weatherData[i].Month << "," << weatherData[i].Day << "," << weatherData[i].Hour << ","; 
		for(int j=0;j<noOfElements;j++){
			fout << T[j] << ",";
		}
		fout << endl;
		
		//Write to MEPDG format (top stabilizedLayer only)
		xPCC = vector<float>(x.begin(),x.begin()+layers[0].numLayerElements);
		TPCC = vector<float>(T.begin(),T.begin()+layers[0].numLayerElements);
		timestamp = weatherData[i].Year*1E6 + weatherData[i].Month*1E4 + weatherData[i].Day*1E2 + weatherData[i].Hour;
		
		WriteMEPDG(xPCC,TPCC,layers[0].numLayerElements,layers[0].thickness,11,2,fMEPDG,timestamp);
	}

	fout.close();
	fMEPDG.close();

	return 0;
}