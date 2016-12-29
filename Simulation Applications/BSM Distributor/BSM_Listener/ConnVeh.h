//This header file is used to store Latitude and Latitude state and error for ARIMA modeling of the GPS error
class ConnVeh
{
public:
	int TempID; //ID, temp, since it is changing every 5 mins
	int nFrame;  //store how many frames are recorded.
	double Lat_pos[10000];  
	double Long_pos[10000];  //latitude and longitude state variable  
	double Lat_err[10000];
	double Long_err[10000];  //latitude and longitude error term
	double Lat_pos_diff[10000];  //latitude state variable after 1st order difference this is for ARIMA(2,x,2) when x=1
};