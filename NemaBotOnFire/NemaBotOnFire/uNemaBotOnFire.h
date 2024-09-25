//---------------------------------------------------------------------------

#ifndef uNemaBotOnFireH
#define uNemaBotOnFireH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Edit.hpp>
#include <FMX.TabControl.hpp>
//---------------------------------------------------------------------------

#include "uNeuronClass.h"
// it defines using TNeuronValue = float; and using TNeuronIndex = std::int16_t;
#include "uConnectome.h"
#include "HermaphroditeChemical.h"
#include <FMX.Objects.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.ActnList.hpp>
#include <System.Actions.hpp>


class TNemaBotOnFire : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TGroupBox *gbRunSimulation;
	TCheckBox *cbRunSimulation;
	TTrackBar *tbFPS;
	TLabel *lbSpeed;
	TLabel *lbIteration;
	TEdit *edIteration;
	TGroupBox *gbStimulation;
	TCheckBox *cbFood;
	TCheckBox *cbTouch;
	TButton *bnOnce;
	TStyleBook *StyleBook1;
	TTabControl *MainTabControl;
	TTabItem *tbSynapseList;
	TTabItem *tbMuscleGame;
	TPanel *Panel2;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TTimer *Timer1;
	TPaintBox *PaintWorm;
	TScrollBox *sbNeurons;
	TLabel *lbName;
	TEdit *edValue;
	TLabel *lbConnectingTo;
	TGroupBox *GroupBox1;
	TLabel *Label4;
	TTrackBar *TrackBar1;
	TLabel *Label5;
	TTrackBar *tbAngleScale;
	TLabel *Label7;
	TTrackBar *tbMuscleLowPass;
	TCheckBox *cbMuscleText;
	TActionList *ActionList1;
	TAction *acRunSimulation;
	TCheckBox *cbShowRibs;
	TAction *Action1;
	void __fastcall acRunSimulationClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall tbFPSChange(TObject *Sender);
	void __fastcall PaintWormPaint(TObject *Sender, TCanvas *Canvas);
	void __fastcall bnOnceClick(TObject *Sender);
	void __fastcall tbAngleScaleChange(TObject *Sender);
	void __fastcall MainTabControlChange(TObject *Sender);
private:	// User declarations
	std::vector<TLabel*> Labels;
	std::vector<TEdit*> ValEditors;
	std::vector<TLabel*> synLabels;
	std::vector<TDVLR> oldMuscles;
	void paintMuscles(TCanvas aCanvas);
public:		// User declarations
	// choose the connectome here, any class is ok that derives from TConnectome:
	THermaphroditeChemical Connectome;
	int   connectomeCount=0;
	int   iter  = 0;
	float dist  = 0;
	float tfood = 0;
	void  RunANema();
	void UpdateSynapseList();
	void PaintMuscles(TCanvas* aCanvas, float Width, float Height);
	void ALine(TCanvas* aCanvas, TPointF A, TPointF B);
	__fastcall TNemaBotOnFire(TComponent* Owner);
};

extern UnicodeString STLString2UCString(const std::string& str);
extern std::string UCString2STLString(const UnicodeString& ustr);

//---------------------------------------------------------------------------
extern PACKAGE TNemaBotOnFire *NemaBotOnFire;
//---------------------------------------------------------------------------
#endif
