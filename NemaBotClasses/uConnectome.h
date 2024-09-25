// TConnectome.h
//---------------------------------------------------------------------------

#ifndef uConnectomeH
#define uConnectomeH
//---------------------------------------------------------------------------

#include "uNeuronClass.h"
// it defines using TNeuronValue = float;
// and using TNeuronIndex = std::int16_t;


// Define a structure that holds the muscle stimulation of one segment
struct TDVLR {
	TNeuronValue DL;
	TNeuronValue DR;
	TNeuronValue VL;
	TNeuronValue VR;
	float    alpha ; // angle value associated to those
	// Member function to compute the different sums of L, R etc.
	TNeuronValue D()   const { return DL + DR; }
	TNeuronValue V()   const { return VL + VR; }
	TNeuronValue L()   const { return DL + VL; }
	TNeuronValue R()   const { return DR + VR; }
	TNeuronValue sum() const { return D()+ V();}

     // Overload the + operator to add two TDVLR objects
    TDVLR operator+(const TDVLR& other) const {
        TDVLR result;
		result.DL = DL + other.DL;  // Add DL values
		result.DR = DR + other.DR;  // Add DR values
		result.VL = VL + other.VL;  // Add VL values
		result.VR = VR + other.VR;  // Add VR values
		return result;
    }
   // Overload the * operator to multiply a TDVLR object by a scalar float
    TDVLR operator*(float m) const {
        TDVLR result;
		result.DL = DL * m;  // Multiply DL by m
		result.DR = DR * m;  // Multiply DR by m
		result.VL = VL * m;  // Multiply VL by m
		result.VR = VR * m;  // Multiply VR by m
		return result;
    }
    // Overload the * operator to allow scalar * TDVLR (commutative property)
    friend TDVLR operator*(float m, const TDVLR& tdvlr) {
        return tdvlr * m;  // Just reuse the above operator
	}
};

// This is the full C Elegans Connectome as expresed in the form of the Presynatptic neurite and the postSynaptic neurites
 class TConnectome{
public:
	//Declare TNeuronen structure that holds all 397 TNeurons.
	TNeuronen Neurons;

	// all the subsets of muscle Neurons:
	TNeuronen musDleft;
	TNeuronen musVleft;
	TNeuronen musDright;
	TNeuronen musVright;
	TNeuronen musLeft;
	TNeuronen musRight;
	TNeuronen musAll;

	// Constructor of TConnectome
	TConnectome(): // Initialize the connectome instances one by one
		Neurons("Connectome"),
		// Used to accumulate muscle weighted values in body muscles 07-23 = worm locomotion
		// D are dorsal muscles, V are ventral muscles
		musDleft("musDleft"),
		musVleft("musVleft"),
		musDright("musDright"),
		musVright("musVright"),
		musLeft("musLeft"),
		musRight("musRight"),
		musAll("musAll"){};

	// Initialize the connectome instances one by one
	void Init();

	// Declare setupNeurons as virtual method
	virtual void setupNeurons() = 0;

	// Declare setupSynapses as virtual method
	virtual void setupSynapses() = 0;

	// Declare setupMuscles as virtual method
	virtual void setupMuscles() = 0;

	// Declare NoseTouch as virtual method
	virtual void NoseTouch() = 0;

	// Declare Food as virtual method
	virtual void Food() = 0;

	// get activation of the ith muscles, left and right
	// TLR getMusclesLR(TNeuronIndex index);

	// get activation of the ith muscles, Dorsal, Ventral, left and right
	TDVLR getMusclesDVLR(TNeuronIndex index);

	// evaluate Motor Neurons to control a motor
	void motorcontrol();

	// run for one cycle
	void Run();

	// Virtual destructor to ensure proper cleanup of derived class objects
	virtual ~TConnectome() = default;

};

#endif

