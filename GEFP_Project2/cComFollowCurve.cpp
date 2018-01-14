#include "cComFollowCurve.h"

#include <iostream>
#include <GLFW/glfw3.h>

extern cDebugRenderer* g_pDebugRenderer;

cComFollowCurve::cComFollowCurve()
{
	this->initPosition = glm::vec3( 0.0f );
	this->prevPosition = glm::vec3( 0.0f );
	this->finalPosition = glm::vec3( 0.0f );
	this->direction = glm::vec3( 0.0f );
	this->traveledDistance = 0.0f;
	this->distanceToTarget = 0.0f;
	this->velocity = 0.0f;
	this->initialTime = 0;
	this->elapsedTime = 0;

	this->hasStarted = false;

	return;
}

cComFollowCurve::~cComFollowCurve()
{
	return;
}


// Quadratic Bezier calculation based on
// https://stackoverflow.com/questions/785097/how-do-i-implement-a-b%C3%A9zier-curve-in-c
float getPoint( float n1, float n2, float perc )
{
	float diff = n2 - n1;

	return n1 + ( diff * perc );
}

float curveLength( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2 )
{
// Based on java Quadractic Bezier Lenght Calculation from:
// https://www.gamedev.net/forums/topic/551455-length-of-a-generalized-quadratic-bezier-curve-in-3d/

	std::vector< glm::vec3 > initialPoints;
	initialPoints.push_back( p0 );
	initialPoints.push_back( p1 );
	initialPoints.push_back( p2 );

	std::vector< glm::vec3 > segments;
	
	glm::vec3 segment1 = initialPoints[1] - initialPoints[0];
	segments.push_back( segment1 );
	
	glm::vec3 segment2 = initialPoints[0] - 2.0f * initialPoints[1] + initialPoints[2];
	segments.push_back( segment2 );

	float length;

	if( segments[1] != glm::vec3( 0.0f ) )
	{
		// Coefficients of f(t) = c*t^2 + b*t + a.
		
		float c = 4.0f * glm::dot( segments[1], segments[1] );  // c > 0 to be in this block of code
		float b = 8.0f * glm::dot( segments[0], segments[1] );
		float a = 4.0f * glm::dot( segments[0], segments[0] );  // a > 0 by assumption
		float q = 4.0f * a * c - b * b;  // = 16*|Cross(A0,A1)| >= 0

		float twoCpB = 2.0f * c + b;
		float sumCBA = c + b + a;
		float mult0 = 0.25f / c;
		float mult1 = q / ( 8.0f * powf( c, 1.5f ) );
		length =
			mult0 * ( twoCpB * sqrtf( sumCBA ) - b*sqrtf( a ) ) +
			mult1 * ( logf( 2.0f * sqrtf( c * sumCBA ) + twoCpB ) - logf( 2.0f * sqrtf( c * a ) + b ) );
	}
	else
	{
		length = 2.0f * glm::length( initialPoints[1] - initialPoints[0] );
	}

	return length;
}

void cComFollowCurve::init( glm::vec3 destination, float time, glm::vec3 thePoint )
{
	this->finalPosition = destination;
	this->direction = glm::normalize( finalPosition - initPosition );
	this->distanceToTarget = glm::distance( finalPosition, initPosition );
	this->duration = time;
	this->theControlPoint = thePoint;

	this->distanceCurveLenght = curveLength( this->initPosition, this->theControlPoint, this->finalPosition );

	// This is the average velocity it would take to reach the destination
	this->velocity = distanceCurveLenght / time;
	
	//std::cout << "Linear dist:" << this->distanceToTarget
	//	<< " / Curve dist: " << this->distanceCurveLenght
	//	//<< " / Curve dist132: " << distance132
	//	<< std::endl;
	
	return;
}

void cComFollowCurve::update( double deltaTime )
{	
	// Start the time 
	if( this->initialTime == 0.0f )
	{
		this->initialTime = glfwGetTime();
	}

	if( this->hasStarted == false )
	{
		this->hasStarted = true;
		this->initPosition = this->theGO->position;
		this->prevPosition = this->initPosition;
	}	

	//float range1 = 0.05 * this->distanceCurveLenght;
	//float range2 = 0.90 * this->distanceCurveLenght;
	float range1 = 0.10 * this->duration;
	float range2 = 0.90 * this->duration;

	//float elapsedTime = glfwGetTime() - this->initialTime;
	this->elapsedTime = glfwGetTime() - this->initialTime;

	float percentual = 0.0f;

	if( this->theGO->position != this->prevPosition )
	{ // Calculate the traveled distance by the worst way 
		this->traveledDistance += glm::distance( this->theGO->position, this->prevPosition );
	}
	
	// Update the previous position
	this->prevPosition = this->theGO->position;

	// The 3 points of the Quadractic Bezier
	// Makes it easier to read the formula
	glm::vec3 point1 = this->initPosition;		
	glm::vec3 point2 = this->theControlPoint;
	glm::vec3 point3 = this->finalPosition;

	//// For the Debug Rendering:
	//if( this->traveledDistance == 0 )
	//{   // Draw lines between p1, p2 and p3 one time only
	//	glm::vec3 color = glm::vec3( 1.0f );

	//	::g_pDebugRenderer->addLine( point1, point2, color, true );
	//	::g_pDebugRenderer->addLine( point3, point2, color, true );
	//}

	percentual = this->elapsedTime / this->duration;
	
	float i = 0;

	//// Just kidding this is hard stuff
	// Lets complicate this stuff with some smoothstep, why not?
	// Using the smooth step 2x to calculate aceleration and deceleration
	//float factor = glm::smoothstep( -0.1f, range1, this->elapsedTime )
	//	* ( 1 - glm::smoothstep( range2, this->duration, this->elapsedTime ) );
	////if( factor < 1.0f )
	////	i = percentual * factor;	// easier to read
	////else
	////	i = percentual;
	//if( factor != 0.0f )
	//	i = glm::pow( percentual, factor );
	//else
	//	i = percentual;

	if( percentual > 1.0f )
		i = 1.0f;
	else
		i = percentual;

	// Calculating the position of the line
	float xa = getPoint( point1.x, point2.x, i );
	float ya = getPoint( point1.y, point2.y, i );
	float za = getPoint( point1.z, point2.z, i );

	float xb = getPoint( point2.x, point3.x, i );
	float yb = getPoint( point2.y, point3.y, i );
	float zb = getPoint( point2.z, point3.z, i );
		
	// Calculate the point over the line position
	float x = getPoint( xa, xb, i );
	float y = getPoint( ya, yb, i );
	float z = getPoint( za, zb, i );

	// Update the Object's position
	this->theGO->position.x = x;
	this->theGO->position.y = y;
	this->theGO->position.z = z;

	//// Draws the line that we got the new point from
	//glm::vec3 color = glm::vec3( 0.0f, 1.0f, 0.0f );
	//::g_pDebugRenderer->addLine( glm::vec3( xa, ya, za ),
	//								glm::vec3( xb, yb, zb ), color, false );

	//// Draws the new point
	//drPoint thePoint;
	//thePoint.xyz = glm::vec3( x, y, z );
	//thePoint.colour = glm::vec3( glm::vec3( 1.0f, 0.0f, 0.0f ) );
	//thePoint.bPersist = true;
	//::g_pDebugRenderer->addPoint( thePoint );

	//float remainingDistance = this->distanceCurveLenght - this->traveledDistance;
	//std::cout
	//	/*<< "Position xyz: " << this->theGO->position.x
	//	<< " , " << this->theGO->position.y << " , " << this->theGO->position.z*/
	//	<< " Elapsed Time: " << this->elapsedTime
	//	<< " - Traveled distancet: " << this->traveledDistance
	//	//<< " - Distance to the target: " << remainingDistance
	//	<< " - Percentual: " << percentual
	//	//<< " - Factor: " << factor
	//	<< " - i: " << i
	//	<< std::endl;

	return;
}

bool cComFollowCurve::isDone()
{
	//if( this->count == 100 )
	//	return true;
	//else
	//	return false;

	// If the GO is on destination, return true;
	if( this->theGO->position == this->finalPosition ||
		this->elapsedTime >= this->duration )
	{		
		return true;
	}
	else
	{
		return false;
	}
}

void cComFollowCurve::setMyGO( cGameObject* myGO )
{
	this->theGO = myGO;
	return;
}

cGameObject* cComFollowCurve::getMyGO()
{
	return this->theGO;
}
