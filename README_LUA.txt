The Init Method:
init( glm::vec3 parameter1, float parameter2, glm::vec3 parameter3 ) = 0;

The GetObjectPosition method:
// getObjectPosition(22, x) 
There's 2 parameters for this method, it will get the position of the GO at the INIT time, 
so it wont work if its down in a chain of commands and the target object has moved.
1) Int; The Game Object that this command will control - MANDATORY;
2) String; The Game Object's coordinate to get;

The newCommand method:
// newCommand( "group1", "FollowCurve", 1, 0, 0, -60, 4.0, 20, 0, -40, 0 )
There is 11 parameters that can be passed on this method;
1) String; The Group name that this command will be attached to - MANDATORY;
2) String; The Commands name - Mandatory and must be one of the following: FollowCurve, FollowObject, MoveTo, OrientTo, Rotate, Trigger - MANDATORY;
3) Int; The Game Object that this command will control - MANDATORY;
4, 5, 6) Floats; usually used as coordinates set (xyz);
7) Float, usually used as time/duration;
8, 9, 10) Floats; usually used as a second coordinates set (xyz)
11) Int; A second Game Object used on commands like FollowObject or OrientTo;
12) String: A subscript of commands to be executed; (Used at the Trigger command)

FollowCurve:
4, 5, 6) First set of XYZ is the Destination;
7) Duration of the movement;
8, 9, 10) Second set of XYZ is the curve's control point;
11) Not used;

FollowObject
4) The minimum following distance;
5) The maximum following distance;
6) 
7) Max. Speed
8, 9, 10) Not used
11) The target Game Object's ID

MoveTo
4, 5, 6) First set of XYZ is the Destination;
7) Duration of the movement;
8, 9, 10) Not used;
11) Not used;
11) The target Game Object's ID is optional

OrientTo
4, 5, 6) 
7) Duration of the movement;
8, 9, 10)
11) The target Game Object's ID

Trigger
4, 5, 6) XYZ Position of the Trigger 
7) The radius of the Triggers activation sphere
8, 9, 10)  Not used;
11) Not used;
12) The commands to be executed when in range, can cointain one or more groups.