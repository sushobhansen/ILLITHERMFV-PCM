/*This file contains functions that writes the temperature data in MEPDG format
MEPDG format: Regardless of the actual problem solved, the temperature profile of the PCC layer
(assumed to be the top layer, always) is interpolated onto 11 evenly-spaced points, the first
point being at the surface and the last point at the interface with the next layer. Furthermore,
the temperature is evaluated in F from C. The timestamp and spacing are generated as per the MEPDG
format for the ThermPCC.dat file.*/

#include "ilthpcm.h"

void WriteMEPDG(vector<double> x, vector<double> T, int N, double thickness, int Ni, int n, ofstream& fMEPDG, int timestamp){
	
	/*This function calls QuadraticFit (see notes for that) and then writes the interpolated temperatures in F*/
	vector<double> xi(Ni,0.0), Ti(Ni,0.0);
	double dx;
	
	dx = thickness/(Ni-1);
	for(int i=0;i<Ni;i++){
		xi[i] = i*dx;
	}
	
	//Option 1 - Piecewise linear interpolation
	Ti = pwl_value_1d ( N, x, T, Ni, xi );
	//Option 2 - Use a quadratic fit
	//QuadraticFit(x,T,N,xi,Ti,Ni,n);
	
	//Convert to F and write
	fMEPDG << setw(11) << left << timestamp;
	for(int i=0;i<Ni;i++){
		fMEPDG << setw(6) << left << 1.8*Ti[i]+32;
	}
	fMEPDG << endl;
}

void QuadraticFit(vector<double> x, vector<double> T, int N, vector<double> xi, vector<double>& Ti, int Ni, int n){
	/*This function takes in the solution T at x points (both of size N). A quadratic polynomial (of order n) is fit to this.
	Ni interpolation points xi are specified. The polynomial is evaluated at those points and the 
	solutions written to Ti (of size Ni as well).
	Algorithm from: https://www.bragitoff.com/2015/09/c-program-for-polynomial-fit-least-squares/
	Also see: https://neutrium.net/mathematics/least-squares-fitting-of-a-polynomial/*/
		
	if(N<n){
		cout << "Error! Order of the polynomial cannot be greater than the number of points!\n";
		exit(0);
	}
	
	/*X stores sum(xi^[...]) for building the augmented matrix B. Y is the RHS.
	a is the vector of coefficients to solve. temp and t are intermediate variables.*/
	double X[2*n+1], B[n+1][n+2], Y[n+1], a[n+1] = {0.0};
	double temp, t;
	int i,j,k;
	
	//Define X
	for(i=0;i<2*n+1;i++){
		X[i] = 0.0;
		for(j=0;j<N;j++){
			X[i] += pow(x[j],i);
		}
	}
	
	//Define Y
	for(i=0;i<n+1;i++){
		Y[i] = 0.0;
		for(j=0;j<N;j++){
			Y[i] += pow(x[j],i)*T[j];
		}
	}
	
	//Define B 
	for (i=0;i<=n;i++){
        for (j=0;j<=n;j++){
            B[i][j]=X[i+j];
		}
	}
	
	//Load Y into last column of B for Gauss elimination
	for(i=0;i<n+1;i++){
		B[i][n+1] = Y[i];
	}
	
	//Gauss elimination
	n = n+1; //There are n+1 eqs to solve
	//Pivoting
	for (i=0;i<n;i++) {                    
        for (k=i+1;k<n;k++) {
            if (B[i][i]<B[k][i]) {
                for (j=0;j<=n;j++)
                {
                    temp=B[i][j];
                    B[i][j]=B[k][j];
                    B[k][j]=temp;
                }
			}
		}
	}
				
    //Elimination 
    for (i=0;i<n-1;i++)  {          
        for (k=i+1;k<n;k++)
            {
                t=B[k][i]/B[i][i];
                for (j=0;j<=n;j++)
                    B[k][j]=B[k][j]-t*B[i][j];    
            }
	}
	
	//Back-substitution
    for (i=n-1;i>=0;i--)                
    {                       
        a[i]=B[i][n];                
        for (j=0;j<n;j++)
            if (j!=i)            
                a[i]=a[i]-B[i][j]*a[j];
        a[i]=a[i]/B[i][i];            
    }
	
	//Evaluate Ti
	for(i=0;i<Ni;i++){
		Ti[i] = 0.0;
		if(xi[i]<x[0]){ //Extrapolation - constant
			Ti[i] = T[0]; 
		}
		else if(xi[i]>x[N-1]){ //Extrapolation - constant
			Ti[i] = T[N-1];
		}
		else{ //Interpolation
			for(j=0;j<n;j++){
				Ti[i] += a[j]*pow(xi[i],j);
			}
		}
	}
}

vector<double> pwl_value_1d ( int nd, vector<double> xd, vector<double> yd, int ni, vector<double> xi ){
	/*From: https://people.sc.fsu.edu/~jburkardt/cpp_src/pwl_interp_1d/pwl_interp_1d.cpp
	Evaluates the piecewise linear interpolated values
	xd[nd] is the vector of locations where data yd[nd] is known*
	xi[ni] is the vector of interpolation positions
	Returns a vector of interpolates values*/
		
	int i;
	int k;
	double t;
	vector<double> yi(ni,0.0);

	for ( i = 0; i < ni; i++ )
	{
		yi[i] = 0.0;
	}

	  if ( nd == 1 )
	  {
		for ( i = 0; i < ni; i++ )
		{
		  yi[i] = yd[0];
		}
		return yi;
	  }

	  for ( i = 0; i < ni; i++ )
	  {
		if ( xi[i] <= xd[0] ) //Extrapolation - linear
		{
		  t = ( xi[i] - xd[0] ) / ( xd[1] - xd[0] );
		  yi[i] = ( 1.0 - t ) * yd[0] + t * yd[1];
		}
		else if ( xd[nd-1] <= xi[i] ) //Extrapolation - linear
		{
		  t = ( xi[i] - xd[nd-2] ) / ( xd[nd-1] - xd[nd-2] );
		  yi[i] = ( 1.0 - t ) * yd[nd-2] + t * yd[nd-1];
		}
		else
		{
		  for ( k = 1; k < nd; k++ )
		  {
			if ( xd[k-1] <= xi[i] && xi[i] <= xd[k] )
			{
			  t = ( xi[i] - xd[k-1] ) / ( xd[k] - xd[k-1] );
			  yi[i] = ( 1.0 - t ) * yd[k-1] + t * yd[k];
			  break;
			}
		  }
		}
	  }
	  return yi;
}