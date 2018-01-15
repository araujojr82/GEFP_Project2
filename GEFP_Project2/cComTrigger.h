#pragma once
#include "iCommand.h"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

class cComTrigger :	public iCommand
{
public:
	cComTrigger();
	~cComTrigger();

	void createSubGroup( std::string theSubScript );

	virtual void init( glm::vec3 theTriggerPoint, float theRadius, glm::vec3 unused );
	virtual void update( double deltaTime );
	virtual bool isDone();
	virtual void setMyGO( cGameObject* myGO );
	virtual cGameObject* getMyGO();
	virtual void setMyID( int myID );

private:
	int m_uniqueID;
	cGameObject* theGO;

	glm::vec3 triggerPoint;	
	float distanceToTarget;
	float radius;
	double initialTime;
	float duration;

	std::string scriptName;
	std::string theScript;

	bool hasStarted;
	bool isCommandDone;
};

