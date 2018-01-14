#include "cComOrientTo.h"

#include <iostream>
#include <GLFW/glfw3.h>

cComOrientTo::cComOrientTo()
{
	//this->initPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	//this->finalPosition = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->direction = glm::vec3( 0.0f, 0.0f, 0.0f );
	//this->distanceToTarget = 0.0f;
	this->velocity = 0.0f;
	this->initialTime = 0;

	this->hasStarted = false;
	this->velocityX = 0.0f;
	this->velocityY = 0.0f;
	this->velocityZ = 0.0f;
	this->duration = 0.0f;
	this->finalOrientation = glm::vec3( 0.0f, 0.0f, 0.0f );

	return;
}

cComOrientTo::~cComOrientTo()
{
	return;
}

void cComOrientTo::init( glm::vec3 unused1, float time, glm::vec3 unused )
{
	this->initPosition = this->theGO->position;
	this->finalPosition = this->targetGO->position;
	this->direction = glm::normalize( finalPosition - initPosition );
	this->distanceToTarget = glm::distance( finalPosition, initPosition );
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

		glm::vec3 degreesToRotate = glm::vec3( 0.0f );
		glm::vec3 vectorToTarget = glm::vec3( 0.0f );
		glm::vec3 theFrontVector = glm::vec3( 0.0f );

		vectorToTarget = glm::normalize( glm::vec3( 0.0f, 0.0f, this->targetGO->position.z ) -
									     glm::vec3( 0.0f, 0.0f, this->theGO->position.z ) );

		theFrontVector = glm::vec3( 0.0f, 0.0f, -1.0f );

		degreesToRotate.y = glm::degrees( glm::acos( glm::dot( vectorToTarget, theFrontVector ) ) );

		/*vectorToTarget = glm::normalize( glm::vec3( 0.0f, this->targetGO->position.y, 0.0f ) -
									     glm::vec3( 0.0f, this->theGO->position.y, 0.0f ) );*/
		vectorToTarget = glm::normalize( this->targetGO->position - this->theGO->position );

		theFrontVector = glm::vec3( 0.0f, 1.0f, -1.0f );
		degreesToRotate.x = glm::degrees( glm::acos( glm::dot( vectorToTarget, theFrontVector ) ) );

		this->velocityX = degreesToRotate.x / this->duration;
		this->velocityY = degreesToRotate.y / this->duration;
		//this->velocityZ = degreesToRotate.z / time;

		this->finalOrientation.x = this->theGO->orientation2.x + glm::radians( degreesToRotate.x );
		this->finalOrientation.y = this->theGO->orientation2.y + glm::radians( degreesToRotate.y );
		this->finalOrientation.z = this->theGO->orientation2.z;

	}

	float rotateX = velocityX * deltaTime;
	float rotateY = velocityY * deltaTime;
	//float rotateZ = velocityZ * deltaTime;

	glm::vec3 newOrientation = this->theGO->orientation2;

	newOrientation.x += glm::radians( rotateX );
	newOrientation.y += glm::radians( rotateY );
	//newOrientation.z += glm::radians( rotateZ );

	// Check newOrientation can't pass the finalOrientation
	if( ( rotateX > 0.0f && newOrientation.x > this->finalOrientation.x ) ||
		( rotateX < 0.0f && newOrientation.x < this->finalOrientation.x ) )
	{
		this->theGO->orientation2.x = this->finalOrientation.x;
	}
	else
	{
		this->theGO->orientation2.x = newOrientation.x;
	}

	if( ( rotateY > 0.0f && newOrientation.y > this->finalOrientation.y ) ||
		( rotateY < 0.0f && newOrientation.y < this->finalOrientation.y ) )
	{
		this->theGO->orientation2.y = this->finalOrientation.y;
	}
	else
	{
		this->theGO->orientation2.y = newOrientation.y;
	}

	//if( ( rotateZ > 0.0f && newOrientation.z > this->finalOrientation.z ) ||
	//	( rotateZ < 0.0f && newOrientation.z < this->finalOrientation.z ) )
	//{
	//	this->theGO->orientation2.z = this->finalOrientation.z;
	//}
	//else
	//{
	//	this->theGO->orientation2.z = newOrientation.z;
	//}

	return;
}


bool cComOrientTo::isDone()
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
