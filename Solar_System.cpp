

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

	virtual void Execute(vtkObject* vtkNotUsed(caller), unsigned long eventId, void* vtkNotUsed(callData))
	{

		//\\//\\Les données de chaque planète sont enregistrés dans un type Planet\\//\\//
		static Planet Mercury(PERIODEmercure, MASSEmercure, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilmercure / PERIODEmercure), DISTANCEsoleilmercure, 0);
		static Planet Venus(PERIODEvenus, MASSEvenus, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilvenus / PERIODEvenus), DISTANCEsoleilvenus, 0);
		static Planet Earth(PERIODEterre, MASSEterre, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilterre / PERIODEterre), DISTANCEsoleilterre, 0);
		static Planet Mars(PERIODEmars, MASSEmars, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilmars / PERIODEmars), DISTANCEsoleilmars, 0);
		static Planet Jupiter(PERIODEjupiter, MASSEjupiter, MASSEsoleil, 0, (2. * pi * DISTANCEsoleiljupiter / PERIODEjupiter), DISTANCEsoleiljupiter, 0);
		static Planet Saturn(PERIODEsaturne, MASSEsaturne, MASSEsoleil, 0, (2. * pi * DISTANCEsoleilsaturne / PERIODEsaturne), DISTANCEsoleilsaturne, 0);

		//\\//\\Je crée des vtkLineSource pour le tracé des orbites\\//\\//
		vtkSmartPointer<vtkLineSource> Mercury_Orbit = vtkSmartPointer<vtkLineSource>::New();
		vtkSmartPointer<vtkLineSource> Venus_orbit = vtkSmartPointer<vtkLineSource>::New();
		vtkSmartPointer<vtkLineSource> Earth_Orbit = vtkSmartPointer<vtkLineSource>::New();
		vtkSmartPointer<vtkLineSource> Mars_Orbit = vtkSmartPointer<vtkLineSource>::New();
		vtkSmartPointer<vtkLineSource> Jupiter_Orbit = vtkSmartPointer<vtkLineSource>::New();
		vtkSmartPointer<vtkLineSource> Saturn_Orbit = vtkSmartPointer<vtkLineSource>::New();

		if (vtkCommand::TimerEvent == eventId)
		{
			++this->TimerCount;

			//\\//\\Lors de chaque interruptions je calcule la position de chaque planète\\//\\//
			Mercury.distance();
			Mercury.Update_position_Runge_Kutta(2);
			Mercury.Update_position_Runge_Kutta(1);
			static double position_Mercury[3] = { 0, 0, 0 };
			position_Mercury[0] = rescale_coordinates(1, Mercury.get_position_x());
			position_Mercury[1] = rescale_coordinates(1, Mercury.get_position_y());


			Venus.distance();
			Venus.Update_position_Runge_Kutta(2);
			Venus.Update_position_Runge_Kutta(1);
			static double position_Venus[3] = { 0, 0, 0 };
			position_Venus[0] = rescale_coordinates(1, Venus.get_position_x());
			position_Venus[1] = rescale_coordinates(1, Venus.get_position_y());

			Earth.distance();
			Earth.Update_position_Runge_Kutta(2);
			Earth.Update_position_Runge_Kutta(1);
			static double position_Earth[3] = { 0, 0, 0 };
			position_Earth[0] = rescale_coordinates(1, Earth.get_position_x());
			position_Earth[1] = rescale_coordinates(1, Earth.get_position_y());

			Mars.distance();
			Mars.Update_position_Runge_Kutta(2);
			Mars.Update_position_Runge_Kutta(1);
			static double position_Mars[3] = { 0, 0, 0 };
			position_Mars[0] = rescale_coordinates(1, Mars.get_position_x());
			position_Mars[1] = rescale_coordinates(1, Mars.get_position_y());

			Jupiter.distance();
			Jupiter.Update_position_Runge_Kutta(2);
			Jupiter.Update_position_Runge_Kutta(1);
			static double position_Jupiter[3] = { 0, 0, 0 };
			position_Jupiter[0] = rescale_coordinates(1, Jupiter.get_position_x());
			position_Jupiter[1] = rescale_coordinates(1, Jupiter.get_position_y());

			Saturn.distance();
			Saturn.Update_position_Runge_Kutta(2);
			Saturn.Update_position_Runge_Kutta(1);
			static double position_Saturn[3] = { 0, 0, 0 };
			position_Saturn[0] = rescale_coordinates(1, Saturn.get_position_x());
			position_Saturn[1] = rescale_coordinates(1, Saturn.get_position_y());

			//\\//\\Avec les nouvelles positions, j'appelle le programme DrawLine pour tracer l'orbite \\//\\//
			
			//Drawing Mercury Orbit
			static int cpt_Mercury = 0;		// Compteur qui me sert de temporisation pour éviter de tracer une ligne à chaque interruption
			cpt_Mercury++;		
			static double last_position_Mercury[3] = { position_Mercury[0], position_Mercury[1], position_Mercury[2] };
			if (cpt_Mercury == 600) {

				//create the orbit
				Mercury_Orbit->SetPoint1(last_position_Mercury);
				Mercury_Orbit->SetPoint2(position_Mercury);
				Mercury_Orbit->Update();
				vtkSmartPointer<vtkPolyDataMapper> mapperLine = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapperLine->SetInputConnection(Mercury_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine = vtkSmartPointer<vtkActor>::New();
				actorLine->SetMapper(mapperLine);
				actorLine->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine);

				renderer->AddActor(DrawLine(position_Mercury, last_position_Mercury));

				cpt_Mercury = 0;
				for (int i = 0; i < 3; i++) last_position_Mercury[i] = position_Mercury[i]; //update position

			}

			//Drawing Venus Orbit
			static int cpt_Venus = 0;		// Compteur qui me sert de temporisation pour éviter de tracer une ligne à chaque interruption
			cpt_Venus++;
			static double last_position_Venus[3] = { position_Venus[0], position_Venus[1], position_Venus[2] };
			if (cpt_Venus == 600) {

				//create the orbit
				Venus_orbit->SetPoint1(last_position_Venus);
				Venus_orbit->SetPoint2(position_Venus);
				Venus_orbit->Update();
				vtkSmartPointer<vtkPolyDataMapper> mapperLine_Venus = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapperLine_Venus->SetInputConnection(Venus_orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Venus = vtkSmartPointer<vtkActor>::New();
				actorLine_Venus->SetMapper(mapperLine_Venus);
				actorLine_Venus->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Venus);

				renderer->AddActor(DrawLine(position_Venus, last_position_Venus));

				cpt_Venus = 0;
				for (int i = 0; i < 3; i++) last_position_Venus[i] = position_Venus[i]; //update position

			}

			//Drawing Earth Orbit
			static int cpt_Earth = 0;		// Compteur qui me sert de temporisation pour éviter de tracer une ligne à chaque interruption
			cpt_Earth++;
			static double last_position_Earth[3] = { position_Earth[0], position_Earth[1], position_Earth[2] };
			if (cpt_Earth == 800) {

				//create the orbit
				Earth_Orbit->SetPoint1(last_position_Earth);
				Earth_Orbit->SetPoint2(position_Earth);
				Earth_Orbit->Update();
				vtkSmartPointer<vtkPolyDataMapper> mapperLine_Earth = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapperLine_Earth->SetInputConnection(Earth_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Earth = vtkSmartPointer<vtkActor>::New();
				actorLine_Earth->SetMapper(mapperLine_Earth);
				actorLine_Earth->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Earth);

				renderer->AddActor(DrawLine(position_Earth, last_position_Earth));

				cpt_Earth = 0;
				for (int i = 0; i < 3; i++) last_position_Earth[i] = position_Earth[i]; //update position

			}

			//Drawing Mars Orbit
			static int cpt_Mars = 0;		// Compteur qui me sert de temporisation pour éviter de tracer une ligne à chaque interruption
			cpt_Mars++;
			static double last_position_Mars[3] = { position_Mars[0], position_Mars[1], position_Mars[2] };
			if (cpt_Mars == 1000) {

				//create the orbit
				Mars_Orbit->SetPoint1(last_position_Mars);
				Mars_Orbit->SetPoint2(position_Mars);
				Mars_Orbit->Update();
				vtkSmartPointer<vtkPolyDataMapper> mapperLine_Mars = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapperLine_Mars->SetInputConnection(Mars_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Mars = vtkSmartPointer<vtkActor>::New();
				actorLine_Mars->SetMapper(mapperLine_Mars);
				actorLine_Mars->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Mars);

				renderer->AddActor(DrawLine(position_Mars, last_position_Mars));

				cpt_Mars = 0;
				for (int i = 0; i < 3; i++) last_position_Mars[i] = position_Mars[i]; //update position

			}

			//Drawing Jupiter Orbit
			static int cpt_Jupiter = 0;		// Compteur qui me sert de temporisation pour éviter de tracer une ligne à chaque interruption
			cpt_Jupiter++;
			static double last_position_Jupiter[3] = { position_Jupiter[0], position_Jupiter[1], position_Jupiter[2] };
			if (cpt_Jupiter == 3000) {

				//create the orbit
				Jupiter_Orbit->SetPoint1(last_position_Jupiter);
				Jupiter_Orbit->SetPoint2(position_Jupiter);
				Jupiter_Orbit->Update();
				vtkSmartPointer<vtkPolyDataMapper> mapperLine_Jupiter = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapperLine_Jupiter->SetInputConnection(Jupiter_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Jupiter = vtkSmartPointer<vtkActor>::New();
				actorLine_Jupiter->SetMapper(mapperLine_Jupiter);
				actorLine_Jupiter->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Jupiter);

				renderer->AddActor(DrawLine(position_Jupiter, last_position_Jupiter));

				cpt_Jupiter = 0;
				for (int i = 0; i < 3; i++) last_position_Jupiter[i] = position_Jupiter[i]; //update position

			}

			//Drawing Saturn Orbit
			static int cpt_Saturn = 0;		// Compteur qui me sert de temporisation pour éviter de tracer une ligne à chaque interruption
			cpt_Saturn++;
			static double last_position_Saturn[3] = { position_Saturn[0], position_Saturn[1], position_Saturn[2] };
			if (cpt_Saturn == 5000) {

				//create the orbit
				Saturn_Orbit->SetPoint1(last_position_Saturn);
				Saturn_Orbit->SetPoint2(position_Saturn);
				Saturn_Orbit->Update();
				vtkSmartPointer<vtkPolyDataMapper> mapperLine_Saturn = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapperLine_Saturn->SetInputConnection(Saturn_Orbit->GetOutputPort());
				vtkSmartPointer<vtkActor> actorLine_Saturn = vtkSmartPointer<vtkActor>::New();
				actorLine_Saturn->SetMapper(mapperLine_Saturn);
				actorLine_Saturn->GetProperty()->SetLineWidth(2);
				renderer->AddActor(actorLine_Saturn);

				renderer->AddActor(DrawLine(position_Saturn, last_position_Saturn));

				cpt_Saturn = 0;
				for (int i = 0; i < 3; i++) last_position_Saturn[i] = position_Saturn[i]; //update position

			}


			//\\//\\Mise à jour de l'affichage graphique\\//\\//
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

	/***************************************** Create Sun sphere **********************************************/
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Sun = vtkSmartPointer<vtkTexturedSphereSource>::New();
	Sphere_Sun->SetRadius(35.0);
	// Make the surface smooth.
	Sphere_Sun->SetPhiResolution(100);
	Sphere_Sun->SetThetaResolution(100);
	// Read texture file
	vtkSmartPointer<vtkImageReader2Factory> readerFactory_Sun = vtkSmartPointer<vtkImageReader2Factory>::New();
	vtkImageReader2* imageReader_Sun = readerFactory_Sun->CreateImageReader2(argv[1]);
	imageReader_Sun->SetFileName(argv[1]);
	// Create texture
	vtkSmartPointer<vtkTexture> texture_Sun = vtkSmartPointer<vtkTexture>::New();
	texture_Sun->SetInputConnection(imageReader_Sun->GetOutputPort());
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture_Sun = vtkSmartPointer<vtkTransformTextureCoords>::New();
	transformTexture_Sun->SetInputConnection(Sphere_Sun->GetOutputPort());
	transformTexture_Sun->SetPosition(translate);

	/***************************************** Create Mecury sphere **********************************************/
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Mercury = vtkSmartPointer<vtkTexturedSphereSource>::New();
	Sphere_Mercury->SetRadius(5.0);
	// Make the surface smooth.
	Sphere_Mercury->SetPhiResolution(100);
	Sphere_Mercury->SetThetaResolution(100);
	// Read texture file
	vtkSmartPointer<vtkImageReader2Factory> readerFactory_Mercury = vtkSmartPointer<vtkImageReader2Factory>::New();
	vtkImageReader2* imageReader_Mercury = readerFactory_Mercury->CreateImageReader2(argv[2]);
	imageReader_Mercury->SetFileName(argv[2]);
	// Create texture
	vtkSmartPointer<vtkTexture> texture_Mercury = vtkSmartPointer<vtkTexture>::New();
	texture_Mercury->SetInputConnection(imageReader_Mercury->GetOutputPort());
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture_Mercury = vtkSmartPointer<vtkTransformTextureCoords>::New();
	transformTexture_Mercury->SetInputConnection(Sphere_Mercury->GetOutputPort());
	transformTexture_Mercury->SetPosition(translate);

	/***************************************** Create Venus sphere **********************************************/
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Venus = vtkSmartPointer<vtkTexturedSphereSource>::New();
	Sphere_Venus->SetRadius(7.0);
	// Make the surface smooth.
	Sphere_Venus->SetPhiResolution(100);
	Sphere_Venus->SetThetaResolution(100);
	// Read texture file
	vtkSmartPointer<vtkImageReader2Factory> readerFactory_Venus = vtkSmartPointer<vtkImageReader2Factory>::New();
	vtkImageReader2* imageReader_Venus = readerFactory_Venus->CreateImageReader2(argv[3]);
	imageReader_Venus->SetFileName(argv[3]);
	// Create texture
	vtkSmartPointer<vtkTexture> texture_Venus = vtkSmartPointer<vtkTexture>::New();
	texture_Venus->SetInputConnection(imageReader_Venus->GetOutputPort());
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture_Venus = vtkSmartPointer<vtkTransformTextureCoords>::New();
	transformTexture_Venus->SetInputConnection(Sphere_Venus->GetOutputPort());
	transformTexture_Venus->SetPosition(translate);

	/***************************************** Create Earth sphere **********************************************/
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Earth = vtkSmartPointer<vtkTexturedSphereSource>::New();
	Sphere_Earth->SetRadius(10.0);
	// Make the surface smooth.
	Sphere_Earth->SetPhiResolution(100);
	Sphere_Earth->SetThetaResolution(100);
	// Read texture file
	vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
	vtkImageReader2* imageReader = readerFactory->CreateImageReader2(argv[4]);
	imageReader->SetFileName(argv[4]);
	// Create texture
	vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputConnection(imageReader->GetOutputPort());
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture_Earth = vtkSmartPointer<vtkTransformTextureCoords>::New();
	transformTexture_Earth->SetInputConnection(Sphere_Earth->GetOutputPort());
	transformTexture_Earth->SetPosition(translate);

	/***************************************** Create Mars sphere **********************************************/
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Mars = vtkSmartPointer<vtkTexturedSphereSource>::New();
	Sphere_Mars->SetRadius(12.0);
	// Make the surface smooth.
	Sphere_Mars->SetPhiResolution(100);
	Sphere_Mars->SetThetaResolution(100);
	// Read texture file
	vtkSmartPointer<vtkImageReader2Factory> readerFactory_Mars = vtkSmartPointer<vtkImageReader2Factory>::New();
	vtkImageReader2* imageReader_Mars = readerFactory_Mars->CreateImageReader2(argv[5]);
	imageReader_Mars->SetFileName(argv[5]);
	// Create texture
	vtkSmartPointer<vtkTexture> texture_Mars = vtkSmartPointer<vtkTexture>::New();
	texture_Mars->SetInputConnection(imageReader_Mars->GetOutputPort());
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture_Mars = vtkSmartPointer<vtkTransformTextureCoords>::New();
	transformTexture_Mars->SetInputConnection(Sphere_Mars->GetOutputPort());
	transformTexture_Mars->SetPosition(translate);

	/***************************************** Create Jupiter sphere **********************************************/
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Jupiter = vtkSmartPointer<vtkTexturedSphereSource>::New();
	Sphere_Jupiter->SetRadius(14.0);
	// Make the surface smooth.
	Sphere_Jupiter->SetPhiResolution(100);
	Sphere_Jupiter->SetThetaResolution(100);
	// Read texture file
	vtkSmartPointer<vtkImageReader2Factory> readerFactory_Jupiter = vtkSmartPointer<vtkImageReader2Factory>::New();
	vtkImageReader2* imageReader_Jupiter = readerFactory_Jupiter->CreateImageReader2(argv[6]);
	imageReader_Jupiter->SetFileName(argv[6]);
	// Create texture
	vtkSmartPointer<vtkTexture> texture_Jupiter = vtkSmartPointer<vtkTexture>::New();
	texture_Jupiter->SetInputConnection(imageReader_Jupiter->GetOutputPort());
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture_Jupiter = vtkSmartPointer<vtkTransformTextureCoords>::New();
	transformTexture_Jupiter->SetInputConnection(Sphere_Jupiter->GetOutputPort());
	transformTexture_Jupiter->SetPosition(translate);

	/***************************************** Create Saturn sphere **********************************************/
	vtkSmartPointer<vtkTexturedSphereSource> Sphere_Saturn = vtkSmartPointer<vtkTexturedSphereSource>::New();
	Sphere_Saturn->SetRadius(12.0);
	// Make the surface smooth.
	Sphere_Saturn->SetPhiResolution(100);
	Sphere_Saturn->SetThetaResolution(100);
	// Read texture file
	vtkSmartPointer<vtkImageReader2Factory> readerFactory_Saturn = vtkSmartPointer<vtkImageReader2Factory>::New();
	vtkImageReader2* imageReader_Saturn = readerFactory_Saturn->CreateImageReader2(argv[7]);
	imageReader_Saturn->SetFileName(argv[7]);
	// Create texture
	vtkSmartPointer<vtkTexture> texture_Saturn = vtkSmartPointer<vtkTexture>::New();
	texture_Saturn->SetInputConnection(imageReader_Saturn->GetOutputPort());
	vtkSmartPointer<vtkTransformTextureCoords> transformTexture_Saturn = vtkSmartPointer<vtkTransformTextureCoords>::New();
	transformTexture_Saturn->SetInputConnection(Sphere_Saturn->GetOutputPort());
	transformTexture_Saturn->SetPosition(translate);

	/***************************************** Create Saturn Ring **********************************************/
	vtkSmartPointer<vtkDiskSource> Saturn_Rings = vtkSmartPointer<vtkDiskSource>::New();
	Saturn_Rings->SetInnerRadius(16);
	Saturn_Rings->SetOuterRadius(19);
	Saturn_Rings->SetRadialResolution(100);
	Saturn_Rings->SetCircumferentialResolution(100);

	//making up the mapper 
	vtkSmartPointer<vtkPolyDataMapper> mapperSun = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperMercury = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperVenus = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperEarth = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperMars = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperJupiter = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperSaturn = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperSaturn_Rings = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapperMoon = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapperSun->SetInputConnection(transformTexture_Sun->GetOutputPort());
	mapperMercury->SetInputConnection(transformTexture_Mercury->GetOutputPort());
	mapperVenus->SetInputConnection(transformTexture_Venus->GetOutputPort());
	mapperEarth->SetInputConnection(transformTexture_Earth->GetOutputPort());
	mapperMars->SetInputConnection(transformTexture_Mars->GetOutputPort());
	mapperJupiter->SetInputConnection(transformTexture_Jupiter->GetOutputPort());
	mapperSaturn->SetInputConnection(transformTexture_Saturn->GetOutputPort());
	mapperSaturn_Rings->SetInputConnection(Saturn_Rings->GetOutputPort());


	//creating the actor
	double Star_Sun[3] = { 0, 0, 0 };
	double Planet_Mercury[3] = { rescale_coordinates(1, DISTANCEsoleilmercure), 0, 0 };
	double Planet_Venus[3] = { rescale_coordinates(1, DISTANCEsoleilvenus), 0, 0 };
	double Planet_Earth[3] = { rescale_coordinates(1, DISTANCEsoleilterre), 0, 0 };
	double Planet_Mars[3] = { rescale_coordinates(1, DISTANCEsoleilmars), 0, 0 };
	double Planet_Jupiter[3] = { rescale_coordinates(1, DISTANCEsoleiljupiter), 0, 0 };
	double Planet_Saturn[3] = { rescale_coordinates(1, DISTANCEsoleilsaturne), 0, 0 };
	double Position_Saturn_Rings[3] = { rescale_coordinates(1, DISTANCEsoleilsaturne), 0, 0 };

	vtkSmartPointer<vtkActor> actor_Sun = vtkSmartPointer<vtkActor>::New();
	actor_Sun->SetPosition(Star_Sun);
	actor_Sun->SetMapper(mapperSun);
	actor_Sun->SetTexture(texture_Sun);

	vtkSmartPointer<vtkActor> actor_Mercury = vtkSmartPointer<vtkActor>::New();
	cb->actor_Mercury = actor_Mercury;
	actor_Mercury->SetPosition(Planet_Mercury);
	actor_Mercury->SetMapper(mapperMercury);
	actor_Mercury->SetTexture(texture_Mercury);

	vtkSmartPointer<vtkActor> actor_Venus = vtkSmartPointer<vtkActor>::New();
	cb->actor_Venus = actor_Venus;
	actor_Venus->SetPosition(Planet_Venus);
	actor_Venus->SetMapper(mapperVenus);
	actor_Venus->SetTexture(texture_Venus);

	vtkSmartPointer<vtkActor> actor_Earth = vtkSmartPointer<vtkActor>::New();
	cb->actor_Earth = actor_Earth;
	actor_Earth->SetPosition(Planet_Earth);
	actor_Earth->SetMapper(mapperEarth);
	actor_Earth->SetTexture(texture);

	vtkSmartPointer<vtkActor> actor_Mars = vtkSmartPointer<vtkActor>::New();
	cb->actor_Mars = actor_Mars;
	actor_Mars->SetPosition(Planet_Mars);
	actor_Mars->SetMapper(mapperMars);
	actor_Mars->SetTexture(texture_Mars);

	vtkSmartPointer<vtkActor> actor_Jupiter = vtkSmartPointer<vtkActor>::New();
	cb->actor_Jupiter = actor_Jupiter;
	actor_Jupiter->SetPosition(Planet_Jupiter);
	actor_Jupiter->SetMapper(mapperJupiter);
	actor_Jupiter->SetTexture(texture_Jupiter);

	vtkSmartPointer<vtkActor> actor_Saturn = vtkSmartPointer<vtkActor>::New();
	cb->actor_Saturn = actor_Saturn;
	actor_Saturn->SetPosition(Planet_Saturn);
	actor_Saturn->SetMapper(mapperSaturn);
	actor_Saturn->SetTexture(texture_Saturn);

	vtkSmartPointer<vtkActor> actor_Saturn_Rings = vtkSmartPointer<vtkActor>::New();
	cb->actor_Saturn_Rings = actor_Saturn_Rings;
	actor_Saturn_Rings->SetPosition(Position_Saturn_Rings);
	actor_Saturn_Rings->SetMapper(mapperSaturn_Rings);
	actor_Saturn_Rings->GetProperty()->SetColor(0.96078, 0.96078, 0.86274);
	actor_Saturn_Rings->SetTexture(texture_Saturn);

	// Setup renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	cb->renderer = renderer;
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	renderer->AddActor(actor_Sun);
	renderer->AddActor(actor_Mercury);
	renderer->AddActor(actor_Venus);
	renderer->AddActor(actor_Earth);
	renderer->AddActor(actor_Mars);
	renderer->AddActor(actor_Jupiter);
	renderer->AddActor(actor_Saturn);
	renderer->AddActor(actor_Saturn_Rings);

	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);



	// Initialize must be called prior to creating timer events.
	interactor->Initialize();

	cb->renderWindow = renderWindow;
	interactor->AddObserver(vtkCommand::TimerEvent, cb);

	int timerId = interactor->CreateRepeatingTimer(000.1);
	std::cout << "timerId: " << timerId << std::endl;

	// Start the interaction and timer

	renderWindow->Render();
	interactor->Start();

	getchar();

	return 0;
}
