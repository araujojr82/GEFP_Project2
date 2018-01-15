#include "cComFollowObject.h"

#include <iostream>
#include <GLFW/glfw3.h>

void cComFollowObject::setMyID( int myID )
{
	this->m_uniqueID = myID;
	return;
}

cComFollowObject::cComFollowObject()
{
	this->idealLocationRelToTarget = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->follow_distance_max_speed = 0.0f;
	this->follow_distance_zero_speed = 0.0f;
	this->follow_max_speed = 0.0f;
	this->velocity = glm::vec3( 0.0f, 0.0f, 0.0f );

	this->initPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->finalPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->idealPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->direction = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->distanceToTarget = 0.0f;
	this->initialTime = 0;
	this->elapsedTime = 0;

	this->hasStarted = false;
	this->isCommandDone = false;

	return;
}

cComFollowObject::~cComFollowObject()
{
	return;
}

void cComFollowObject::init( glm::vec3 followParameters, float time, glm::vec3 relativeToTarget )
{
	this->idealLocationRelToTarget = relativeToTarget;
	this->follow_distance_zero_speed = followParameters.x; // The minimum following distance;	
	this->follow_distance_max_speed = followParameters.y;  // The maximum following distance;
	this->follow_max_speed = followParameters.z;		   // Max Speed
	this->duration = time;
	
	return;
}

//float clamp( float x, float lowerlimit, float upperlimit ) {
//	if( x < lowerlimit )
//		x = lowerlimit;
//	if( x > upperlimit )
//		x = upperlimit;
//	return x;
//}
//
//float smootherstep( float edge0, float edge1, float x ) {
//	// Scale, and clamp x to 0..1 range
//	x = clamp( ( x - edge0 ) / ( edge1 - edge0 ), 0.0, 1.0 );
//	// Evaluate polynomial
//	return x * x * x * ( x * ( x * 6 - 15 ) + 10 );
//}

void cComFollowObject::update( double deltaTime )
{
	if( !this->hasStarted )
	{
		this->hasStarted = true;
		this->initialTime = glfwGetTime();
	}

	this->elapsedTime = glfwGetTime() - this->initialTime;

	// Update every frame
	this->initPosition = this->theGO->position;
	this->finalPosition = this->targetGO->position;
	//this->direction = glm::normalize( this->finalPosition - this->initPosition );
	//this->distanceToTarget = glm::distance( this->finalPosition, this->initPosition );

	// Calculate ideal location in world
	this->idealPosition	= this->finalPosition + this->idealLocationRelToTarget;

	// How far away from the object are we?
	glm::vec3 vectorToTarget = this->idealPosition - this->theGO->position;
	glm::vec3 vectorToTargetNormal = glm::normalize( vectorToTarget );

	// Pro tip: don't use .lenth();
	float currentDistanceToTarget = glm::length( vectorToTarget );

	// Pass all that into the smooth (or smoother) step 
	// 1st is distance to Zero Speed "circle"
	// 2nd is distance to Max Speed "circle"
	float speedRatio = glm::smoothstep( this->follow_distance_zero_speed,
										this->follow_distance_max_speed,
										currentDistanceToTarget );

	float scaledMaxSpeed = speedRatio * this->follow_max_speed; 

	// Ideal max speed
	glm::vec3 vecMaxSpeed = glm::vec3( scaledMaxSpeed, scaledMaxSpeed, scaledMaxSpeed );

	this->velocity = vecMaxSpeed * vectorToTargetNormal;

	this->theGO->vel = this->velocity;

	// Calculate delta position according to the velocity based on time elapsed
	glm::vec3 deltaPosition = ( float )deltaTime * this->theGO->vel;

	this->theGO->position += deltaPosition;

	//std::cout 
	//	<< "Speed: " << this->theGO->vel.x << ", " << this->theGO->vel.y << ", " << this->theGO->vel.z
	//	<< " | Target Speed: " << this->targetGO->vel.x << ", " << this->targetGO->vel.y << ", " << this->targetGO->vel.z
	//	<< " | Distance: " << currentDistanceToTarget
	//	<< " | Ratio: " << speedRatio
	//	<< std::endl;	

	return;
}


bool cComFollowObject::isDone()
{
	if( this->isCommandDone ) return true;

	if( !this->hasStarted  ) return false;
	else
	{
		if( this->theGO->position == this->idealPosition
		    || this->elapsedTime >= this->duration )
		{
			this->isCommandDone = true;
			return true;
		}
		else
			return false;
	}
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

void cComFollowObject::setTargetGO( cGameObject* target )
{
	this->targetGO = target;
	return;
}

cGameObject* cComFollowObject::getTargetGO()
{
	return this->targetGO;
}
