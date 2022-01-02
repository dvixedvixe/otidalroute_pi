///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __otidalrouteUIDIALOGBASE_H__
#define __otidalrouteUIDIALOGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>
#include <wx/colordlg.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/splitter.h>
#include <wx/listbox.h>
#include <wx/radiobox.h>

class ConfigurationDialog;
class otidalroute_pi;
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class oTidalRoute
///////////////////////////////////////////////////////////////////////////////
class otidalrouteUIDialogBase : public wxFrame
{
private:

protected:
	
	
	wxPanel* m_panel2;
	wxButton* m_bCalcDR;
	wxButton* m_bCalcETA;
	wxTextCtrl* m_tSpeed;
	wxMenuBar* m_menubar3;
	wxMenu* m_menu2;
	wxMenu* m_menu3;
	wxMenu* m_menu4;
	wxMenu* m_mHelp;
	wxStaticText* m_staticText2;
	
	wxStaticText* m_staticText3;
	wxStaticText* m_staticText4;
	

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose(wxCloseEvent& event) { event.Skip(); }
	virtual void OnSize(wxSizeEvent& event) { event.Skip(); }
	virtual void DRCalculate(wxCommandEvent& event) { event.Skip(); }
	virtual void ETACalculate(wxCommandEvent& event) { event.Skip(); }
	virtual void OnSummary(wxCommandEvent& event) { event.Skip(); }
	virtual void OnShowTables(wxCommandEvent& event) { event.Skip(); }
	virtual void OnDeleteAllRoutes(wxCommandEvent& event) { event.Skip(); }
	virtual void OnInformation(wxCommandEvent& event) { event.Skip(); }
	virtual void OnAbout(wxCommandEvent& event) { event.Skip(); }

public:
	wxTextCtrl* m_textCtrl1;
	wxTextCtrl* m_textCtrl3;
	wxTextCtrl* m_textCtrl4;

	wxTextCtrl* m_tRouteName;
	wxCheckBox* m_cbGPX;
	wxChoice* m_choiceDepartureTimes;

	otidalrouteUIDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 364), long style = wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT | wxSYSTEM_MENU | wxTAB_TRAVERSAL | wxRESIZE_BORDER);

	~otidalrouteUIDialogBase();

};


///////////////////////////////////////////////////////////////////////////////
/// Class otidalroutePreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class otidalroutePreferencesDialogBase : public wxDialog
{
private:

protected:

	// Virtual event handlers, overide them in your derived class

public:


	wxCheckBox* m_cbUseRate;
	wxCheckBox* m_cbUseDirection;
	wxCheckBox* m_cbFillColour;

	wxStaticText* m_staticText4;
	wxStaticText* m_staticText41;
	wxStaticText* m_staticText42;
	wxStaticText* m_staticText43;
	wxStaticText* m_staticText44;


	wxButton* m_button111;
	wxButton* m_button2;
	wxButton* m_button3;
	wxButton* m_button4;
	wxButton* m_button5;

	wxColourPickerCtrl* myColourPicker0;
	wxColourPickerCtrl* myColourPicker1;
	wxColourPickerCtrl* myColourPicker2;
	wxColourPickerCtrl* myColourPicker3;
	wxColourPickerCtrl* myColourPicker4;

	wxCheckBox* m_cbCopyFirstCumulativeRecord;
	wxCheckBox* m_cbCopyMissingWaveRecord;
	wxRadioBox* m_rbLoadOptions;
	wxRadioBox* m_rbStartOptions;
	wxRadioBox* m_rbTimeFormat;

	otidalroutePreferencesDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~otidalroutePreferencesDialogBase();

};


///////////////////////////////////////////////////////////////////////////////
/// Class ConfigurationDialog
///////////////////////////////////////////////////////////////////////////////
class ConfigurationDialog : public wxDialog
{
private:
	
	

protected:

	wxButton* m_bDelete;
	wxButton* m_bSelect;
	wxButton* m_bGenerate;
	wxButton* m_bClose;

	// Virtual event handlers, overide them in your derived class
	void OnDelete(wxCommandEvent& event);
	void OnInformation(wxCommandEvent& event);
	void OnGenerate(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);

public:
	otidalroute_pi *pPlugIn;
	wxListBox* m_lRoutes;

	ConfigurationDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tidal Routes"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~ConfigurationDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialogBase
///////////////////////////////////////////////////////////////////////////////
class AboutDialogBase : public wxDialog
{
private:

protected:
	wxStaticText* m_staticText135;
	wxStaticText* m_stVersion;
	wxStaticText* m_staticText110;
	wxButton* m_bAboutAuthor;
	wxButton* m_bClose;

	// Virtual event handlers, overide them in your derived class
	virtual void OnAboutAuthor(wxCommandEvent& event) { event.Skip(); }
	virtual void OnClose(wxCommandEvent& event) { event.Skip(); }


public:

	AboutDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About Weather Routing"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~AboutDialogBase();

};


#endif //__otidalrouteUIDIALOGBASE_H__
