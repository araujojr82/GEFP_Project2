#include "cComMoveTo.h"

#include <iostream>
#include <GLFW/glfw3.h>

cComMoveTo::cComMoveTo()
{
	initPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	finalPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	direction = glm::vec3( 0.0f, 0.0f, 0.0f );
	distanceToTarget = 0.0f;
	velocity = 0.0f;
	initialTime = 0;

	return;
}

cComMoveTo::~cComMoveTo()
{
	return;
}

void cComMoveTo::init( glm::vec3 destination, float time, glm::vec3 notBeingUsed )
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

void cComMoveTo::update( double deltaTime )
{
	// Start the time 
	if( this->initialTime == 0.0f )
	{
		this->initialTime = glfwGetTime();
	}
	
	float range1 = 0.02 * this->distanceToTarget;
	float range2 = 0.98 * this->distanceToTarget;

	float elapsedTime = glfwGetTime() - this->initialTime;

	float traveledDistance = glm::distance( this->initPosition, this->theGO->position );
	// It must have some value on the first calculation, otherwise it will return 0
	// Let's give it 1% of the total distance
	//if( traveledDistance == 0 ) traveledDistance = 0.01 * this->distanceToTarget;

	// Calculate remaining distance
	float remainingDistance = glm::distance( this->finalPosition, this->theGO->position );

	// If the remaining distance is less then 1%, just move it there and stop
	if( remainingDistance / this->distanceToTarget <= 0.01 )
	{
		this->theGO->position = this->finalPosition;
		// Set the Velocity to 0
		this->theGO->vel = glm::vec3( 0.0f, 0.0f, 0.0f );

		//		return;
	}

	else
	{
		// Using the smooth step 2x to calculate aceleration and deceleration
		float factor = glm::smoothstep( -0.1f, range1, traveledDistance ) * ( 1 - glm::smoothstep( range2, this->distanceToTarget, traveledDistance ) );

		factor *= this->velocity;

		this->direction = glm::normalize( finalPosition - this->theGO->position );

		this->theGO->vel = this->direction * factor;

		// Calculate delta position according to the velocity based on time elapsed
		glm::vec3 deltaPosition = ( float )deltaTime * this->theGO->vel;

		// Where the object will be
		glm::vec3 nextPosition = this->theGO->position += deltaPosition;
		float nextDistance = glm::distance( finalPosition, nextPosition );
		//glm::vec3 nextDirection = glm::normalize( finalPosition - nextPosition );
				
		//if( nextDirection != this->direction )
		if( nextDistance > remainingDistance )
		{ // It means it has inverted the direction, or that it has passed the final position
		  	this->theGO->position = this->finalPosition;
		  	// Set the Velocity to 0
		  	this->theGO->vel = glm::vec3( 0.0f, 0.0f, 0.0f );
		}
		else
		{
			this->theGO->position += deltaPosition;
		}			
	}

	std::cout << "Velocity x: " << this->theGO->vel.x
		<< " / y: " << this->theGO->vel.y << " / z: " << this->theGO->vel.z
		<< " - Elapsed Time: " << elapsedTime
		<< " - Traveled distancet: " << traveledDistance
		<< " - Distance to the target: " << remainingDistance
		<< std::endl;
	return;
}


bool cComMoveTo::isDone()
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

void cComMoveTo::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComMoveTo::getMyGO()
{
	return this->theGO;
}
