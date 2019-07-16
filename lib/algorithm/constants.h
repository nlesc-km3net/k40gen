#include <math.h>

static const double PI                        = 3.1415927;            //!< pi
static const double EULER                     = 0.577215664901533;    //!< Euler number
static const double C                         = 0.299792458;          //!< Speed of light in vacuum [m/ns]
static const double C_INVERSE                 = 1.0/C;                //!< Inverse speed of light in vacuum [ns/m]
static const double AVOGADRO                  = 6.0221415e23;         //!< Avogadro's number [gr^-1]
static const double H                         = 4.13566733e-15;       //!< Planck constant [eV s]
static const double HBAR                      = H/(2*PI);             //!< Planck constant [eV s]
static const double HBARC                     = HBAR*C*1.0e9;         //!< Planck constant [eV m]
static const double ALPHA_ELECTRO_MAGNETIC    = 1.0/137.036;          //!< Electro-Magnetic coupling constant
static const double THETA_MCS                 = 13.6e-3;              //!< Multiple Coulomb scattering constant [GeV]

/**
* Fixed environment values.
*/
static const double DENSITY_SEA_WATER         = 1.038;                //!< Density  of sea water [g/cm^3]
static const double DENSITY_ROCK              = 2.65;                 //!< Density  of rock      [g/cm^3]
static const double SALINITY_SEA_WATER        = 0.035;                //!< Salinity of sea water
static const double INDEX_OF_REFRACTION_WATER = 1.3800851282;         //!< average index of refraction of water
static const double X0_WATER_M                = 0.36;                 //!< Radiation length pure water [m]  

/**
* Derived quantities of optical medium.
*/
static const double TAN_THETA_C_WATER         = sqrt((INDEX_OF_REFRACTION_WATER - 1.0) * (INDEX_OF_REFRACTION_WATER + 1.0));
static const double COS_THETA_C_WATER         = 1.0 / INDEX_OF_REFRACTION_WATER;
static const double SIN_THETA_C_WATER         = TAN_THETA_C_WATER * COS_THETA_C_WATER;
static const double KAPPA_WATER               = 0.96;

/**
* Particle masses.
* Note that the neutrino masses are set to zero.
*/
static const double MASS_PHOTON               = 0.0;                 //!< photon     mass [GeV]

static const double MASS_ELECTRON_NEUTRINO    = 0.0;                 //!< electron neutrino mass [GeV]
static const double MASS_MUON_NEUTRINO        = 0.0;                 //!< muon     neutrino mass [GeV]
static const double MASS_TAU_NEUTRINO         = 0.0;                 //!< tau      neutrino mass [GeV]
  
static const double MASS_ELECTRON             = 0.510998946e-3;      //!< electron   mass [GeV]
static const double MASS_MUON                 = 0.1056583745;        //!< muon       mass [GeV]
static const double MASS_TAU                  = 1.77682;             //!< tau        mass [GeV]

static const double MASS_NEUTRAL_PION         = 0.1349766;           //!< pi_0       mass [GeV]
static const double MASS_CHARGED_PION         = 0.13957018;          //!< pi^+/-     mass [GeV]
static const double MASS_NEUTRAL_KAON         = 0.497614;            //!< K_0        mass [GeV]
static const double MASS_CHARGED_KAON         = 0.493677;            //!< K^+/-      mass [GeV]

static const double MASS_PROTON               = 0.9382720813;        //!< proton     mass [GeV]
static const double MASS_NEUTRON              = 0.9395654133;        //!< neutron    mass [GeV]
static const double MASS_LAMBDA               = 1.115683;            //!< Lambda     mass [GeV]
static const double MASS_NEUTRAL_SIGMA        = 1.192642;            //!< Sigma_0    mass [GeV]
static const double MASS_CHARGED_SIGMA        = 1.18937;             //!< Sigma^+/-  mass [GeV]
static const double MASS_NEUTRAL_XI           = 1.31486;             //!< Xi_0       mass [GeV]
static const double MASS_CHARGED_XI           = 1.32171;             //!< Xi^+/-     mass [GeV]
static const double MASS_CHARGED_OMEGA        = 1.67245;             //!< Omega^+/-  mass [GeV]

/**
 * Computing quantities.
*/
static const long long int KILOBYTE   = 1024;                        //! Number of bytes in a kilobyte
static const long long int MEGABYTE   = KILOBYTE*KILOBYTE;           //! Number of bytes in a megabyte
static const long long int GIGABYTE   = KILOBYTE*KILOBYTE*KILOBYTE;  //! Number of bytes in a gigabyte

  
/*
   Get speed of light.
*/
inline const double getSpeedOfLight()
{
    return C;
}


/*
   Get inverse speed of light[ns/m]
*/
inline const double getInverseSpeedOfLight()
{ 
    return C_INVERSE; 
}


/*
 Get average index of refraction of water
*/
inline double getIndexOfRefraction() 
{
    return INDEX_OF_REFRACTION_WATER;
}


/* 
    Get average index of refraction of water.
*/
inline double getIndexOfRefractionPhase() 
{
    return 1.35;
}


/*
    Get average tangent of Cherenkov angle of water.
    tan(theta_C)
*/
inline double getTanThetaC() 
{
    return TAN_THETA_C_WATER;
}


/*
 Get average cosine of Cherenkov angle of water.
 cos(theta_C)
*/
inline double getCosThetaC() 
{
    return COS_THETA_C_WATER;
}


/* 
    Get average sine of Cherenkov angle of water.
    sin(theta_C)
*/
inline double getSinThetaC() 
{
    return SIN_THETA_C_WATER;
}


/*
 Get average kappa of Cherenkov light in water.
*/
inline double getKappaC() 
{
    return KAPPA_WATER;
}


