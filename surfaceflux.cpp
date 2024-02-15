#include "ilthpcm.h"

float solar(Weather weatherObject){
	
	int N1, N2, N3, N;
	float declination, rHour, A, B, cosh0, h0, Z, h, cosZ, tsunrise, tsunset, time, correction;
	const float d2r = M_PI/180.0; //Conversion factor, deg to rad 
	const float S0 = 1367.0; //Solar constant, 1367 W/m2
	
	/***** Calculate time of sunrise and sunset *****/
	/*See https://en.wikipedia.org/wiki/Solar_irradiance*/
	/*ASSUMPTIONS - Solar noon is at 12.0, Earth's orbit is circular*/
	
	//Day of the year
	N1 = floor(275*weatherObject.Month/9);
	N2 = floor((weatherObject.Month+9)/12);
	N3 = (1+floor((weatherObject.Year - 4*floor(weatherObject.Year/4) + 2)/3));
	N = N1 - (N2*N3) + weatherObject.Day - 30;
	
	//Calculate declination angle for N
	declination = 23.45*d2r*sin(2*M_PI*(284+N)/365);
	rHour = 15.0*d2r; //One hour (1/15 of a degree) in radians, this is a conversion factor from hours to radians
	
	//Calculate A and B coefficients
	A = sin(declination);
	B = cos(declination);
	
	//Calculate h0, the hour angle at sunrise or sunset, when the zenith angle Z is zero
	//Note that cosZ = sin(lat)*A + cos(lat)*B*cos(h), from http://www.atmos.albany.edu/facstaff/brose/classes/ATM623_Spring2015/Notes/Lectures/Lecture11%20--%20Insolation.html
	cosh0 = -(sin(weatherObject.Latitude*d2r)*A)/(cos(weatherObject.Latitude*d2r)*B); //cosine of the hour angle at sunrise of sunset, with zenith at 90 degrees
	h0 = acos(fabs(cosh0)); //hour angle at sunrise and sunset
	
	if(cosh0<0){
		//h0 can only be between 0 and 180 deg, so rescale for it
		h0 = 180.0*d2r-h0;
	}
	
	//Calculate tsunrise and tsunset
	time = h0/rHour; //Z radians to hours
	tsunrise = 12.0 - time;
	tsunset = 12.0 + time;
	//Verified that this works - SS 5.31.18
	
	/***** Calculate solar radiation *****/
	/*Q=S0*cos(Z)*correction*/
	//Correction factor for radiation reaching the ground is based on Paltineanu et al but with instantaneous values
	h = (12.0-weatherObject.Hour)*rHour;
	cosZ = sin(weatherObject.Latitude*d2r)*A + cos(weatherObject.Latitude*d2r)*B*cos(h);
	correction = 0.237 + 0.511*weatherObject.PctSun/100.0;
	
	if(weatherObject.Hour < tsunrise || weatherObject.Hour > tsunset){
		return 0.0;
	}
	else{
		return S0*cosZ*correction;
	}
	//Verified that this works - SS 5.31.18
}

float longwave(Weather weatherObject, float Ts, float emissivity){
	float const sigma = 5.67*powf(10,-8); //Stefan-Boltzmann constant of 5.67e-8 Wm/K^4 
	float esky, Tsky, irraditation;
	
	/*Sky emissivity from Berdahl and Martin (1982)*/
	/*Does not have to be extremely accurate, therefore time and pressure corrections are ignored*/
	esky = 0.711 + 0.56*(weatherObject.DewPoint/100) + 0.73*powf(weatherObject.DewPoint/100,2.0);
	Tsky = (weatherObject.AirTemp+273.15) * powf(esky,0.25);
	irraditation = sigma*emissivity*(powf(Tsky,4.0)-powf(Ts+273.15,4.0));
	//Verified - SS 5.31.2018
	
	return irraditation;
}

float convection(Weather weatherObject, float Ts){
	float h, v;
	
	v = weatherObject.WindSpeed;
	
	//Convection coefficient from Bentz (2000), simpler than Qin & Hiller (2011)
	if(v<=5){
		h = 5.6 + 4.0*v;
	}
	else{
		h = 7.2*powf(v,0.78);
	}
	
	//Verified - SS 6.4.18
	return h*(weatherObject.AirTemp - Ts);
}