#include <cmath>
#include <stdio.h>

#include "pico/stdlib.h"

#include "humidity.h"

static const double a1 = -7.85951783;
static const double a2 = 1.84408259;
static const double a3 = -11.7866497;
static const double a4 = 22.6807411;
static const double a5 = -15.9618719;
static const double a6 = 1.80122502;

static const double Pc = 22064000;
static const double Tc = 647.096;

static const double Rw = 461.5;

double Ps (double T) {
	double tau = 1-T/Tc;
	double pol = Tc/T*(a1*tau + a2*pow(tau, 1.5) + a3*pow(tau, 3) + a4*pow(tau, 3.5) + a5*pow(tau, 4) + a6*pow(tau, 7));
	return Pc*exp(pol);
}

// kg/m^3
double AH (double t, double RH) {
	double T = t + 273.15;
	return (RH*Ps(T))/(Rw*T*100);
}

uint16_t AH_for_sgp30 (double t, double RH) {
	double ah = AH(t, RH)*1000;
	return (uint16_t) round(ah*256);
}
