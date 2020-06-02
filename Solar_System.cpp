

/************************************************************************	
 * Projet : Simulation du système solaire en c++ avec vtk				*	
 * Auteur : Mouhamadou Ndoye											*	
 * UE : Développement librairie image									*	
 * Université : Clermont Auvergne										*
 * Année Universitaire : 2019 - 2020									*
 ************************************************************************/

/****************************************************************************************************************************************
 * Lire le fichier readme.docx avant de compiler et d'exécuter le programme
 * L'objectif de ce projet est de simuler le mouvement des planetes autour du soleil													*
 * Je me suis limité à 6 Planètes car plus la planète est loin du soleil plus son temps de simulation est long							*
 * Le principe reste le même si on souhaite rajouter les 2 autres planètes (Uranus et Neptune)											*	
 * Ce projet peut être divisé en deux parties distinct :																				*
    + Une partie C++ avec le calcul de la vitesse et de la position de chaque planète (planet.cpp)										*	
    + Une partie VTK pour l'affichage Graphique (Solar_System.cpp)																		*
 * Pour calculer les coordonnées de chaque planète j'utilise l'équation générale de la gravité (voir planet.cpp pour plus de détails)	*
 * Voir la section de chaque fonction pour avoir plus de détails sur celle-ci															*
 ****************************************************************************************************************************************/

#include <iostream>

#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkDiskSource.h>

#include <vtkCommand.h>
#include <vtkSmartPointer.h>

#include <vtkPolyDataMapper.h>

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkActor.h>
#include <vtkProperty.h>

#include <vtkTransformTextureCoords.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader.h>
#include <vtkTexturedSphereSource.h>

#include <vtkPointSource.h>
#include <vtkPolyData.h>

#include "Planet.h"

/*==========================================================================================
	vtkSmartPointer<vtkActor> DrawLine(double position_new[3], double position_last[3])
	Fonction : Trace l'orbite de chaque planète à partir de 2 points fournis 
	Paramètres : 
		position_new[3] : coordonnées x, y, z de la nouvelle position de la planète
		position_new[3] : coordonnées x, y, z de la l'ancienne position de la planète

	A partir de ces deux positions je trace une ligne qui reprèsente une parite de l'orbite
===========================================================================================*/

vtkSmartPointer<vtkActor> DrawLine(double position_new[3], double position_last[3]) {

	vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
	//static double position_last[3] = { DISTANCEterrelune / 3865751, 0, 0 };
	lineSource->SetPoint1(position_last);
	lineSource->SetPoint2(position_new);
	lineSource->Update();
	vtkSmartPointer<vtkPolyDataMapper> mapperLine = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapperLine->SetInputConnection(lineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actorLine = vtkSmartPointer<vtkActor>::New();
	actorLine->SetMapper(mapperLine);
	actorLine->GetProperty()->SetLineWidth(1);

	for (int i = 0; i < 3; i++) position_last[i] = position_new[i]; //update position	

	return actorLine;
}

/*=========================================================================================================================
	double rescale_coordinates(char planet, double value)
	Fonction : met à l'echelle les coordonnées de chaque planète pour l'affichage
	Paramètres :
		Planet : 0 ou 1 
			++ 0 : On rescale les coordonnées de la lune
			++ 1 : Je rescale les coorconnées des autres planètes 
		Value : Valeur à rescaler
	
	La référence est la position initiale de mercure par rapport à la terre qui vaudra 50 suivant x en échelle graphique
==========================================================================================================================*/

double rescale_coordinates(char planet, double value) {

	if (planet == 0) return value / 3865751;

	else return (value * 50) / DISTANCEsoleilmercure;

}

/*=========================================================================================================================
	class vtkTimerCallback : public vtkCommand
	Fonction : Programme d'interruption permettant :
		++ De calculer la nouvelle position de chaque planète à chaque interruption
		++ De tracer l'orbite de chaque planète en fonction des nouvelles positions
		++ De mettre à jour l'affichage graphique
==========================================================================================================================*/

class vtkTimerCallback : public vtkCommand
{
public:
	static vtkTimerCallback* New()
	{
		vtkTimerCallback* cb = new vtkTimerCallback;
		cb->TimerCount = 0;
		return cb;
	}

	void Execute(vtkObject* vtkNotUsed(caller), unsigned long eventId, void* vtkNotUsed(callData))
	{

		//\\//\\Les données de chaque planète sont enregistrés dans un type Planet\\//\\//
		static Planet Mercury(PERIODEmercure, MASSEmercure, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilmercure / PERIODEmercure), DISTANCEsoleilmercure, 0);
		static Planet Venus(PERIODEvenus, MASSEvenus, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilvenus / PERIODEvenus), DISTANCEsoleilvenus, 0);
		static Planet Earth(PERIODEterre, MASSEterre, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilterre / PERIODEterre), DISTANCEsoleilterre, 0);
		static Planet Mars(PERIODEmars, MASSEmars, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilmars / PERIODEmars), DISTANCEsoleilmars, 0);
		static Planet Jupiter(PERIODEjupiter, MASSEjupiter, MASSEsoleil, 0, (2. * pi * DISTANCEsoleiljupiter / PERIODEjupiter), DISTANCEsoleiljupiter, 0);
		static Planet Saturn(PERIODEsaturne, MASSEsaturne, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilsaturne / PERIODEsaturne), DISTANCEsoleilsaturne, 0);

		
		int LockIN = 3600;
		static double position_Mercury[3] = { 0, 0, 0 };
		static double position_Venus[3] = { 0, 0, 0 };
		static double position_Earth[3] = { 0, 0, 0 };
		static double position_Mars[3] = { 0, 0, 0 };
		static double position_Jupiter[3] = { 0, 0, 0 };
		static double position_Saturn[3] = { 0, 0, 0 };
		while (LockIN != 0) {
			--LockIN;
			std::cout << "Status : " << LockIN << std::endl;

			//\\//\\Lors de chaque interruptions je calcule la position de chaque planète\\//\\//
			Mercury.distance();
			Mercury.Update_position_Runge_Kutta(2);
			Mercury.Update_position_Runge_Kutta(1);
			position_Mercury[0] = rescale_coordinates(1, Mercury.get_position_x());
			position_Mercury[1] = rescale_coordinates(1, Mercury.get_position_y());


			Venus.distance();
			Venus.Update_position_Runge_Kutta(2);
			Venus.Update_position_Runge_Kutta(1);
			position_Venus[0] = rescale_coordinates(1, Venus.get_position_x());
			position_Venus[1] = rescale_coordinates(1, Venus.get_position_y());

			Earth.distance();
			Earth.Update_position_Runge_Kutta(2);
			Earth.Update_position_Runge_Kutta(1);
			position_Earth[0] = rescale_coordinates(1, Earth.get_position_x());
			position_Earth[1] = rescale_coordinates(1, Earth.get_position_y());

			Mars.distance();
			Mars.Update_position_Runge_Kutta(2);
			Mars.Update_position_Runge_Kutta(1);
			position_Mars[0] = rescale_coordinates(1, Mars.get_position_x());
			position_Mars[1] = rescale_coordinates(1, Mars.get_position_y());

			Jupiter.distance();
			Jupiter.Update_position_Runge_Kutta(2);
			Jupiter.Update_position_Runge_Kutta(1);
			position_Jupiter[0] = rescale_coordinates(1, Jupiter.get_position_x());
			position_Jupiter[1] = rescale_coordinates(1, Jupiter.get_position_y());

			Saturn.distance();
			Saturn.Update_position_Runge_Kutta(2);
			Saturn.Update_position_Runge_Kutta(1);
			position_Saturn[0] = rescale_coordinates(1, Saturn.get_position_x());
			position_Saturn[1] = rescale_coordinates(1, Saturn.get_position_y());

		}

		

		

		if (vtkCommand::TimerEvent == eventId)
		{
			++this->TimerCount;
			//\\//\\Mise à jour de l'affichage graphique\\//\\//

			//Drawing Mercury Orbit
			static int Mercury_Callback_Counter = 16;
			if (Mercury_Callback_Counter != 0) {
				--Mercury_Callback_Counter;
				static double last_position_Mercury[3] = { position_Mercury[0], position_Mercury[1], position_Mercury[2] };
				Mercury_Orbit->SetPoint1(last_position_Mercury);
				Mercury_Orbit->SetPoint2(position_Mercury);
				Mercury_Orbit->Update();
				mapperLine->SetInputConnection(Mercury_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine = vtkSmartPointer<vtkActor>::New();
				actorLine->SetMapper(mapperLine);
				actorLine->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine);
				renderer->AddActor(DrawLine(position_Mercury, last_position_Mercury));
				for (int i = 0; i < 3; i++) last_position_Mercury[i] = position_Mercury[i]; //update position
			}
			

			//Drawing Venus Orbit
			static int Venus_Callback_Counter = 37;
			static double last_position_Venus[3] = { position_Venus[0], position_Venus[1], position_Venus[2] };
			if (Venus_Callback_Counter != 0) {
				--Venus_Callback_Counter;
				//create the orbit
				Venus_orbit->SetPoint1(last_position_Venus);
				Venus_orbit->SetPoint2(position_Venus);
				Venus_orbit->Update();
				mapperLine_Venus->SetInputConnection(Venus_orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Venus = vtkSmartPointer<vtkActor>::New();
				actorLine_Venus->SetMapper(mapperLine_Venus);
				actorLine_Venus->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Venus);
				renderer->AddActor(DrawLine(position_Venus, last_position_Venus));
				for (int i = 0; i < 3; i++) last_position_Venus[i] = position_Venus[i]; //update position
			}

			//Drawing Earth Orbit
			static int Earth_Callback_Counter = 61;
			static double last_position_Earth[3] = { position_Earth[0], position_Earth[1], position_Earth[2] };
			if (Earth_Callback_Counter != 0) {
				--Earth_Callback_Counter;
				//create the orbit
				Earth_Orbit->SetPoint1(last_position_Earth);
				Earth_Orbit->SetPoint2(position_Earth);
				Earth_Orbit->Update();
				mapperLine_Earth->SetInputConnection(Earth_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Earth = vtkSmartPointer<vtkActor>::New();
				actorLine_Earth->SetMapper(mapperLine_Earth);
				actorLine_Earth->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Earth);
				renderer->AddActor(DrawLine(position_Earth, last_position_Earth));
				for (int i = 0; i < 3; i++) last_position_Earth[i] = position_Earth[i]; //update position

			}
			
			//Drawing Mars Orbit
			static int Mars_Callback_Counter = 100;
			static double last_position_Mars[3] = { position_Mars[0], position_Mars[1], position_Mars[2] };
			if (Mars_Callback_Counter != 0) {
				--Mars_Callback_Counter;
				//create the orbit
				Mars_Orbit->SetPoint1(last_position_Mars);
				Mars_Orbit->SetPoint2(position_Mars);
				Mars_Orbit->Update();
				mapperLine_Mars->SetInputConnection(Mars_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Mars = vtkSmartPointer<vtkActor>::New();
				actorLine_Mars->SetMapper(mapperLine_Mars);
				actorLine_Mars->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Mars);
				renderer->AddActor(DrawLine(position_Mars, last_position_Mars));
				for (int i = 0; i < 3; i++) last_position_Mars[i] = position_Mars[i]; //update position

			}

			//Drawing Jupiter Orbit
			static int Jupiter_Callback_Counter = 550;
			static double last_position_Jupiter[3] = { position_Jupiter[0], position_Jupiter[1], position_Jupiter[2] };
			if (Jupiter_Callback_Counter != 0) {
				--Jupiter_Callback_Counter;
				//create the orbit
				Jupiter_Orbit->SetPoint1(last_position_Jupiter);
				Jupiter_Orbit->SetPoint2(position_Jupiter);
				Jupiter_Orbit->Update();
				mapperLine_Jupiter->SetInputConnection(Jupiter_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Jupiter = vtkSmartPointer<vtkActor>::New();
				actorLine_Jupiter->SetMapper(mapperLine_Jupiter);
				actorLine_Jupiter->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Jupiter);
				renderer->AddActor(DrawLine(position_Jupiter, last_position_Jupiter));
				for (int i = 0; i < 3; i++) last_position_Jupiter[i] = position_Jupiter[i]; //update position

			}

			//Drawing Saturn Orbit
			static int Saturn_Callback_Counter = 900;
			static double last_position_Saturn[3] = { position_Saturn[0], position_Saturn[1], position_Saturn[2] };
			if (Saturn_Callback_Counter != 0) {
				--Saturn_Callback_Counter;
				Saturn_Orbit->SetPoint1(last_position_Saturn);
				Saturn_Orbit->SetPoint2(position_Saturn);
				Saturn_Orbit->Update();
				mapperLine_Saturn->SetInputConnection(Saturn_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Saturn = vtkSmartPointer<vtkActor>::New();
				actorLine_Saturn->SetMapper(mapperLine_Saturn);
				actorLine_Saturn->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Saturn);
				renderer->AddActor(DrawLine(position_Saturn, last_position_Saturn));
				for (int i = 0; i < 3; i++) last_position_Saturn[i] = position_Saturn[i]; //update position
			}
			


			actor_Mercury->SetPosition(position_Mercury);
			actor_Venus->SetPosition(position_Venus);
			actor_Earth->SetPosition(position_Earth);
			actor_Mars->SetPosition(position_Mars);
			actor_Jupiter->SetPosition(position_Jupiter);
			actor_Saturn->SetPosition(position_Saturn);
			actor_Saturn_Rings->SetPosition(position_Saturn);
			renderWindow->Render();

		}
	}



	vtkSmartPointer<vtkActor> actor_Sun;
	vtkSmartPointer<vtkActor> actor_Mercury;
	vtkSmartPointer<vtkActor> actor_Venus;
	vtkSmartPointer<vtkActor> actor_Earth;
	vtkSmartPointer<vtkActor> actor_Mars;
	vtkSmartPointer<vtkActor> actor_Jupiter;
	vtkSmartPointer<vtkActor> actor_Saturn;
	vtkSmartPointer<vtkActor> actor_Saturn_Rings;
	vtkSmartPointer<vtkActor> actor_Uranus;
	vtkSmartPointer<vtkActor> actor_Neptune;
	vtkSmartPointer<vtkActor> actor_Moon;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkRenderWindow> renderWindow;

	//\\//\\ Mapper for Planet line orbit
	vtkSmartPointer<vtkPolyDataMapper> mapperLine = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperLine_Venus = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperLine_Earth = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperLine_Mars = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperLine_Jupiter = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperLine_Saturn = vtkSmartPointer<vtkPolyDataMapper>::New();

	//\\//\\Je crée des vtkLineSource pour le tracé des orbites\\//\\//
	vtkSmartPointer<vtkLineSource> Mercury_Orbit = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> Venus_orbit = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> Earth_Orbit = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> Mars_Orbit = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> Jupiter_Orbit = vtkSmartPointer<vtkLineSource>::New();
	vtkSmartPointer<vtkLineSource> Saturn_Orbit = vtkSmartPointer<vtkLineSource>::New();
private:
	int TimerCount;

};

/*=========================================================================================================================
	int main(int argc, char* argv[])
	Fonction : Programme principal :
		1 -> Je lis les images de texture
		2 -> Je crée chaque sphére de chaque planète en appliquant la texture
		3 -> Je crée les mappers
		4 -> Je crée les acteurs
		5 -> Je crée le renderer et le renderer window
		6 -> J'ajoute les acteurs dans le renderer
		7 -> J'initialise l'interracteur et le programme d'interruption
==========================================================================================================================*/

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0]
			<< " texture(.png)"
			<< " [translate]" << std::endl;
		return EXIT_FAILURE;
	}
	double translate[3];
	if (argc > 2)
	{
		translate[0] = atof(argv[0]);
	}
	else
	{
		translate[0] = 0.0;
	}
	translate[1] = 0.0;
	translate[2] = 0.0;
	std::cout << translate[0] << ", "
		<< translate[1] << ", "
		<< translate[2] << "\n";


	//vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
	// Sign up to receive TimerEvent
	vtkSmartPointer<vtkTimerCallback> cb = vtkSmartPointer<vtkTimerCallback>::New();

	/////////////////////////////////UPDATE/////////////////////////////////////////////
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Planet[NB_Planet];
	vtkSmartPointer<vtkImageReader2Factory> readerFactory[NB_Planet];
	vtkImageReader2* imageReader[NB_Planet];

	// Create texture
	vtkSmartPointer<vtkTexture> texture [NB_Planet];
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture [NB_Planet];
	
	for (int i = 0; i < NB_Planet; i++) {
		Sphere_Planet [i] = vtkSmartPointer<vtkTexturedSphereSource>::New();
		readerFactory [i] = vtkSmartPointer<vtkImageReader2Factory>::New();
		texture [i] = vtkSmartPointer<vtkTexture>::New();
		transformTexture[i] = vtkSmartPointer<vtkTransformTextureCoords>::New();
	}

	for (int i = 0; i < NB_Planet; i++) {
		if (i == 0) Sphere_Planet[0]->SetRadius(35.0);
		else Sphere_Planet[i]->SetRadius(35.0 - (35.0-(2.0*i + 3.0)) );
		
		// Make the surface smooth.
		Sphere_Planet[i]->SetPhiResolution(100);
		Sphere_Planet[i]->SetThetaResolution(100);

		imageReader [i] = readerFactory[i]->CreateImageReader2(argv[i+1]);
		imageReader [i]->SetFileName(argv[i+1]);

		texture [i]->SetInputConnection(imageReader [i]->GetOutputPort());
		transformTexture [i]->SetInputConnection(Sphere_Planet [i]->GetOutputPort());
		transformTexture [i]->SetPosition(translate);
	}

	///////////////////////////////////////////////////////////////////////////////////////


	/***************************************** Create Saturn Ring **********************************************/
	vtkSmartPointer<vtkDiskSource> Saturn_Rings = vtkSmartPointer<vtkDiskSource>::New();
	Saturn_Rings->SetInnerRadius(16);
	Saturn_Rings->SetOuterRadius(19);
	Saturn_Rings->SetRadialResolution(100);
	Saturn_Rings->SetCircumferentialResolution(100);

	/*********************************************** Create Belt Asteroid ***************************************/

	vtkSmartPointer<vtkDiskSource> Asteroid_Belt = vtkSmartPointer<vtkDiskSource>::New();
	Asteroid_Belt->SetInnerRadius(rescale_coordinates(1, 503E9));
	Asteroid_Belt->SetOuterRadius(rescale_coordinates(1, 303E9));
	Asteroid_Belt->SetRadialResolution(5);
	Asteroid_Belt->SetCircumferentialResolution(5);

	// Create a point cloud
	vtkSmartPointer<vtkPointSource> pointSource = vtkSmartPointer<vtkPointSource>::New();
	pointSource->SetCenter(rescale_coordinates(1, 503E9), 0.0, 0.0);
	pointSource->SetNumberOfPoints(1000);
	pointSource->SetRadius(rescale_coordinates(1, 1E9));
	pointSource->SetDistributionToShell();
	pointSource->Update();

	//////////////////////////////////////////////////////////UPDATE////////////////////////////////////////////
	vtkSmartPointer<vtkPolyDataMapper> mapper [NB_Planet];

	for (int i = 0; i < NB_Planet; i++) {
		mapper [i] = vtkSmartPointer<vtkPolyDataMapper>::New();
	}
	//making up the mapper 
	vtkSmartPointer<vtkPolyDataMapper> mapperSaturn_Rings = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperOrion_Belt = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperMoon = vtkSmartPointer<vtkPolyDataMapper>::New();
	
	for (int i = 0; i < NB_Planet; i++) {
		mapper[i]->SetInputConnection(transformTexture[i]->GetOutputPort());
	}

	mapperSaturn_Rings->SetInputConnection(Saturn_Rings->GetOutputPort());
	mapperOrion_Belt->SetInputConnection(pointSource->GetOutputPort());

	//creating the actor
	double Position_Sun[3] = { 0, 0, 0 };
	double Position_Mercury[3] = { rescale_coordinates(1, DISTANCEsoleilmercure), 0, 0 };
	double Position_Venus[3] = { rescale_coordinates(1, DISTANCEsoleilvenus), 0, 0 };
	double Position_Earth[3] = { rescale_coordinates(1, DISTANCEsoleilterre), 0, 0 };
	double Position_Mars[3] = { rescale_coordinates(1, DISTANCEsoleilmars), 0, 0 };
	double Position_Jupiter[3] = { rescale_coordinates(1, DISTANCEsoleiljupiter), 0, 0 };
	double Position_Saturn[3] = { rescale_coordinates(1, DISTANCEsoleilsaturne), 0, 0 };
	double Position_Saturn_Rings[3] = { rescale_coordinates(1, DISTANCEsoleilsaturne), 0, 0 };
	double Position_Orion_Belt[3] = { 0, 0, 0 };

	vtkSmartPointer<vtkActor> actor [NB_Planet]; 

	for (int i = 0; i < NB_Planet; i++) {
		actor [i] = vtkSmartPointer<vtkActor>::New();
		actor[i]->SetMapper(mapper[i]);
		actor[i]->SetTexture(texture[i]);
	}

	cb->actor_Mercury = actor[1];
	cb->actor_Venus = actor[2];
	cb->actor_Earth = actor[3];
	cb->actor_Mars = actor[4];
	cb->actor_Jupiter = actor[5];
	cb->actor_Saturn = actor[6];

	actor[0]->SetPosition(Position_Sun);
	actor[1]->SetPosition(Position_Mercury);
	actor[2]->SetPosition(Position_Venus);
	actor[3]->SetPosition(Position_Earth);
	actor[4]->SetPosition(Position_Mars);
	actor[5]->SetPosition(Position_Jupiter);
	actor[6]->SetPosition(Position_Saturn);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	vtkSmartPointer<vtkActor> actor_Saturn_Rings = vtkSmartPointer<vtkActor>::New();
	cb->actor_Saturn_Rings = actor_Saturn_Rings;
	actor_Saturn_Rings->SetPosition(Position_Saturn_Rings);
	actor_Saturn_Rings->SetMapper(mapperSaturn_Rings);
	actor_Saturn_Rings->GetProperty()->SetColor(0.96078, 0.96078, 0.86274);
	actor_Saturn_Rings->SetTexture(texture[6]);

	vtkSmartPointer<vtkActor> actor_Orion_Belt = vtkSmartPointer<vtkActor>::New();
	actor_Orion_Belt->SetPosition(Position_Orion_Belt);
	actor_Orion_Belt->SetMapper(mapperOrion_Belt);
	actor_Orion_Belt->GetProperty()->SetColor(0.96078, 0.96078, 0.86274);

	// Setup renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	cb->renderer = renderer;
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	for (int i = 0; i < NB_Planet; i++) {
		renderer->AddActor(actor[i]);
	}
	renderer->AddActor(actor_Saturn_Rings);
	renderer->AddActor(actor_Orion_Belt);

	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);



	// Initialize must be called prior to creating timer events.
	interactor->Initialize();

	cb->renderWindow = renderWindow;
	interactor->AddObserver(vtkCommand::TimerEvent, cb);

	
	int timerId = interactor->CreateRepeatingTimer(0.01);
	std::cout << "timerId: " << timerId << std::endl;
	
	

	// Start the interaction and timer

	renderWindow->Render();

	interactor->Start();

	getchar();

	return 0;
}
