//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "uNemaBotOnFire.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"


#include <vector>
#include "uNeuronClass.h"
// it defines using TNeuronValue = float;
// and using TNeuronIndex = std::int16_t;



TNemaBotOnFire *NemaBotOnFire;
//---------------------------------------------------------------------------
__fastcall TNemaBotOnFire::TNemaBotOnFire(TComponent* Owner)
	: TForm(Owner)
{   TNeuron* aNeuron;
	// Creator is already called alone?
	Connectome.Init();
	connectomeCount = Connectome.Neurons.Count;
	oldMuscles.resize( connectomeCount );

	for (int i=0; i < connectomeCount; i++) {
		aNeuron = Connectome.Neurons.get(i);
		TLabel *aLabel;
		aLabel = new TLabel(NemaBotOnFire);
		aLabel->Parent = sbNeurons;
		aLabel->Position->X =  lbName->Position->X;
		aLabel->Position->Y = lbName->Position->Y  + i * 28;
		aLabel->Text = STLString2UCString( aNeuron->Name );
		Labels.push_back(aLabel);
		TEdit *ValEditor;
		ValEditor = new TEdit(NemaBotOnFire);
		ValEditor->Parent = sbNeurons;
		ValEditor->Position->X = edValue->Position->X;
		ValEditor->Position->Y  = edValue->Position->Y + i * 28;
		ValEditor->Text = edValue->Text;
		ValEditor->Width = edValue->Width;
		ValEditor->ReadOnly = edValue->ReadOnly;
		ValEditors.push_back(ValEditor);
		TLabel *aSynapses;
		aSynapses = new TLabel(NemaBotOnFire);
		aSynapses->Parent = sbNeurons;
		aSynapses->Position->X =  lbConnectingTo->Position->X;
		aSynapses->Position->Y = lbConnectingTo->Position->Y + i * 28;
		aSynapses->Text = STLString2UCString( aNeuron->Synapses->ToStr() );
		synLabels.push_back(aSynapses);
		}
	lbName->Visible=False;
	edValue->Visible=False;
	lbConnectingTo->Visible=False;
}//---------------------------------------------------------------------------



/* Based on GoPiGo Connectome
That is Written by Timothy Busbice, Gabriel Garrett, Geoffrey Churchill (c) 2014, in Python 2.7
The GoPiGo Connectome uses a postSynaptic dictionary based on the C Elegans Connectome Model
That application could be run on the Raspberry Pi GoPiGo robot with a Sonar that represents Nose Touch when activated
To run standalone without a GoPiGo robot, simply comment out the sections with Start and End comments

#TIME STATE EXPERIMENTAL OPTIMIZATION
The previous version had a logic error whereby if more than one neuron fired into the same neuron in the next time state,
it would overwrite the contribution from the previous neuron. Thus, only one neuron could fire into the same neuron at any given time state.

This version also explicitly lists all left and right muscles, so that during the muscle checks for the motor control function, instead of
iterating through each neuron, we now iterate only through the relevant muscle neurons.

This postSynaptic dictionary contains the accumulated weighted values as the connectome is executed: postSynaptic = {}
The postSynaptic dictionary maintains the accumulated values for each neuron and muscle. The Accumulated values are initialized to Zero
*/

void TNemaBotOnFire::RunANema()
{
	/*## Here is where you would put in a method to stimulate the neurons ###
	### We stimulate chemosensory neurons constantly unless nose touch   ###
	### (sonar) is stimulated and then we fire nose touch neurites       ###
	 ## Start comment - use a fixed value if you want to stimulte nose touch
	## use something like "dist = 27" if you want to stop nose stimulation
	# dist = us_dist(15)
	## End Comment
	#Do we need to switch states at the end of each loop? No, this is done inside the runconnectome()
	#function, called inside each loop.  */

	if ( cbTouch->IsChecked ) { Connectome.NoseTouch(); }

	if (cbFood->IsChecked ) { Connectome.Food(); }

	Connectome.Run();

	UpdateSynapseList(); // update the values on page tbSynapseList if needed

	// Ask the Paintbox to be redrawn.
	PaintWorm->Repaint();

	iter = iter+1;
	edIteration->Text = IntToStr(iter);

 }  // RunANema

void TNemaBotOnFire::UpdateSynapseList()
{   TNeuron* aNeuron;
	if ( MainTabControl->ActiveTab == tbSynapseList )
	{  // currently displaying the synapse list, so worth updating them immediately
		for (int i=0; i < connectomeCount; i++) {
		   aNeuron = Connectome.Neurons.get(i);
		   ValEditors[i]->Text = FloatToStrF(aNeuron->OutVal,ffFixed,5,3) ;
		}
	}

}

void TNemaBotOnFire::ALine(TCanvas* aCanvas, TPointF A, TPointF B)
{
   aCanvas->DrawLine(A, B, 1);
}

TFillTextFlags fTextFlag = TFillTextFlags();

void ATextOutDXDY(TCanvas* aCanvas, float* X0, float* Y0, float DX, float DY, UnicodeString str)
{   float Width, Height;
	TRectF aRectF;
	if (DX==0) { Width  = 1000;} else  { Width = DX;}
	if (DY==0) { Height = 1000;} else  { Height= DY;}

	aRectF = RectF(*X0, *Y0, *X0+Width, *Y0+Height);
	aCanvas->FillText(aRectF,str,false,100,fTextFlag, TTextAlign::Leading, TTextAlign::Leading);
	*X0 = *X0 + DX;
	*Y0 = *Y0 + DY;
}
// reserves vertically space of height DY, but horizontall infinite space ?!
void TextSeriesOut(TCanvas* aCanvas, float X0, float Y0, float DX, float DY,
	const std::vector<UnicodeString>& strings)
{
	for (const auto& str : strings)
		{ ATextOutDXDY(aCanvas, &X0, &Y0, DX, DY, str); }
}

void IntSeriesOut(TCanvas* aCanvas, float X0, float Y0, float DX, float DY,
	const std::vector<int> ints)
{   UnicodeString str;
	for (const auto aInt : ints ) {
		str = IntToStr(aInt);
		ATextOutDXDY(aCanvas, &X0, &Y0, DX, DY, str) ; }
}

void FloatSeriesOut(TCanvas* aCanvas, float X0, float Y0, float DX, float DY,
	const std::vector<float> floats)
{	UnicodeString str;
	for (const auto aFloat : floats ) {
		str = FloatToStrF(aFloat,ffFixed,3,1);
		ATextOutDXDY(aCanvas, &X0, &Y0, DX, DY, str);
	}
}

void TNemaBotOnFire::PaintMuscles(TCanvas* aCanvas, float Width, float Height)
{   TPointF Origin, A, oldA, B, oldB, D, Rib, Dorsal;
	float    radius,dist;
	radius = Width / 30; // length of one rib
	dist = Height / 30; // length of one spine
	float  angle;
	angle = 0;  // zero is horizontal
	// calculating and drawing the PointFs
	Origin = PointF( Width * 2 / 3, 10); // PointF of the first Spine
	Rib   = PointF(radius,0);// direction of the first rib
	A = Origin - Rib;		// the two opposite rib PointFs
	B = Origin + Rib;
	ALine(aCanvas,A,B);     // draw the rib
	aCanvas->DrawEllipse(Rect(A.X,Origin.Y+dist,B.X,Origin.Y-dist),1);
	Dorsal = PointF(0,dist); // the direction of the spine PointFing downwards
	D = Origin + Dorsal;    // next PointF on the spine
	// ALine(aCanvas,Origin,D);// draw the first spine segment

	float kL, kR, Strength, d0, alpha;
	Strength = tbAngleScale->Value/tbAngleScale->Max; // scaling factor between alpha and the actual angle
	d0 = 0; // offset factor to play with

	// text output
	float   textX0, textY0, textDX, textDY;
	if (cbMuscleText->IsChecked ) {
		textX0 =10;
		textY0 =10;
		textDX =28;
		textDY =20;
		TextSeriesOut(aCanvas, textX0, textY0, textDX, 0, {"L","DL","VL","D","sum","V","DR","VR","R","alpha"});
	}


	aCanvas->Stroke->Color = claWhite;

	// one set of muscle values
	TDVLR    aDVLR;
	float pLPF = tbMuscleLowPass->Value / (tbMuscleLowPass->Max + 1);
	for (TNeuronIndex cnt=0; cnt < Connectome.musDleft.Count; cnt++) {
		// get the activation of the ith Muscles, dorsal, ventral, Left and Right separately
		aDVLR = Connectome.getMusclesDVLR(cnt);
		// low-pass filter the muscle activation
		aDVLR =  oldMuscles[cnt] * pLPF + aDVLR * (1-pLPF);
		kL = aDVLR.D(); // select dorsal and ventral activations to use as left and right elements of segment
		kR = aDVLR.V();
		if (std::abs(kL + kR) >= 1)
			{ aDVLR.alpha = (dist - d0) / radius * (kR - kL) / (kL + kR);}
		else
			{ aDVLR.alpha = 0; }
		oldMuscles[cnt] = aDVLR;

		// output all values as text
		if (cbMuscleText->IsChecked ) {
			FloatSeriesOut(aCanvas, textX0, textY0+(cnt+1)*textDY, textDX, 0,
			{aDVLR.L(),aDVLR.DL,aDVLR.VL,aDVLR.D(),aDVLR.sum(),aDVLR.V(),aDVLR.DR,aDVLR.VR,aDVLR.R(),alpha});
			// aCanvas->TextOut(textX0+9*textDX,textY0+(cnt+1)*textDY,FloatToStrF(alpha,ffFixed,5,3));
		}

		// depict the angles as a sequence of ribs
		angle = angle + aDVLR.alpha * Strength ; // accumulate angles

		// calculating and drawing the next PointFs
		oldA = A; oldB = B;     // remember them for redrawing later
		Origin = D;             // Previous PointF D is the new origin
		Rib   = PointF(radius * cos(angle) , radius* sin(angle));// direction of the first rib
		A = Origin - Rib;		// the two opposite rib PointFs
		B = Origin + Rib;
		if (cbShowRibs->IsChecked ) {ALine(aCanvas,A,B); }     // draw the rib
		ALine(aCanvas,oldA,A);  // draw the outer skin
		ALine(aCanvas,oldB,B);  // draw the outer skin
		Dorsal = PointF(- dist * sin(angle) , dist * cos(angle) ); // the direction of the spine
		D = Origin + Dorsal;    // next PointF on the spine
		//ALine(aCanvas,Origin,D);// draw the spine segment
	} // for all connectome muscles count
	//aCanvas->Ellipse(A.X,Origin.Y+dist,B.X,Origin.Y-dist);
	//aCanvas->PolyBezier({oldA,A,A,D},3)
}


//---------------------------------------------------------------------------

UnicodeString STLString2UCString(const std::string& str){
	UnicodeString ustr;
	for (int i=0; i < str.length(); i++)
		ustr += str[i];
	return ustr;
};

std::string UCString2STLString(const UnicodeString& ustr){
	std::string str;
	for (int i=0; i < ustr.Length(); i++)
		str += ustr[i+1];
	return str;
};

std::string U2Sv2 (const UnicodeString& src){
	char* temp;
	temp = new char[src.Length()];
	std::wcstombs(temp, src.c_str(), src.Length());
	std::string dest(temp);
	delete[] temp;
	return dest;
}


//---------------------------------------------------------------------------
void __fastcall TNemaBotOnFire::tbFPSChange(TObject *Sender)
{
	float FPS;
	FPS = exp( Ln(1000) *  tbFPS->Value / tbFPS->Max ); // scales from 1 to 1000
	Timer1->Interval = round( 1000/FPS );
	lbSpeed->Text = FloatToStrF(FPS,ffFixed,4,1);
}
//---------------------------------------------------------------------------
void __fastcall TNemaBotOnFire::acRunSimulationClick(TObject *Sender)
{
  Timer1->Enabled = cbRunSimulation->IsChecked;
}
//---------------------------------------------------------------------------
void __fastcall TNemaBotOnFire::Timer1Timer(TObject *Sender)
{
	RunANema();
}
//---------------------------------------------------------------------------
void __fastcall TNemaBotOnFire::PaintWormPaint(TObject *Sender, TCanvas *Canvas)
{
	PaintMuscles(PaintWorm->Canvas, PaintWorm->Width, PaintWorm->Height);
}
//---------------------------------------------------------------------------

void __fastcall TNemaBotOnFire::bnOnceClick(TObject *Sender)
{
	RunANema();
}
//---------------------------------------------------------------------------

void __fastcall TNemaBotOnFire::tbAngleScaleChange(TObject *Sender)
{
	// Ask the Paintbox to be redrawn.
	PaintWorm->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TNemaBotOnFire::MainTabControlChange(TObject *Sender)
{
  UpdateSynapseList(); // update the list unless if needed
}
//---------------------------------------------------------------------------

