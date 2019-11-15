#include <iostream>
#include "Planet.h"

/*======================================================================================================================================================
	Planet::Planet(double p_periode, double p_weight, double r_weight, double velocity_x0, double velocity_y0, double position_x0, double position_y0)
	Fonction : Initialisation
======================================================================================================================================================*/
Planet::Planet(double p_periode, double p_weight, double r_weight, double velocity_x0, double velocity_y0, double position_x0, double position_y0)
{
	_p_periode = p_periode;
	_p_weight = p_weight;
	_r_weight = r_weight;
	_velocity_x0 = velocity_x0;
	_velocity_y0 = velocity_y0;
	_position_x0 = position_x0;
	_position_y0 = position_y0;

	_velocity_xt = _velocity_x0;
	_velocity_yt = _velocity_y0;
	_positionX = _position_x0;
	_positionY = _position_y0;
	_distance = 0;

}

void Planet::Print_planet(void)
{
	std::cout << "_p_periode = " << _p_periode << std::endl;
	std::cout << "_p_weight = " << _p_weight << std::endl;
	std::cout << "_r_weight = " << _r_weight << std::endl;
	std::cout << "_velocity_x0 = " << _velocity_x0 << std::endl;
	std::cout << "_velocity_y0 = " << _velocity_y0 << std::endl;
	std::cout << "_position_x0 = " << _position_x0 << std::endl;
	std::cout << "_position_y0 = " << _position_y0 << std::endl;
}

/*=====================================================================================================================================================
	void Planet::Update_position_Runge_Kutta(int cases)
	Fonction : Calcul les nouveaux coordonnées des planètes. Les détails pour obtenir les formes ci-desssous sont dans le fichier Readme
=====================================================================================================================================================*/
void Planet::Update_position_Euler(int cases)
{
	/*************************************************************************************************************/
	/*Calcul de la position de la planète bassée sur l'algorithme d'Euler pour le calcul différentiel voir notes*/
	/*************************************************************************************************************/

	switch (cases) {
	case 1: 
		_velocity_xt = _velocity_xt - h * (_r_weight * GRAVI * _positionX) / pow(_distance, 3);//renvoie la vitesse Vx
		_velocity_yt = _velocity_yt - h * (_r_weight * GRAVI * _positionY) / pow(_distance, 3);//renvoie la vitesse Vy
		std::cout << "_velocity_xt : " << _velocity_xt << "\t" << "_velocity_yt : " << _velocity_yt << std::endl;
		break;
	case 2: 
		_positionX = (_positionX + h * _velocity_xt); //renvoie la position X
		_positionY = (_positionY + h * _velocity_yt); //renvoie la position Y

		//std::cout << "_positionX : " << _positionX / 3865751 << "\t" << "_positionY : " << _positionY / 3899491 << std::endl;
		break;
	}
}

/*=====================================================================================================================================================
	void Planet::Update_position_Runge_Kutta(int cases)
	Fonction : Calcul les nouveaux coordonnées des planètes. Les détails pour obtenir les formes ci-desssous sont dans le fichier Readme
=====================================================================================================================================================*/
void Planet::Update_position_Runge_Kutta(int cases)
{
	/********************************************************************************************************************/
	/*Calcul de la position de la planète bassée sur l'algorithme de Runge Kutta pour le calcul différentiel voir notes*/
	/********************************************************************************************************************/
	switch (cases) {
	case 1:
		//std::cout << "velocity_x : " << _velocity_xt <<"	velocity_y : "<< _velocity_yt<<"	velocity_x0 : "<<_velocity_x0 << "	velocity_y0 : " << _velocity_y0 << std::endl;
		_velocity_xt = _velocity_xt * (1 - (h * h * _r_weight * GRAVI / (2 * pow(_distance, 3)))) - h * _r_weight * GRAVI * _positionX / pow(_distance, 3);//renvoie la vitesse Vx
		_velocity_yt = _velocity_yt * (1 - (h * h * _r_weight * GRAVI / (2 * pow(_distance, 3)))) - h * _r_weight * GRAVI * _positionY / pow(_distance, 3);//renvoie la vitesse Vy			
		break;
	case 2:
		//std::cout << "position_x : " << _positionX << "	position_y : " << _positionY << "	position_x0 : " << _position_x0 << "	position_y0 : " << _position_y0 << std::endl;
		_positionX = _positionX * (1 - (h * h * _r_weight * GRAVI / (2 * pow(_distance, 3)))) + h * _velocity_xt;//renvoie la position X
		_positionY = _positionY * (1 - (h * h * _r_weight * GRAVI / (2 * pow(_distance, 3)))) + h * _velocity_yt;//renvoie la position Y
		break;
	}
}

/*=====================================================================================================================================================
	void Planet::distance(void)
	Fonction : Calcul la distance entre le soleil et la planète
=====================================================================================================================================================*/
void Planet::distance(void)
{
	_distance = sqrt(pow(_positionX, 2) + pow(_positionY, 2));// Distance between Sun and Planet
}
