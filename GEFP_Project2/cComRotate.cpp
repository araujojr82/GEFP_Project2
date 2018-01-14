#include "cComRotate.h"

#include <iostream>
#include <GLFW/glfw3.h>

cComRotate::cComRotate()
{
	velocityX = 0.0f;
	velocityY = 0.0f;
	velocityZ = 0.0f;
	initialTime = 0;
	duration = 0.0f;
	finalOrientation = glm::vec3( 0.0f, 0.0f, 0.0f );

	return;
}

cComRotate::~cComRotate()
{
	return;
}

void cComRotate::init( glm::vec3 degreesToRotate, float time, glm::vec3 notUsed )
{
	this->duration = time;

	// This is the average velocity it would take to reach the destination
	// within this time
	this->velocityX = degreesToRotate.x / time;
	this->velocityY	= degreesToRotate.y / time;
	this->velocityZ = degreesToRotate.z / time;

	this->finalOrientation.x += glm::radians( degreesToRotate.x );
	this->finalOrientation.y += glm::radians( degreesToRotate.y );
	this->finalOrientation.z += glm::radians( degreesToRotate.z );
	
	return;
}

void cComRotate::update( double deltaTime )
{
	float rotateX = velocityX * deltaTime;
	float rotateY = velocityY * deltaTime;
	float rotateZ = velocityZ * deltaTime;

	glm::vec3 newOrientation = this->theGO->orientation2;

	newOrientation.x += glm::radians( rotateX );
	newOrientation.y += glm::radians( rotateY );
	newOrientation.z += glm::radians( rotateZ );

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

	if( ( rotateZ > 0.0f && newOrientation.z > this->finalOrientation.z ) ||
		( rotateZ < 0.0f && newOrientation.z < this->finalOrientation.z ) )
	{
		this->theGO->orientation2.z = this->finalOrientation.z;
	}
	else
	{
		this->theGO->orientation2.z = newOrientation.z;
	}
	
	return;
}

bool cComRotate::isDone()
{
	// If the GO is on destination, clear the velocity 
	if( this->theGO->orientation2 == finalOrientation )	
	{
		return true;
	}
	else
		return false;
}

void cComRotate::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComRotate::getMyGO()
{
	return this->theGO;
}
