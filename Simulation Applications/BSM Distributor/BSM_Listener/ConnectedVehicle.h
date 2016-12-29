#include "PositionLocal3D.h"
#include <vector>

class ConnectedVehicle
{
public:
	int TempID; //ID, temp, since it is changing every 5 mins
	double Speed;  //Vehicle current Speed
	int desiredPhase ; //traffic signal phase associated with a lane (MAP + phase association)
	double stopBarDistance ; //distance to the stop bar from current location (MAP)
	double estArrivalTime ; // estimated arrival time at the stop bar given current speed
	PositionLocal3D traj[5000]; //store vehicle trajectory, totally store 500s trajectory
	int nFrame;  //store how many frames are recorded.
	double heading; //vehicle heading
	double N_Offset[5000];  //offset to the center of the intersection
	double E_Offset[5000];  //in cm
	int Dsecond;


//	-----------------Methods-------------------------

};