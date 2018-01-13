#include "cComTrigger.h"

#include <iostream>
#include <GLFW/glfw3.h>

cComTrigger::cComTrigger()
{
	initPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	finalPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	direction = glm::vec3( 0.0f, 0.0f, 0.0f );
	distanceToTarget = 0.0f;
	velocity = 0.0f;
	initialTime = 0;

	return;
}

cComTrigger::~cComTrigger()
{
	return;
}

void cComTrigger::init( glm::vec3 destination, float time )
{
	this->initPosition = this->theGO->position;
	this->finalPosition = destination;
	this->direction = glm::normalize( finalPosition - initPosition );
	this->distanceToTarget = glm::distance( finalPosition, initPosition );
	this->duration = time;

	// This is the average velocity it would take to reach the destination
	// within this time
	this->velocity = distanceToTarget / time;
	
	return;
}

void cComTrigger::update( double deltaTime )
{
	return;
}


bool cComTrigger::isDone()
{
	// If the GO is on destination, clear the velocity 
	if( this->theGO->position == this->finalPosition )
	{
		// Set the Velocity
		this->theGO->vel = glm::vec3( 0.0f, 0.0f, 0.0f );
		return true;
	}
	else
		return false;
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
