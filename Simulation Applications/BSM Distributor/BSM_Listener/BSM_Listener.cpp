#pragma once
#pragma comment(lib,"Ws2_32.lib")

#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include<vector>
#include <math.h>
#include<list>
#include <ctime>

#include "geoCoord.h"
#include "BasicVehicle.h"
#include "ListHandle.h"
#include "LinkedList.h"
#include "ConnVeh.h"


#include <random>

using namespace std;

#ifndef BSM_MSG_SIZE
    #define BSM_MSG_SIZE  (45)
#endif

#define MAX_INTERSECTION_NO 100

geoCoord geoPoint;

char Intersectionconfig[256] = "IntersectionConfig_CA_Network.txt";
char BSM_Data_File[256]= "BSM_out.txt";

char BSMInfoFile[256];
FILE* fp_BSM;

char temp_log[256];

double ecef_x,ecef_y,ecef_z;					
//Important!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Note the rectangular coordinate system: local_x is N(+) S(-) side, and local_y is E(+) W(-) side 
//This is relative distance to the ref point!!!!
//local z can be considered as 0 because no elevation change.
double GPS_long,GPS_lat,GPS_ele;
double local_x,local_y,local_z;

int port_list[MAX_INTERSECTION_NO];
double ref_point[MAX_INTERSECTION_NO][2];
double radius[MAX_INTERSECTION_NO];


//GPS Error Model Parameters based on David Kelly Stationary GPS Data

//This set of parameter is from David Kelley's GPS data
//double ar1_lat=0.671663581;
//double ar2_lat=0.227134006;
//double ma1_lat=-0.692119746;
//double ma2_lat=-0.293535913;
//double sd_lat=8.2793108/10000000;
//
//double ar1_long=-0.031290509;
//double ar2_long=0.923609759;
//double ma1_long=1.026319890;
//double ma2_long=0.043891835;
//double sd_long=1.0938612/1000000;
//
//double drift_lat=0.000006365;
//double drift_long=0.00001119;

//This set of parameter is from our GPS data 
//double ar1_lat=1.867200780;
//double ar2_lat=-0.869312633;
//double ma1_lat=-0.148837748;
//double ma2_lat=0.090026483;
//double sd_lat=1.85176412/10000000;

//double drift_lat=-0.00000681;





//normal random number generator
//default_random_engine generator_lat;
//default_random_engine generator_long;
//normal_distribution<double> distribution_lat(0,sd_lat);
//normal_distribution<double> distribution_long(0,sd_long);

LinkedList <ConnVeh> Vehlist;

FILE *fp;

#define PI 3.14159265

int  outputlog(char *filename, char *output);

int main()
{
    BasicVehicle vehIn;

	int nread;
    int i;

	double ref_lat;
	double ref_long;
	double ref_ele;

	//read the Intersection configuration file
	string lineread;
	fstream f;
	f.open(Intersectionconfig,fstream::in);

	int inter_no;  //number of intersections of this segment

	
	getline(f,lineread);
	sscanf(lineread.c_str(),"%lf %lf %lf",&ref_lat,&ref_long,&ref_ele);

	getline(f,lineread);
	sscanf(lineread.c_str(),"%d",&inter_no);


	for (i=0;i<inter_no;i++)
	{
		getline(f,lineread);
		sscanf(lineread.c_str(),"%d %lf %lf %lf",&port_list[i],&ref_point[i][0],&ref_point[i][1],&radius[i]);
	}
	f.close();

	//initialize the geo_point, this should be the GPS coordinates of 0,0 point in VISSIM
	geoPoint.init(ref_long, ref_lat, ref_ele);
	
	//geoPoint.lla2ecef(-122.289130,37.530439,10,&ecef_x,&ecef_y,&ecef_z);

	//geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);


	//Start network connection
	WSAData swaData;
	WORD DllVersion = MAKEWORD(2,2);
	int startup_RetVal = WSAStartup(DllVersion,&swaData);

	SOCKET sSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	SOCKADDR_IN addr;
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_addr.s_addr=INADDR_ANY;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(30000);

	
	bind(sSocket,(SOCKADDR*)&addr,sizeof(addr));

	//End of network connection

	char BSM_buf[BSM_MSG_SIZE]={0};  //buffer to store the BSM
	char Blobout[BSM_MSG_SIZE]={0};  //buffer to store the updated BSM with GPS error

	//recvfrom(sSocket,MAP_buf,5000,NULL,NULL,NULL);
	//cout<<(int)MAP_buf[0]<<endl;
    
	int timeCnt=0;

	while(true)
	{	
		nread=recvfrom(sSocket,BSM_buf,45,NULL,NULL,NULL);

		vehIn.BSM2Vehicle(BSM_buf);  //change from BSM to Vehicle information

		int found=0;
	//================================================== THe following part add GPS errors
	/*
		//Update state variable and error term or add new vehicle
		Vehlist.Reset();
		while(!Vehlist.EndOfList())
		{
			if (Vehlist.Data().TempID==vehIn.TemporaryID)  //matched ID
			{
				if(Vehlist.Data().nFrame==1)   //this is the second item
				{
					//Still don't fit the model since we need to start from the third item becuase it is (2,x,2) model
					Vehlist.Data().Lat_err[Vehlist.Data().nFrame]=distribution_lat(generator_lat);
					Vehlist.Data().Long_err[Vehlist.Data().nFrame]=distribution_long(generator_long);
					Vehlist.Data().Lat_pos[Vehlist.Data().nFrame]=0;
					Vehlist.Data().Long_pos[Vehlist.Data().nFrame]=0;
					Vehlist.Data().Lat_pos_diff[Vehlist.Data().nFrame]=0;
				}
				if(Vehlist.Data().nFrame>1)  //from the third term
				{
					int pos=Vehlist.Data().nFrame;
					//generate error term
					Vehlist.Data().Lat_err[pos]=distribution_lat(generator_lat);
					Vehlist.Data().Long_err[pos]=distribution_long(generator_long);

					//Generate GPS error for Latitude
					//Calculate the time series after 1st order difference
					Vehlist.Data().Lat_pos_diff[pos]=ar1_lat*Vehlist.Data().Lat_pos_diff[pos-1]+ar2_lat*Vehlist.Data().Lat_pos_diff[pos-2]+Vehlist.Data().Lat_err[pos]+ma1_lat*Vehlist.Data().Lat_err[pos-1]+ma2_lat*Vehlist.Data().Lat_err[pos-2];
					//Calculate the original time series
					Vehlist.Data().Lat_pos[pos]=Vehlist.Data().Lat_pos[pos-1]+Vehlist.Data().Lat_pos_diff[pos];

					//Generate GPS error for Longitude
					Vehlist.Data().Long_pos[pos]=ar1_long*Vehlist.Data().Long_pos[pos-1]+ar2_long*Vehlist.Data().Long_pos[pos-2]+Vehlist.Data().Long_err[pos]+ma1_long*Vehlist.Data().Long_err[pos-1]+ma2_long*Vehlist.Data().Long_err[pos-2];


					//Add GPS error to the current trajectory point
					//vehIn.pos.latitude+=Vehlist.Data().Lat_pos[pos]+drift_lat;
					//vehIn.pos.longitude+=Vehlist.Data().Long_pos[pos]+drift_long;


					//cout.precision(10);
					//cout<<"Vehicle ID is: "<<vehIn.TemporaryID<<" "<<"Veh Heading is: "<<vehIn.heading<<endl;

				}


				Vehlist.Data().nFrame++;
				found=1;
				break;
			}
			Vehlist.Next();
		}
		if(found==0) //This is a new vehicle
		{
			ConnVeh TempVeh;
			TempVeh.TempID=vehIn.TemporaryID;
			TempVeh.Lat_pos[0]=0;
			TempVeh.Long_pos[0]=0;
			TempVeh.Lat_err[0]=distribution_lat(generator_lat);
			TempVeh.Long_err[0]=distribution_long(generator_long);
			TempVeh.Lat_pos_diff[0]=0;
			TempVeh.nFrame=1;
			Vehlist.InsertRear(TempVeh);
		}


		//Re-pack BSM with GPS error



========================================================*/

		vehIn.Vehicle2BSM(Blobout);

		
		//Write BSM into file
	   // sprintf(BSMInfoFile,"./map/BSM_OUT_%05i.ber",timeCnt);
		//timeCnt++;

		//fp_BSM=fopen(BSMInfoFile,"wb");	

		//fwrite(Blobout,45,1,fp_BSM);
		//fclose(fp_BSM);


		



		geoPoint.lla2ecef(vehIn.pos.longitude,vehIn.pos.latitude,vehIn.pos.elevation,&ecef_x,&ecef_y,&ecef_z);

		//geoPoint.lla2ecef(-122.305681,37.544500,10,&ecef_x,&ecef_y,&ecef_z);

		geoPoint.ecef2local(ecef_x,ecef_y,ecef_z,&local_x,&local_y,&local_z);


		//fstream fp;
		//fp.open(BSM_Data_File,fstream::out|fstream::app);
		
		




		sprintf(temp_log,"Get BSM from vehicle %d at %f %f\n", vehIn.TemporaryID,local_y,local_x);   //X is north offset and Y is east offset
		cout<<temp_log;
		//distribute the BSM to the corresponding RSE
		for (i=0;i<inter_no;i++)
		{
			if(fabs(local_y-ref_point[i][0])<radius[i] && fabs(local_x-ref_point[i][1])<radius[i])
			{
				//send the BSM to intersection i
				SOCKADDR_IN addr_out;
				if (port_list[i]==501)
					{
						addr_out.sin_addr.s_addr = inet_addr("10.254.56.100");  //Send to RSE 27 Gavilan
						//sprintf(temp_log,"%d %2.8f %2.8f %d 501\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
						//fp<<temp_log;
					}	
				if (port_list[i]==502)
				{
					addr_out.sin_addr.s_addr = inet_addr("10.254.56.101");  //Send to RSE 18 Dedication
					//sprintf(temp_log,"%d %2.8f %2.8f %d 502\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
					//fp<<temp_log;
				}
				if (port_list[i]==503)
				{
					addr_out.sin_addr.s_addr = inet_addr("10.254.56.102");  //Send to RSE 21 Meridian
					//sprintf(temp_log,"%d %2.8f %2.8f %d 503\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
					//fp<<temp_log;
				}
				if (port_list[i]==504)
				{
					addr_out.sin_addr.s_addr = inet_addr("10.254.56.103");  //Send to RSE 25  Hastings
					//sprintf(temp_log,"%d %2.8f %2.8f %d 504\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
					//fp<<temp_log;
				}
				if (port_list[i]==505)
				{
					addr_out.sin_addr.s_addr = inet_addr("10.254.56.104");  //Send to RSE 29  Memorial
					//sprintf(temp_log,"%d %2.8f %2.8f %d 505\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
					//fp<<temp_log;
				}
				if (port_list[i]==506)
				{
					addr_out.sin_addr.s_addr = inet_addr("10.254.56.105");  //Send to RSE 30  Anthem
					//sprintf(temp_log,"%d %2.8f %2.8f %d 506\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
					//fp<<temp_log;
				}
				if (port_list[i]==507)
				{
					addr_out.sin_addr.s_addr = inet_addr("10.254.56.106");  //Send to RSE 30  Anthem
					//sprintf(temp_log,"%d %2.8f %2.8f %d 507\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
					//fp<<temp_log;
				}
				if (port_list[i]==508)
				{
					addr_out.sin_addr.s_addr = inet_addr("10.254.56.107");  //Send to RSE 30  Anthem
					//sprintf(temp_log,"%d %2.8f %2.8f %d 508\n",vehIn.TemporaryID, local_y, local_x, time(NULL));
					//fp<<temp_log;
				}
				addr_out.sin_family=AF_INET;
				addr_out.sin_port=htons(20000);
				bind(sSocket,(SOCKADDR*)&addr_out,sizeof(addr_out));
				int send_RetVal=sendto(sSocket,Blobout,BSM_MSG_SIZE,NULL,(SOCKADDR*)&addr_out,sizeof(addr_out));
				if (send_RetVal== SOCKET_ERROR)
				{
					cout <<"Error"<<endl;
					getchar();
				}
				sprintf(temp_log,"Send to RSE %d\n",port_list[i]);
				cout<<temp_log;
			}

		}
	
	//fp.close();		

	}  //end of While(true)
	return 0;

}

int outputlog(char *filename, char *output)
{
	FILE * stream = fopen(filename, "r" );

	if (stream==NULL)
	{
		perror ("Error opening file");
	}

	fseek( stream, 0L, SEEK_END );
	long endPos = ftell( stream );
	fclose( stream );

	fstream fs;
	if (endPos <10000000)
		fs.open(filename, ios::out | ios::app);
	else
		fs.open(filename, ios::out | ios::trunc);

	//fstream fs;
	//fs.open("/nojournal/bin/OBU_logfile.txt", ios::out | ios::app);
	if (!fs || !fs.good())
	{
		cout << "could not open file!\n";
		return -1;
	}
	fs << output;

	if (fs.fail())
	{
		cout << "failed to append to file!\n";
		return -1;
	}
	fs.close();

	return 1;
}

