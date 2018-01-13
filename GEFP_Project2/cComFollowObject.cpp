#include "cComFollowObject.h"

#include <iostream>
#include <GLFW/glfw3.h>

cComFollowObject::cComFollowObject()
{
	initPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	finalPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	direction = glm::vec3( 0.0f, 0.0f, 0.0f );
	distanceToTarget = 0.0f;
	velocity = 0.0f;
	initialTime = 0;

	return;
}

cComFollowObject::~cComFollowObject()
{
	return;
}

void cComFollowObject::init( glm::vec3 destination, float time )
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

void cComFollowObject::update( double deltaTime )
{
	return;
}


bool cComFollowObject::isDone()
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

void cComFollowObject::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComFollowObject::getMyGO()
{
	return this->theGO;
}
