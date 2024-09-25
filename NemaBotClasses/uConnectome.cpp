// uConnectome.cpp
//---------------------------------------------------------------------------

#pragma hdrstop

#include "uConnectome.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

void TConnectome::Init() // Initialize the connectome instances one by one
{
	Threshold = 30;

	setupNeurons();

	setupMuscles();

	setupSynapses();
}

// get activation of the ith muscles, left and right
/*TLR TConnectome::getMusclesLR(TNeuronIndex index)
{ 	TLR aLR  = {0, 0}; // Initializing L and R to 0
	if ((index>=0) && (index<musDleft.Count)) {
		aLR.L = musDleft[index]->OutVal + musVleft[index]->OutVal;
		aLR.R = musDright[index]->OutVal + musVright[index]->OutVal;
	}
	return aLR;
}
*/

	// get activation of the ith muscles, Dorsal, Ventral, left and right
TDVLR TConnectome::getMusclesDVLR(TNeuronIndex index)
{ 	TDVLR aDVLR = {0, 0, 0, 0}; // Initializing all four to 0
	if ((index>=0) && (index<musDleft.Count)) {
		aDVLR.DL = musDleft [index]->OutVal;
		aDVLR.DR = musDright[index]->OutVal;
		aDVLR.VL = musVleft [index]->OutVal;
		aDVLR.VR = musVright[index]->OutVal;
	}
	return aDVLR;
}


void TConnectome::Run(){ // run for one cycle
	/* Each time a set of neuron is stimulated, this method will execute
	# The weigted values are accumulated in the postSynaptic array
	# Once the accumulation is read, we see what neurons are greater
	# then the threshold and fire the neuron or muscle that has exceeded
	# the threshold */

	motorcontrol(); // evaluate the motor neurons first, then they get set to 0 automatically

	Neurons.PropagateAll(); // Propagate all owned Neurons through their Synapses

	Neurons.UpdateAll();  // Update all owned Neuron Out Values
}


void TConnectome:: motorcontrol(){
	// Accumulators are used to decide the value to send to the Left and Right motors of the GoPiGo robot
	int accumright;
	int accumleft;
	int motSpeed;

	// accumulate left and right muscles and the accumulated values are
	// used to move the left and right motors of the robot
	accumright = musRight.Sum(true);
	accumleft  = musLeft.Sum(true);

	// We turn the wheels according to the motor weight accumulation
	motSpeed = abs(accumleft) + abs(accumright);
	motSpeed = std::max(75,std::min(motSpeed, 150));
	//// Start Commented section
	// set_speed(new_speed)
	// if accumleft == 0 and accumright == 0:
	//         stop()
	// elif accumright <= 0 and accumleft < 0:
	//         set_speed(150)
	//         turnratio = float(accumright) / float(accumleft)
	//         // print "Turn Ratio: ", turnratio
	//         if turnratio <= 0.6:
	//                  left_rot()
	//                  time.sleep(0.8)
	//         elif turnratio >= 2:
	//                  right_rot()
	//                  time.sleep(0.8)
	//         bwd()
	//         time.sleep(0.5)
	// elif accumright <= 0 and accumleft >= 0:
	//         right_rot()
	//         time.sleep(.8)
	// elif accumright >= 0 and accumleft <= 0:
	//         left_rot()
	//         time.sleep(.8)
	// elif accumright >= 0 and accumleft > 0:
	//         turnratio = float(accumright) / float(accumleft)
	//         // print "Turn Ratio: ", turnratio
	//         if turnratio <= 0.6:
	//                  left_rot()
	//                  time.sleep(0.8)
	//         elif turnratio >= 2:
	//                  right_rot()
	//                  time.sleep(0.8)
	//         fwd()
	//         time.sleep(0.5)
	// else:
	//         stop()
	 //// End Commented section
}



