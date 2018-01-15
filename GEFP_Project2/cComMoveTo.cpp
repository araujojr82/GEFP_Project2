#include "cComMoveTo.h"

#include <iostream>
#include <GLFW/glfw3.h>

void cComMoveTo::setMyID( int myID )
{
	this->m_uniqueID = myID;
	return;
}

cComMoveTo::cComMoveTo()
{
	this->initPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->finalPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->direction = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->distanceToTarget = 0.0f;
	this->velocity = 0.0f;
	this->initialTime = 0;
	this->elapsedTime = 0;

	this->hasStarted = false;
	this->isCommandDone = false;

	return;
}

cComMoveTo::~cComMoveTo()
{
	return;
}

void cComMoveTo::init( glm::vec3 destination, float time, glm::vec3 notBeingUsed )
{
	this->finalPosition = destination;
	this->duration = time;

	return;
}

void cComMoveTo::update( double deltaTime )
{

	if( this->hasStarted == false )
	{
		if( this->targetGO != nullptr ) // Get the position from the target GO
			this->finalPosition = this->targetGO->position;

		this->initialTime = glfwGetTime();
		this->hasStarted = true;
		this->initPosition = this->theGO->position;
		this->direction = glm::normalize( finalPosition - initPosition );
		this->distanceToTarget = glm::distance( finalPosition, initPosition );
		// This is the average velocity it would take to reach the destination
		this->velocity = (float) this->distanceToTarget / this->duration;

	}
	
	float range1 = 0.02 * this->distanceToTarget;
	float range2 = 0.98 * this->distanceToTarget;

	this->elapsedTime = glfwGetTime() - this->initialTime;

	float traveledDistance = glm::distance( this->initPosition, this->theGO->position );

	// Using the smooth step 2x to calculate aceleration and deceleration
	float factor = glm::smoothstep( -0.1f, range1, traveledDistance ) * ( 1 - glm::smoothstep( range2, this->distanceToTarget + 0.1f , traveledDistance ) );

	factor = factor * this->velocity;

	//this->direction = glm::normalize( this->finalPosition - this->theGO->position );

	this->theGO->vel = this->direction * factor;

	// Calculate delta position according to the velocity based on time elapsed
	glm::vec3 deltaPosition = ( float )deltaTime * this->theGO->vel;

	// Where the object will be
	glm::vec3 nextPosition = this->theGO->position += deltaPosition;
	float nextDistance = glm::distance( finalPosition, nextPosition );
				
	this->theGO->position += deltaPosition;

	//std::cout
	//	<< "ID: " << this->theGO->getID()
	//	<< " | Max.vel: " << this->velocity
	//	<< " | Elapsed Time: " << this->elapsedTime
	//	<< " | Vel.: " << this->theGO->vel.x
	//	<< ", " << this->theGO->vel.y << ", " << this->theGO->vel.z
	//	<< " - Traveled distancet: " << traveledDistance
	//	//<< " | Distance: " << remainingDistance
	//	<< std::endl;
	return;
}


bool cComMoveTo::isDone()
{
	if( this->isCommandDone ) return true;

	// If the GO is on destination, clear the velocity 
	if( this->theGO->position == this->finalPosition ||
		this->elapsedTime >= this->duration )
	{
		this->isCommandDone = true;
		// Set the Velocity
		this->theGO->vel = glm::vec3( 0.0f, 0.0f, 0.0f );
		return true;
	}
	else
		return false;
}

void cComMoveTo::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComMoveTo::getMyGO()
{
	return this->theGO;
}

void cComMoveTo::setTargetGO( cGameObject* target )
{
	this->targetGO = target;
	return;
}

cGameObject* cComMoveTo::getTargetGO()
{
	return this->targetGO;
}
