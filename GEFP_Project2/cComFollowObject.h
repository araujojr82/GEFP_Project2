#pragma once
#include "iCommand.h"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class cComFollowObject :	public iCommand
{
public:
	cComFollowObject();
	~cComFollowObject();

	virtual void init( glm::vec3 destination, float time, glm::vec3 notBeingUsed );
	virtual void update( double deltaTime );
	virtual bool isDone();
	virtual void setMyGO( cGameObject* myGO );
	virtual cGameObject* getMyGO();

	void setTargetGO( cGameObject* target );
	cGameObject* getTargetGO();

private:
	cGameObject* theGO;

	glm::vec3 initPosition;
	glm::vec3 finalPosition;
	glm::vec3 direction;
	float distanceToTarget;
	float velocity;
	double initialTime;
	float duration;

	cGameObject* targetGO;

};

