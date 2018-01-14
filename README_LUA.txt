The Init Method:
init( glm::vec3 parameter1, float parameter2, glm::vec3 parameter3 ) = 0;

The newCommand method:
newCommand( "group1", "FollowCurve", 1, 0, 0, -60, 4.0, 20, 0, -40, 0 )
There is 11 parameters that can be passed on this method;
1) String; The Group name that this command will be attached to - MANDATORY;
2) String; The Commands name - Mandatory and must be one of the following: FollowCurve, FollowObject, MoveTo, OrientTo, Rotate, Trigger - MANDATORY;
3) Int; The Game Object that this command will control - MANDATORY;
4, 5, 6) Floats; usually used as coordinates set (xyz);
7) Float, usually used as time/duration;
8, 9, 10) Floats; usually used as a second coordinates set (xyz)
11) Int; A second Game Object used on commands like FollowObject or OrientTo;

FollowCurve:
4, 5, 6) First set of XYZ is the Destination;
7) The Duration of the movement;
8, 9, 10) Second set of XYZ is the curve's control point;
11) Not used;

FollowObject
4) The minimum following distance;
5) The maximum following distance;
6) 
7) Max. Speed
8, 9, 10) Second set of XYZ is the curve's control point;
11) The target Game Object's ID

MoveTo
4, 5, 6) First set of XYZ is the Destination;
7) The Duration of the movement;
8, 9, 10) Second set of XYZ is the curve's control point;
11) Not used;

OrientTo
4, 5, 6)
7)
8, 9, 10)
11)

Rotate
4, 5, 6)
7)
8, 9, 10)
11)

Trigger
4, 5, 6)
7)
8, 9, 10)
11)