#pragma once
#include "iCommand.h"

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class cComFollowCurve :	public iCommand
{
public:
	cComFollowCurve();
	~cComFollowCurve();

	virtual void init( glm::vec3 destination, float time, glm::vec3 thePoint );
	virtual void update( double deltaTime );
	virtual bool isDone();
	virtual void setMyGO( cGameObject* myGO );
	virtual cGameObject* getMyGO();

	virtual void setMyID( int myID );

private:
	int m_uniqueID;
	cGameObject* theGO;

	glm::vec3 initPosition;
	glm::vec3 prevPosition;
	glm::vec3 finalPosition;
	glm::vec3 theControlPoint;
	glm::vec3 direction;
	float traveledDistance;
	float distanceToTarget;
	float velocity;
	double initialTime;
	float duration;
	float elapsedTime;
	float distanceCurveLenght;
	
	bool hasStarted;
	bool isCommandDone;
	
};

