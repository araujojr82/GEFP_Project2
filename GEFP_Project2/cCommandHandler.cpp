#include "cCommandHandler.h"
#include "cLuaBrain.h"

extern std::vector< cGameObject* > g_vecGameObjects;
extern cCommandScheduler g_theScheduler;

//static 
int cCommandHandler::m_nextCommandID = 1;

cCommandHandler::cCommandHandler()
{
}


cCommandHandler::~cCommandHandler()
{
}

// Search by ID, then returns pointer to object
cGameObject* findObjectByID( unsigned int ID )
{
	for( int index = 0; index != g_vecGameObjects.size(); index++ )
	{
		if( ::g_vecGameObjects[index]->getID() == ID )
		{	// Found it! 
			return ::g_vecGameObjects[index];
		}
	}
	// Didn't find it
	return nullptr;
}

int cCommandHandler::newGroup( lua_State *L )
{
	// Getting the values from the Lua Script
	std::string groupName = lua_tostring( L, 1 );	// The name of the group

	cCommandGroup* theGroup = new cCommandGroup( groupName );
	::g_theScheduler.commandGroups.push_back( theGroup );

	return 0;
}

int cCommandHandler::newCommand( lua_State *L )
{	
	// Getting the values from the Lua Script
	std::string groupName = lua_tostring( L, 1 );	// The name of the group
	std::string commandName = lua_tostring( L, 2 ); // The name of the command
	int theGOID = lua_tonumber( L, 3 );				// (Commanded) Game Object's ID
	float x1 = lua_tonumber( L, 4 );				// The first parameter of a command
	float y1 = lua_tonumber( L, 5 );				// is a vec3 
	float z1 = lua_tonumber( L, 6 );				// end of first parameter
	float secondParameter = lua_tonumber( L, 7 );	// The second is a float ( usually time )
	float x2 = lua_tonumber( L, 8 );				// The third parameter 
	float y2 = lua_tonumber( L, 9 );				// is another vec3
	float z2 = lua_tonumber( L, 10 );				// end of third parameter	
	int targetGOID = lua_tonumber( L, 11 );			// (Target) Game Object's ID
	std::string subCommands = lua_tostring( L, 12 );// The sub-commands string

	// Find the Command Group by name
	cCommandGroup* theGroup = ::g_theScheduler.findGroupByName( groupName );

	// Find the Game Object by ID
	cGameObject* theObject = findObjectByID( theGOID );

	// Find target GO if there's one requested
	cGameObject* targetObject;
	if( targetGOID != 0 )
		targetObject = findObjectByID( targetGOID );
	else
		targetObject = nullptr;
	
	// Create the new command using the commandName
	if( commandName == "FollowCurve" )
	{
		cComFollowCurve* theCommand = new cComFollowCurve();
		
		theCommand->setMyID( cCommandHandler::m_nextCommandID );		
		cCommandHandler::m_nextCommandID++;

		theCommand->setMyGO( theObject );

		glm::vec3 targetPosition = glm::vec3( x1, y1, z1 );
		glm::vec3 curvePosition = glm::vec3( x2, y2, z2 );

		theCommand->init( targetPosition, secondParameter, curvePosition );

		theGroup->theCommands.push_back( theCommand );
	}
	else if( commandName == "FollowObject" )
	{
		cComFollowObject* theCommand = new cComFollowObject();

		theCommand->setMyID( cCommandHandler::m_nextCommandID );
		cCommandHandler::m_nextCommandID++;

		theCommand->setMyGO( theObject );
		theCommand->setTargetGO( targetObject );

		glm::vec3 followParam = glm::vec3( x1, y1, z1 );
		glm::vec3 relativeToTarget = glm::vec3( x2, y2, z2 );

		theCommand->init( followParam, secondParameter, relativeToTarget );
		theCommand->setTargetGO( targetObject );

		theGroup->theCommands.push_back( theCommand );
	}
	else if( commandName == "MoveTo" )
	{
		cComMoveTo* theCommand = new cComMoveTo();

		theCommand->setMyID( cCommandHandler::m_nextCommandID );
		cCommandHandler::m_nextCommandID++;

		theCommand->setMyGO( theObject );

		glm::vec3 targetPosition = glm::vec3( x1, y1, z1 );

		glm::vec3 smoothStepParam = glm::vec3( x2, y2, 0 );

		theCommand->init( targetPosition, secondParameter, smoothStepParam );

		theCommand->setTargetGO( targetObject );

		theGroup->theCommands.push_back( theCommand );
	}
	else if( commandName == "OrientTo" )
	{
		cComOrientTo* theCommand = new cComOrientTo();

		theCommand->setMyID( cCommandHandler::m_nextCommandID );
		cCommandHandler::m_nextCommandID++;

		theCommand->setMyGO( theObject );
		theCommand->setTargetGO( targetObject );

		//glm::vec3 degreesToRotate = glm::vec3( x1, y1, z1 );

		theCommand->init( glm::vec3( 0.0f ), secondParameter, glm::vec3( 0.0f ) );
		theGroup->theCommands.push_back( theCommand );
	}
	else if( commandName == "Rotate" )
	{
		cComRotate* theCommand = new cComRotate();

		theCommand->setMyID( cCommandHandler::m_nextCommandID );
		cCommandHandler::m_nextCommandID++;

		theCommand->setMyGO( theObject );

		glm::vec3 degreesToRotate = glm::vec3( x1, y1, z1 );

		theCommand->init( degreesToRotate, secondParameter, glm::vec3( 0.0f ) );
		theGroup->theCommands.push_back( theCommand );
	}
	else if( commandName == "Trigger" )
	{
		cComTrigger* theCommand = new cComTrigger();

		theCommand->setMyID( cCommandHandler::m_nextCommandID );
		cCommandHandler::m_nextCommandID++;

		theCommand->setMyGO( theObject );

		glm::vec3 triggerPosition = glm::vec3( x1, y1, z1 );

		theCommand->init( triggerPosition, secondParameter, glm::vec3( 0.0f ) );
		theCommand->createSubGroup( subCommands );

		theGroup->theCommands.push_back( theCommand );
	}
	else	// Command isn't mapped yet
	{		// Do nothing

	}


	return 0;
}