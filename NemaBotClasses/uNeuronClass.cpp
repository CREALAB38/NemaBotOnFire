//---------------------------------------------------------------------------

#pragma hdrstop

#include "uNeuronClass.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#include <sstream>

// The Threshold is the maximum accumulated value that must be exceeded before the Neurite will fire
TNeuronValue Threshold;

void TSynapses::Fire(){
   for (TNeuronIndex i = 0; i < getCount(); ++i) {
		pNeurons[i]->AddValue( pWeights[i] );
	}
}

 std::string TSynapses::ToStr(){
	// Format the string using std::stringstream
	std::stringstream ss;
	std::string aString;
	ss << "Synapses: ";

	for (TNeuronIndex i = 0; i < getCount(); ++i) {
		if (i>0) { ss << ", " ; }
		// Format the string using std::stringstream
		ss << pNeurons[i]->Name << " x " << pWeights[i] ;
	}
	return ss.str();
}

 void TSynapses::Report(TStringVect aReport ){
	// Format the string using std::stringstream
	std::stringstream ss;

	for (TNeuronIndex i = 0; i < getCount(); ++i) {
		// Format the string using std::stringstream
		ss << "Synapse " << i << ": Neuron " << pNeurons[i]->Name << " x Weight " << pWeights[i] ;
		// Append the formatted string to the vector
		aReport.push_back(ss.str());
	}
}

void TNeuron::Report(TStringVect aReport, bool withSynapses) {
	// Concatenate strings and integers
	std::string concatenatedString = "Neuron " + pName+ ": Accu= " + std::to_string(pAccumulated) +
			 ", OutVal= " + std::to_string(pOutVal) + ";";
	// Printout name, values and all Synapses
	aReport.push_back( concatenatedString );
	if (withSynapses) { pSynapses.Report(aReport); }
}

void TNeuronen::PropagateAll() {// Propagate all owned Neurons through their Synapses
	for (TNeuron* aNeuron : pNeurons) {
		// TODO if aNeuron not in muscles &&  aNeuron != "MVULVA":
		aNeuron->PropagateSynapses();
	}
}

void TNeuronen::UpdateAll(){ // Update all owned Neuron Out Values
	for (TNeuron* aNeuron : pNeurons) {
		aNeuron->UpdateState();
	}
}

int TNeuronen::Sum(bool deleteNextState){ // sum all Accumulator=NextState Values in a sum
	int accum;
	TNeuron* muscle;
	for (TNeuronIndex i = 0; i < getCount(); ++i) {
		muscle = pNeurons[i];
		//if muscle in mLeft:
		   accum += muscle->nextState; // postSynaptic[muscle][nextState]
		   //accumleft = accumleft + postSynaptic[muscle][thisState]
		   //what???  For some reason, thisState weight is always 0.
		   //postSynaptic[muscle][thisState] = 0
		   //print muscle, "Before", postSynaptic[muscle][thisState], accumleft
		   //Both states have to be set to 0 once the muscle is fired, or
		   if (deleteNextState){ muscle->nextState = 0; }  // postSynaptic[muscle][nextState] = 0
		   // print muscle, "After", postSynaptic[muscle][thisState], accumleft
		   // it will keep returning beyond the threshold within one iteration.
		//elif muscle in mRight:
		   // accumright += postSynaptic[muscle][nextState]
		   //accumleft = accumright + postSynaptic[muscle][thisState] //what???
		   //postSynaptic[muscle][thisState] = 0
		   // postSynaptic[muscle][nextState] = 0
	};
	return accum;
};


void TNeuronen::Report(TStringVect aReport) {
	aReport.push_back( "Neurons: " );
	for (TNeuron* aNeuron : pNeurons) {
		aNeuron->Report(aReport, false);
	}
}


