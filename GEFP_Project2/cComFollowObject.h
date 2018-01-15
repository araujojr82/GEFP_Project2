#pragma once
#include "iCommand.h"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class cComFollowObject :	public iCommand
{
public:
	cComFollowObject();
	~cComFollowObject();

	virtual void init( glm::vec3 destination, float time, glm::vec3 relativeToTarget );
	virtual void update( double deltaTime );
	virtual bool isDone();
	virtual void setMyGO( cGameObject* myGO );
	virtual cGameObject* getMyGO();

	void setTargetGO( cGameObject* target );
	cGameObject* getTargetGO();
	virtual void setMyID( int myID );

private:
	int m_uniqueID;
	cGameObject* theGO;

	glm::vec3 idealLocationRelToTarget;
	float follow_distance_max_speed;
	float follow_distance_zero_speed;
	float follow_max_speed;	

	glm::vec3 initPosition;
	glm::vec3 finalPosition;
	glm::vec3 idealPosition;
	glm::vec3 direction;
	float distanceToTarget;
	glm::vec3 velocity;
	double initialTime;
	double elapsedTime;
	float duration;

	cGameObject* targetGO;
	bool isCommandDone;
	bool hasStarted;

};

