// uNeuronClass.h
//---------------------------------------------------------------------------

#ifndef uNeuronClassH
#define uNeuronClassH
//---------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>

using TNeuronValue = float;
using TNeuronIndex = std::int16_t;

extern TNeuronValue Threshold;

class TNeuron; // Forward Declaration of an array of Synapses (Neuron-Weight Pairs)
// class TNeuronen; // Forward decclaration of the owner class TNeuronen

/* Base Class TNeuon

Size of class TNeuron is reported as 44 bytes
    each std::vector object takes 12 bytes; the actual content is stored on the heap and not counted to the size of TNeuron
    each call of AddSynapse change nothing to the sizeof(TNeuron),
    float and int each are 4 bytes, to save memory we could use here int16_t to cut that in half
    to get the actual memory taken for storing all content of a vector use this: sizeof(std::int16_t) * pWeights.size()
        printf( "Size of MDL01: %d bytes.\n", sizeof(MDL01) );
        printf( "Size of MDL01.pNeurons: %d bytes.\n", sizeof(MDL01.pNeurons) + sizeof(TNeuron*) * MDL01.pNeurons.size() );
        printf( "Size of MDL01.pWeights: %d bytes.\n", sizeof(MDL01.pWeights) + sizeof(std::int16_t) * MDL01.pWeights.size());
*/

using  TNeuronArray = std::vector<TNeuron*>;
using  TStringVect  = std::vector<std::string>;
// must use TNeuron* here otherwise the entire class is put into the vector!

/*TNeuronVect encapsulate handling a vector of Neurons, not yet much to done here */
class TNeuronVect {
protected:
	TNeuronArray pNeurons;

	// Private function to get the count of pNeurons
	TNeuronIndex getCount() const { return pNeurons.size();}

public:
	// Constructor to initialize the TNeurons object, currently not doing anything
	TNeuronVect(): pNeurons() {}

	// Constructor to initialize the array with given values
	TNeuronVect(const TNeuronArray& values) : pNeurons(values) {}

	 // Constructor to initialize the array with an initializer list
	TNeuronVect(std::initializer_list<TNeuron*> values) : pNeurons(values) {}

	void Add(TNeuron* aNeuron) { pNeurons.push_back(aNeuron); }

   // Overloading the + operator to append the contents of two arrays
	TNeuronVect operator+(const TNeuronVect& other) const {
		TNeuronVect result = *this; // Create a copy of the current object
		result.pNeurons.insert(result.pNeurons.end(), other.pNeurons.begin(), other.pNeurons.end()); // Append the other array
		return result;
	}

	// Getter function to access the a single TNeuron
	TNeuron* get(TNeuronIndex idx ) { return pNeurons[idx]; }

	// Overloading [] operator for array-like access
	TNeuron* operator[](int index) {
		if (index >= 0 && index < pNeurons.size() ) {
			return pNeurons[index];
		} else {
			// Handle out-of-bounds access here, maybe throw an exception
            throw std::out_of_range("Index out of range");
        }
	}

	// Getter function to access the vector of TNeuron pointers
	const TNeuronArray& getNeurons() const { return pNeurons; }

	// Property Count to get the count of pNeurons
	__property TNeuronIndex Count = {  read  = getCount };
};

/* TSynapses inherits a vector of Neurons from TNeuronVect and adds a vector of
   weights together. It also makes sure that the two vectors are served simultaneously.
*/
class TSynapses : public TNeuronVect {
private:
	std::vector<TNeuronValue> pWeights;

    // Private function to get the count of synapse weights
	int getCount() const {
		return std::min( pWeights.size(), pNeurons.size()) ;
	}

	// Function to add a synapse weight to the vector
	void addWeight(TNeuronValue weight) { pWeights.push_back(weight);  }

	// Getter function to access the vector of synapse weights
	const std::vector<TNeuronValue>& getWeights() const {return pWeights; }

public:
	// Constructor to initialize the TSynapses object, nothing really to report here
	TSynapses() {}

	// Property Count to get the count of synapse weights
	__property TNeuronIndex Count = { read  = getCount };

	// Public Function to add a synapse weight to the vector
	void Add(TNeuron* neuron, TNeuronValue weight) {
		addWeight(weight);
		TNeuronVect::Add(neuron);
	}

	void Fire();

	void Report(TStringVect aReport); // Report all Synapses in a multi line Report

	std::string ToStr(); // Repoart all Synapses in a concise string
};

enum TCategory {
	cat_GENERIC,
    cat_PHARYNX,
    cat_SENSORY_NEURONS,
    cat_INTERNEURONS,
    cat_MOTOR_NEURONS,
    cat_BODYWALL_MUSCLES,
    cat_OTHER_END_ORGANS,
    cat_SEXSPECIFIC_CELLS
};

enum TSubCategory {
	sub_Generic,
	sub__7,
	sub__6,
	sub__5,
    sub__4,
    sub__3,
    sub__2,
    sub__1,
	sub_HEAD_MOTOR_NEURONS,
    sub_SUBLATERAL_MOTOR_NEURONS,
    sub_BODY_MOTOR_NEURONS
};

class TNeuron {
private:
	//TNeuronen* pOwner;
	std::string	 pName;
	TCategory	 pCat;
	TSubCategory pSubCat;
	TSynapses	 pSynapses;
	TNeuronValue pAccumulated; // during fireNeurons the Accumulate accumulates the triggers
							  // equivalent to nextState element in original python code
	TNeuronValue pOutVal;     // When all fireNeurons are done, the Accumulator is transferred to OutVal
							  // the decision that a Neuron triggered is done based on OutVal
							  // equivalent to thisState element in original python code
	TSynapses* getSynapsesPnt() { return   &pSynapses; }

public:
	// Constructor to set properties during object creation
	TNeuron(const std::string& aName) :
		pName(aName), pCat(cat_GENERIC), pSubCat(sub_Generic), pOutVal(0), pAccumulated(0) {}
	TNeuron(const std::string& aName, const TCategory aCat, const TSubCategory aSubCat) :
		pName(aName), pCat(aCat), pSubCat(aSubCat), pOutVal(0), pAccumulated(0) {}

	__property std::string  Name  = { read = pName  };
	__property TCategory    Cat   = { read = pCat   };
	__property TSubCategory SubCat= { read = pSubCat};
	__property int Accu = { read = pAccumulated };
	__property int OutVal = { read = pOutVal };
	__property int nextState = { read = pAccumulated, write = pAccumulated };
	__property int thisState = { read = pOutVal };
	// Getter function to access the synapses
	__property TSynapses* Synapses = { read = getSynapsesPnt };

	void AddSynapse(TNeuron* neuron, TNeuronValue weight) {
		pSynapses.Add(neuron, weight);
	}

	// Add AValue to the existing value:
	void AddValue(TNeuronValue AValue) { pAccumulated += AValue; }

	// If the threshold has been exceeded then we fire the neurite
	void PropagateSynapses() {
		if ( pOutVal>Threshold ) { fireNeuron(); }
	}

	// Fire synapses unconditionally
	void fireNeuron(){
		pSynapses.Fire();
		pAccumulated = 0;
		pOutVal = 0;
	}

	// After firing all neurites accumulating into pAccumulated we update the OutVal
	void UpdateState() { pOutVal = pAccumulated; }// pAccumulated = 0;

	void  Report(TStringVect aReport, bool withSynapses = true );
};


// Array of Neurons
class TNeuronen: public TNeuronVect {
	std::string pName;

public:
	// Constructor to initialize only the name
	TNeuronen(const std::string& name): pName(name) {};

	// Constructor to initialize the array with given values
	TNeuronen(const std::string& name, std::initializer_list<TNeuron*> values):
		TNeuronVect(values), pName(name) {}

    // Overloading the + operator to append the contents of two arrays
	TNeuronen operator+(const TNeuronen& other) const {
		TNeuronen result = *this; // Create a copy of the current object
		result.pNeurons.insert(result.pNeurons.end(), other.pNeurons.begin(), other.pNeurons.end()); // Append the other array
		result.pName = result.pName + "+" + other.pName;
		return result;
	}

	__property std::string Name = { read = pName, write = pName };

	void AddNeuronen( const TNeuronen& other ) {
		pNeurons.insert(pNeurons.end(), other.pNeurons.begin(), other.pNeurons.end());
	}

	void AddNeuronen( const std::initializer_list<TNeuron*> values ) {
		TNeuronArray tempArray(values);
		pNeurons.insert(pNeurons.end(), tempArray.begin(), tempArray.end());
	}

	void PropagateAll(); // Propagate all owned Neurons through their Synapses

	void UpdateAll(); // Update all owned Neuron Out Values

	int Sum(bool deleteNextState = false); // sum all Accumulator values in a sum, and optionally delete them

	// Function to display the contents of the array
	void  Report(TStringVect aReport );
};

#endif // TNEURONCLASS_H

