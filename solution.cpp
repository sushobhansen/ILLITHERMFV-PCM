#include "ilthpcm.h"

void solve(vector<double>& Tnew, vector<double> a, vector<double> b, vector<double> c, vector<double> d, int noOfElements){
	
	/*Solution based on the Tridiagonal Matrix Algorithm (TDMA or Thomas algorithm), which is O(n)
	See https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm */
	
	vector<double> cstar, dstar;
	double m;
	
	cstar.assign(noOfElements,0.0);
	dstar.assign(noOfElements,0.0);
	
	cstar[0] = c[0]/b[0];
	dstar[0] = d[0]/b[0];
	
	//Forward sweep
	
	for(int i = 1; i<noOfElements; i++){
		m = 1.0/(b[i]-a[i]*cstar[i-1]);
		cstar[i] = c[i]*m;
		dstar[i] = (d[i]-a[i]*dstar[i-1])*m;
		//cout << cstar[i] << "\t" << dstar[i] << "\t" << m << endl;
	}
	

	
	//Reverse sweep
	Tnew[noOfElements-1] = dstar[noOfElements-1];
	for(int i = noOfElements-2; i>=0; i--){
		Tnew[i] = dstar[i] - cstar[i]*Tnew[i+1];
	}
	
}

void stiffnessmat(vector<double>& a, vector<double>& b, vector<double>& c, vector<double> x, vector<double> dx, vector<double> alpha, double dt, int noOfElements){
	
	double betap, betam, alphap, alpham;
	
	/*Piecewise linear finite volume formulation with Crank-Nicolson scheme*/
	
	//Define stiffness matrix [a b c]
	for(int j=0;j<noOfElements;j++)
	{
		if(j==0){
			betap = dx[j]*0.5/(x[j+1]-x[j]);
			alphap = alpha[j+1]*betap + alpha[j]*(1.0-betap);
			
			a[j] = 0.0;
			b[j] = 1.0 + (0.5*dt/dx[j])*alphap/(x[j+1]-x[j]);
			c[j] = -(0.5*dt/dx[j])*alphap/(x[j+1]-x[j]);
		}
		else if(j == noOfElements-1){
			betam = dx[j]*0.5/(x[j]-x[j-1]);
			alpham = alpha[j]*(1.0-betam) + alpha[j-1]*betam;
			
			a[j] = -(0.5*dt/dx[j])*alpham/(x[j]-x[j-1]);
			b[j] = 1.0 + (0.5*dt/dx[j])*alpham/(x[j]-x[j-1]);
			c[j] = 0.0;
		}
		else{
			betap = dx[j]*0.5/(x[j+1]-x[j]);
			alphap = alpha[j+1]*betap + alpha[j]*(1.0-betap);
			
			betam = dx[j]*0.5/(x[j]-x[j-1]);
			alpham = alpha[j]*(1.0-betam) + alpha[j-1]*betam;
			
			a[j] = -(0.5*dt/dx[j])*alpham/(x[j]-x[j-1]);
			b[j] = 1.0 + (0.5*dt/dx[j])*((alphap/(x[j+1]-x[j]))+(alpham/(x[j]-x[j-1])));
			c[j] = -(0.5*dt/dx[j])*alphap/(x[j+1]-x[j]);
		}
	}
	
}

void rhsvector(vector<double>& d, vector<double> T, vector<double> x, vector<double> dx, vector<double> alpha, double dt, double qrad, double xi, int noOfElements){
	
	double betap, betam, alphap, alpham;
	
	/*Piecewise linear finite volume formulation with Crank-Nicolson scheme*/
	
	//Define RHS vector [d]
	for(int j=0;j<noOfElements;j++){
		if(j==0){
			betap = dx[j]*0.5/(x[j+1]-x[j]);
			alphap = alpha[j+1]*betap + alpha[j]*(1.0-betap);
			
			d[j] = T[j]*(1.0-(0.5*dt/dx[j])*alphap/(x[j+1]-x[j])) + T[j+1]*(0.5*dt/dx[j])*alphap/(x[j+1]-x[j]) + qrad*(dt/dx[j]);
		}
		else if(j==noOfElements-1){
			betam = dx[j]*0.5/(x[j]-x[j-1]);
			alpham = alpha[j]*(1.0-betam) + alpha[j-1]*betam;
		
			d[j] = T[j-1]*(0.5*dt/dx[j])*alpham/(x[j]-x[j-1]) + T[j]*(1.0-(0.5*dt/dx[j])*alpham/(x[j]-x[j-1])) + alpha[j]*xi*(dt/dx[j]);
		}
		else{
			betap = dx[j]*0.5/(x[j+1]-x[j]);
			alphap = alpha[j+1]*betap + alpha[j]*(1.0-betap);
			
			betam = dx[j]*0.5/(x[j]-x[j-1]);
			alpham = alpha[j]*(1.0-betam) + alpha[j-1]*betam;
			
			d[j] = T[j-1]*(0.5*dt/dx[j])*alpham/(x[j]-x[j-1]) + T[j]*(1.0-(0.5*dt/dx[j])*((alphap/(x[j+1]-x[j]))+(alpham/(x[j]-x[j-1])))) + T[j+1]*(0.5*dt/dx[j])*alphap/(x[j+1]-x[j]);
		}
	}
	
}

void update_thermal_properties(vector<Layer> &layers)
{
	for(int i = 0; i < layers.size(); i++)
	{
		double kPCM, CPCM;
		if(layers[i].layerCode == 1) //if layer has PCMs
		{
			for(int j = 0; j < layers[i].numLayerElements; j++)
			{
				kPCM = (1.0 - layers[i].fl[j])*layers[i].PCMSolidThermalConductivity + layers[i].fl[j]*layers[i].PCMLiquidThermalConductivity;
				CPCM = (1.0 - layers[i].fl[j])*layers[i].PCMSolidHeatCapacity + layers[i].fl[j]*layers[i].PCMLiquidHeatCapacity;

				layers[i].effectiveThermalConductivity[j] = layers[i].solidMatrixFraction*layers[i].solidMatrixThermalConductivity + (1.0 - layers[i].solidMatrixFraction)*kPCM;
				layers[i].effectiveHeatCapacity[j] = layers[i].solidMatrixFraction*layers[i].solidMatrixHeatCapacity + (1.0 - layers[i].solidMatrixFraction)*CPCM;
			}
		}
	}
}

void update_liquid_fraction(vector<Layer> &layers, vector<double> T)
{
	int elementOffset = 0;
	double sigmaS, sigmaL, nodeTemperature;

	for (size_t i = 0; i < layers.size(); i++)
	{
		if(layers[i].layerCode == 1) //if layer has PCMs
		{
			sigmaS = (sqrt(2.0)/4.0)*(layers[i].PCMIntermediateTransitionTemp - layers[i].PCMLowerTransitionTemp);
			sigmaL = (sqrt(2.0)/4.0)*(layers[i].PCMUpperTransitionTemp - layers[i].PCMIntermediateTransitionTemp);

			for(int j = 0; j < layers[i].numLayerElements; j++)
			{
				nodeTemperature = T[elementOffset + j];
				if(nodeTemperature <= layers[i].PCMIntermediateTransitionTemp)
				{
					layers[i].fl[j] = erf((nodeTemperature - layers[i].PCMIntermediateTransitionTemp)/sigmaS) + 1.0;
					layers[i].fl[j] *= sigmaS/(sigmaS + sigmaL);
				}
				else
				{
					layers[i].fl[j] = sigmaL*erf((nodeTemperature - layers[i].PCMIntermediateTransitionTemp)/sigmaL) + sigmaS;
					layers[i].fl[j] *= 1.0/(sigmaS + sigmaL);
				}
			}
		}
		
		elementOffset += layers[i].numLayerElements;
	}
	
}