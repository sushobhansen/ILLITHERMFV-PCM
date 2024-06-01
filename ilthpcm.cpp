#include "ilthpcm.h"

int main(int argc,char* argv[])
{
	/*Declarations*/
	int nt, noOfElements, maxinneriters;
	Surface surface;
	vector<Layer> layers;
	vector<Weather> weatherData;
	vector<double> x,dx,alpha,T,Tnew;
	vector<double> a,b,c,d;
	double solarrad, qirr, qconv, qrad, albedo;
	double dt, xi = -0.1, TOL; //dt in seconds, xi in C/m
	ofstream fout;
	double fl0; //liquid fraction initial update
	double dfl; //liquid fraction derivative
	vector<double> fl, deltaH; //liquid fraction vector
	double Sc,Sp, underrelax_factor; // Source term in RHS and Sorce Term in LHS
	double k_eff; //Composite thermal conductivity
	double c_eff; //Composite heat capacity
	double rho_eff; //Composite density

	//Variables for MEPDG format solution
	ofstream fMEPDG;
	vector<double> xPCC, TPCC;
	int timestamp;

	/*Read data*/
	//Read input file
	readInputFile(nt, underrelax_factor, maxinneriters, TOL, surface, layers, argv[1]);
	//Read weather data
	readWeatherData(weatherData, argv[2]);

	//Define mesh (x and dx or each element, also diffusivity of each element)
	noOfElements = 0;
	for(int i=0;i<layers.size();i++){
		noOfElements += layers[i].numLayerElements;
	}
	
	defineMesh(x, dx, layers, noOfElements);

	a.assign(noOfElements,0.0);
	b.assign(noOfElements,0.0);
	c.assign(noOfElements,0.0);
	d.assign(noOfElements,0.0);
	alpha.assign(noOfElements,0.0);
	deltaH.assign(noOfElements,0.0);
	

	dt = 3600.0/(double)nt;
	//Define stiffness matrix
	//Initialize temperature field
	T.assign(noOfElements,weatherData[0].AirTemp);
	Tnew.assign(noOfElements,0.0);
	update_liquid_fraction(layers, T, underrelax_factor, 0);
	update_thermal_properties(layers);
	assign_layer_to_element(alpha, deltaH, layers, noOfElements);
	
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

	//Begin loop for each weather case 
	for(int i=0;i<weatherData.size();i++){
		cout << "Analyzing hour " << i+1 << " of " << weatherData.size() << endl;
		//Calculate solar energy ( assumed constant over the hour)
		solarrad = solar(weatherData[i]);

		//Iterate nt times to cover the hour
		for(int t=0;t<nt;t++){
			//Inner iterations for updating liquid fraction
			for (int inner = 0; inner < maxinneriters; inner++)
			{
				if(surface.surfaceCode == 0)
				{
					albedo = surface.isothermalAlbedo;
				}
				else
				{
					albedo = thermochromic_albedo(surface, T[0]);
				}

				qirr = longwave(weatherData[i], T[0], surface.isothermalEmissivity);
				qconv = convection(weatherData[i], T[0]);
				qrad = (solarrad*(1.0-albedo) + qirr + qconv)/(layers[0].solidMatrixDensity*layers[0].solidMatrixHeatCapacity);
				stiffnessmat(a, b, c, x, dx, alpha, dt, noOfElements);
				//Define RHS [d]
				rhsvector(d, T, x, dx, alpha, dt, qrad, xi, noOfElements);
				//Solve the system of equations and store result in Tnew
				solve(Tnew, a, b, c, d, noOfElements);
				if(calculate_max_diff(T, Tnew) < TOL)
				{
					T = Tnew;
					//Now update liquid fraction iteratively
					update_liquid_fraction(layers, T, underrelax_factor, 1);
					update_thermal_properties(layers);
					assign_layer_to_element(alpha, deltaH, layers, noOfElements);
					cout << "Converged in " << inner+1 << " iterations.\n";
					break;
				}
				T = Tnew;
				//Now update liquid fraction iteratively
				update_liquid_fraction(layers, T, underrelax_factor, 1);
				update_thermal_properties(layers);
				assign_layer_to_element(alpha, deltaH, layers, noOfElements);
			}
		}

		//Print current case to file
		fout << weatherData[i].Year << "," << weatherData[i].Month << "," << weatherData[i].Day << "," << weatherData[i].Hour << ","; 
		for(int j=0;j<noOfElements;j++){
			fout << T[j] << ",";
		}
		fout << endl;
		
		for(int ilayer = 0; ilayer < layers.size(); ilayer++)
		{
			cout << layers[ilayer].layerCode << endl;
			if(layers[ilayer].layerCode == 1)
			{
				for(int ielem = 0; ielem < layers[i].numLayerElements; ielem++)
				{
					cout << layers[ilayer].fl[ielem] << " ";
				}
				cout << endl;
			}
		}

		//Write to MEPDG format (top stabilizedLayer only)
		xPCC = vector<double>(x.begin(),x.begin()+layers[0].numLayerElements);
		TPCC = vector<double>(T.begin(),T.begin()+layers[0].numLayerElements);
		timestamp = weatherData[i].Year*1E6 + weatherData[i].Month*1E4 + weatherData[i].Day*1E2 + weatherData[i].Hour;
		
		WriteMEPDG(xPCC,TPCC,layers[0].numLayerElements,layers[0].thickness,11,2,fMEPDG,timestamp);
	}

	fout.close();
	fMEPDG.close();

	return 0;
}