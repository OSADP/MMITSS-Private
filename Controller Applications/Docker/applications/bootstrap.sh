#!/bin/bash
## This script is the starting point for all the MMITSS applications inside the Docker Container

run_in_bg () {
	$@ &> /nojournal/bin/log/${1}.log &
	sleep 1s
}

ifconfig > /nojournal/bin/log/my_ip.txt

# Run the applications which are required
#run_in_bg MMITSS_MRP_MAP_SPAT_Broadcast_ASN 127.0.0.1 127.0.0.1 1
  run_in_bg MMITSS_MRP_EquippedVehicleTrajectoryAware_ASN 20000 1 0
  run_in_bg MMITSS_MRP_TrafficControllerInterface
  run_in_bg MMITSS_rsu_Signal_Control_visual 20000 0.5 0
#  run_in_bg MMITSS_MRP_EquippedVehicleTrajectoryAware_ASN_vissimtime 20000 1 0
 #  run_in_bg MMITSS_MRP_TrafficControllerInterface_vissimtime 20000
  # run_in_bg MMITSS_rsu_Signal_Control_visual_coord_TSP 20000 1.0 0 20000
  #  run_in_bg MMITSS_rsu_PerformanceObserver_TT 20000
#run_in_bg mehdi_mprsolver_ack
#run_in_bg MMITSS_MRP_PriorityRequestServer_ackasn
#run_in_bg MMITSS_OBE_MAP_SPAT_Receiver_ASN
#run_in_bg MMITSS_OBE_PriorityRequestGenerator_ackasn -c 1
# run_in_bg NewModel


#Applications to run Integration of Signal Priority (w/w.o. cooridnation) and Adaptive Control
 #  run_in_bg MMITSS_MRP_TrafficControllerInterface_vissimtime 20000     
 #  run_in_bg MMITSS_MRP_EquippedVehicleTrajectoryAware_ASN_vissimtime 20000 1 0
 #  run_in_bg MMITSS_rsu_Signal_Control_visual_coord_TSP 20000 0.5 0 20000

# run_in_bg MMITSS_MRP_MAP_SPAT_Broadcast_ASN 127.0.0.1 127.0.0.1 0
# run_in_bg MMITSS_OBE_MAP_SPAT_Receiver_ASN                     
# run_in_bg MMITSS_MRP_PriorityRequestServer_sim -c 2
# run_in_bg mprSolver -c 2                                        
# run_in_bg MMITSS_MRP_PriorityRequestServer_sim -c 2 -o 1
# run_in_bg MMITSS_OBE_PriorityRequestGenerator_sim -c 1           
###################################################################################
 #Applications to run Integration of Signal Priority with Actuation
# run_in_bg MMITSS_MRP_MAP_SPAT_Broadcast_ASN 127.0.0.1 127.0.0.1 0 
#  run_in_bg MMITSS_OBE_MAP_SPAT_Receiver_ASN
#  run_in_bg mprSolver
#  run_in_bg MMITSS_OBE_PriorityRequestGenerator_sim -c 1
#  run_in_bg MMITSS_MRP_TrafficControllerInterface     
#  run_in_bg MMITSS_MRP_PriorityRequestServer_sim 
#######################################################################

#Applications to run Integration of Signal Priority with Acutation and Coordination
# run_in_bg MMITSS_MRP_MAP_SPAT_Broadcast_ASN 127.0.0.1 127.0.0.1 0
# run_in_bg MMITSS_OBE_MAP_SPAT_Receiver_ASN                     
# run_in_bg MMITSS_MRP_PriorityRequestServer_sim 
# run_in_bg mprSolver                                        
# run_in_bg MMITSS_OBE_PriorityRequestGenerator_sim -c 1           
#   run_in_bg MMITSS_MRP_TrafficControllerInterface_vissimtime 20000     
####################################################################################



# Do not remove this line
while true; do sleep 1h; done
