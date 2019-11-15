#ifndef _Planet_H_
#define _Planet_H_
#include<math.h>

//d�finition constantes (masse plan�te, distance soleil-plan�te, perdiode plan�te...)
#define MASSEsoleil 2E30
#define MASSEmercure 0.33018E24
#define MASSEvenus 4.8685E24
#define MASSEterre 5.9736E24
#define MASSEmars 0.64185E24
#define MASSEjupiter 1898.6E24
#define MASSEsaturne 568.46E24
#define MASSEuranus 86.831E24
#define MASSEneptune 102.43E24
#define MASSElune 5.9736E22

#define DISTANCEsoleilmercure 58E9
#define DISTANCEsoleilvenus 108E9
#define DISTANCEsoleilterre 150E9
#define DISTANCEsoleilmars 228E9
#define DISTANCEsoleiljupiter 778E9
#define DISTANCEsoleilsaturne 1425E9
#define DISTANCEsoleiluranus 2880E9
#define DISTANCEsoleilneptune 4500E9
#define DISTANCEterrelune 384E6

#define PERIODEmercure 7603200.
#define PERIODEvenus 19440000.
#define PERIODEterre 31558464.
#define PERIODEmars 59356800.
#define PERIODEjupiter 375535440.
#define PERIODEsaturne 930949200.
#define PERIODEuranus 2650838400.
#define PERIODEneptune 5207004000.
#define PERIODElune 2358720. 

#define pi std::acos(-1.0)

#define h 205.  //	Le pas pour chaque calcule des �quations diff�rentielles
#define GRAVI 6.6742E-11

/*=========================================================================================================================
	class Planet
	Fonction : 
		++ Sauvegarde les constantes de chaque plan�te 
		++ Calcule les nouveaux coordonn�es de chaque plan�te avec les m�thodes num�riques d'Euler ou de Runge Kutta 
==========================================================================================================================*/

class Planet {
public:
	Planet(double p_periode, double p_weight, double r_weight, double velocity_x0, double velocity_y0, double position_x0, double position_y0);

	void Print_planet(void);

	//\\//\\Calul des nouveaux coordonn�es (positions en x, y et vitesse en x, y) de chaque plan�te en utilisant la m�thode de Runge Kutta \\//\\//
	void Update_position_Euler(int cases);

	//\\//\\Calul des nouveaux coordonn�es (positions en x, y et vitesse en x, y) de chaque plan�te en utilisant la m�thode de Runge Kutta \\//\\//
	void Update_position_Runge_Kutta(int cases);

	//\\//\\Calcule la distance entre la nouvelle position de la plan�te et le soleil  \\//\\//
	void distance(void);

	//\\//\\Retourne la position en x de la plan�te \\//\\//
	double get_position_x() {
		return _positionX;
	}

	//\\//\\Retourne la position en y de la plan�te \\//\\//
	double get_position_y() {
		return _positionY;
	}

private:
	//constantes des plan�tes lors de l'initialisation
	double _p_periode;
	double _p_weight;
	double _r_weight;
	double _velocity_x0;
	double _velocity_y0;
	double _position_x0;
	double _position_y0;

	//Variables pour le calcul des nouvelles coordonn�es de chaque plan�te (position en x, y et vitesse en x, y)
	double _velocity_xt;
	double _velocity_yt;
	double _positionX;
	double _positionY;
	double _distance;
};

#endif