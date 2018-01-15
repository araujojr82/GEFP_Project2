#include "cComTrigger.h"

#include <iostream>
#include <sstream>
#include <fstream>      // std::ifstream
#include <GLFW/glfw3.h>
#include "cLuaBrain.h"

void cComTrigger::setMyID( int myID )
{	
	this->m_uniqueID = myID;
	return;
}

extern cLuaBrain* p_LuaScripts;

cComTrigger::cComTrigger()
{
	this->triggerPoint = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->distanceToTarget = 0.0f;
	this->radius = 0.0f;
	this->initialTime = 0;

	this->hasStarted = false;
	this->isCommandDone = false;

	return;
}

cComTrigger::~cComTrigger()
{
	return;
}

void cComTrigger::init( glm::vec3 theTriggerPoint, float theRadius, glm::vec3 unused )
{
	this->triggerPoint = theTriggerPoint;
	this->radius = theRadius;
	//this->finalPosition = destination;
	//this->direction = glm::normalize( finalPosition - initPosition );
	//this->duration = time;

	// This is the average velocity it would take to reach the destination
	// within this time
	//this->velocity = distanceToTarget / time;
	
	return;
}

extern std::stringstream loadScriptFile( std::string fileName );

void cComTrigger::createSubGroup( std::string theSubScript )
{
	// Load the new lua script from file
	std::stringstream ssTheScript = loadScriptFile( theSubScript );

	// Print the script just in case
	std::cout << ssTheScript.str() << std::endl;

	int pos = theSubScript.find_first_of( '.' );
	this->scriptName = theSubScript.substr( 0, pos );

	this->theScript = ssTheScript.str();

	return;
}

void cComTrigger::update( double deltaTime )
{
	if( !this->hasStarted )
	{
		this->distanceToTarget = glm::distance( this->triggerPoint, this->theGO->position );
		if( distanceToTarget <= this->radius )
		{
			this->hasStarted = true;
			::p_LuaScripts->LoadScript( this->scriptName, this->theScript );

			// Update the lua script manager
			::p_LuaScripts->Update();
			::p_LuaScripts->DeleteScript( this->scriptName );

			this->isCommandDone = true;
		}
	}
	return;
}


bool cComTrigger::isDone()
{
	if( this->isCommandDone ) return true;

	if( this->hasStarted ) return true;
	else return false;
}

void cComTrigger::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComTrigger::getMyGO()
{
	return this->theGO;
}
