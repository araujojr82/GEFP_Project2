#include "cComOrientTo.h"

#include <iostream>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

cComOrientTo::cComOrientTo()
{
	this->initialTime = 0;

	this->hasStarted = false;
	this->velocityX = 0.0f;
	this->velocityY = 0.0f;
	this->velocityZ = 0.0f;
	this->duration = 0.0f;
	this->theUpVector = glm::vec3( 0.0f );
	this->lookAtOrigin = glm::vec3( 0.0f );
	this->lookAtPosition = glm::vec3( 0.0f );
	this->initialOrientation = glm::mat4x4( 1.0f );
	this->finalOrientation = glm::mat4x4( 1.0f );
	return;
}

cComOrientTo::~cComOrientTo()
{
	return;
}

void cComOrientTo::init( glm::vec3 unused1, float time, glm::vec3 unused )
{
	//this->initPosition = this->theGO->position;
	//this->finalPosition = this->targetGO->position;
	//this->direction = glm::normalize( finalPosition - initPosition );
	//this->distanceToTarget = glm::distance( finalPosition, initPosition );
	this->duration = time;

	// This is the average velocity it would take to reach the destination
	// within this time


	return;
}

void cComOrientTo::update( double deltaTime )
{

	if( this->hasStarted == false )
	{
		this->hasStarted = true;
		this->theUpVector = glm::vec3( 0.0f, 1.0f, 0.0f );

		// LookAt direction at the origin
		this->lookAtOrigin = this->targetGO->position - this->theGO->position;

		// Normalized lookAt at the origin
		this->lookAtOrigin = glm::normalize( this->lookAtOrigin );

		// New LooAt
		this->lookAtPosition = this->theGO->position + this->lookAtOrigin;

		// Care about the orientation too
		this->finalOrientation = glm::inverse( glm::lookAt( glm::vec3( 0.0f, 0.0f, 0.0f ), this->lookAtOrigin, this->theUpVector ) );

		this->initialOrientation = this->theGO->orientation;

		this->initialTime = glfwGetTime();
	}
		
	//glm::mat4x4 orientationStep = this->orientationSpeed * glm::mat4x4( deltaTime );

	glm::quat quatStart = glm::quat_cast( this->initialOrientation );
	glm::quat quatEnd = glm::quat_cast( this->finalOrientation );

	this->elapsedTime = glfwGetTime() - this->initialTime;

	float factor = this->elapsedTime / this->duration;

	//glm::quat quatInterp = glm::slerp( quatStart, quatEnd, factor );
	glm::quat quatInterp = glm::mix( quatStart, quatEnd, factor );

	std::cout 
		<< "Factor: " << factor
		<< " / Elapsed: " << elapsedTime
		<< std::endl;


	this->theGO->orientation = glm::mat4_cast( quatInterp );

	return;
}


bool cComOrientTo::isDone()
{
	if( this->hasStarted == false )
	{
		return false;
	}
	else
	{
		if( this->theGO->orientation == this->finalOrientation
			|| this->elapsedTime >= this->duration )
		{
			return true;
		}
		else
			return false;
	}
}

void cComOrientTo::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComOrientTo::getMyGO()
{
	return this->theGO;
}

void cComOrientTo::setTargetGO( cGameObject* target )
{
	this->targetGO = target;
	return;
}

cGameObject* cComOrientTo::getTargetGO()
{
	return this->targetGO;
}
