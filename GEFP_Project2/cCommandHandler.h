#ifndef _cCommandHandler_HG_
#define _cCommandHandler_HG_

#include "iCommand.h"
#include "cCommandGroup.h"
#include "cCommandScheduler.h"
#include "cComFollowCurve.h"
#include "cComFollowObject.h"
#include "cComMoveTo.h"
#include "cComOrientTo.h"
#include "cComRotate.h"
#include "cComTrigger.h"

#include <string>

extern "C" {
#include <Lua5.3.3\lualib.h>
#include <Lua5.3.3\lua.h>
#include <Lua5.3.3\lauxlib.h>
}

class cCommandHandler
{
public:
	cCommandHandler();
	~cCommandHandler();
	
	//void newGroup( std::string groupName ); // <- C++ Function
	//
	//void newCommand( std::string groupName,			// The name of the group
	// 				 std::string commandName,		 // The name of the command to be created
	// 				 int theGOID,					 // The Id of the Game Object to be commanded
	// 				 float x1, float y1, float z1,	 // The first parameter of a new command is a vec3
	//				 float time,					 // The second parameter is a float (time)
	//				 float x2, float y2, float z2 ); // the third parameter is another vec3

	static int newGroup( lua_State *L ); // <- C Lua Function

	static int newCommand( lua_State *L ); // <- C Lua Function	

};

#endif
