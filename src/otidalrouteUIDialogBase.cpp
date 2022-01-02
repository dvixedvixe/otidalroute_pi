///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "otidalrouteUIDialogBase.h"
#include "otidalrouteUIDialog.h"
#include "otidalroute_pi.h"

///////////////////////////////////////////////////////////////////////////

otidalrouteUIDialogBase::otidalrouteUIDialogBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Calculation")), wxHORIZONTAL);

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer(new wxStaticBox(sbSizer4->GetStaticBox(), wxID_ANY, wxT("Speed")), wxVERTICAL);

	m_tSpeed = new wxTextCtrl(sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	sbSizer3->Add(m_tSpeed, 0, wxALL, 5);

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Start Date/Time")), wxVERTICAL);

	m_textCtrl1 = new wxTextCtrl(sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	sbSizer2->Add(m_textCtrl1, 0, wxALL, 5);

	wxStaticBoxSizer* sbSizer91;
	sbSizer91 = new wxStaticBoxSizer(new wxStaticBox(sbSizer2->GetStaticBox(), wxID_ANY, wxT("DR/ETA")), wxVERTICAL);

	m_staticText4 = new wxStaticText(sbSizer91->GetStaticBox(), wxID_ANY, wxT("Route Name"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->Wrap(-1);
	sbSizer91->Add(m_staticText4, 0, wxALL, 5);

	m_tRouteName = new wxTextCtrl(sbSizer91->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	sbSizer91->Add(m_tRouteName, 0, wxALL, 5);

	m_cbGPX = new wxCheckBox(sbSizer91->GetStaticBox(), wxID_ANY, wxT("Save as GPX file"), wxDefaultPosition, wxDefaultSize, 0);
	sbSizer91->Add(m_cbGPX, 0, wxALL, 5);

	m_staticText2 = new wxStaticText(sbSizer91->GetStaticBox(), wxID_ANY, wxT("Departure Times"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	sbSizer91->Add(m_staticText2, 0, wxALL, 5);

	wxString m_choiceDepartureTimesChoices[] = { wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6") };
	int m_choiceDepartureTimesNChoices = sizeof(m_choiceDepartureTimesChoices) / sizeof(wxString);
	m_choiceDepartureTimes = new wxChoice(sbSizer91->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDepartureTimesNChoices, m_choiceDepartureTimesChoices, 0);
	m_choiceDepartureTimes->SetSelection(0);
	sbSizer91->Add(m_choiceDepartureTimes, 0, wxALL, 5);

	m_bCalcDR = new wxButton(sbSizer91->GetStaticBox(), wxID_ANY, wxT("Calculate DR"), wxDefaultPosition, wxDefaultSize, 0);
	sbSizer91->Add(m_bCalcDR, 0, wxALL | wxEXPAND, 5);

	m_bCalcETA = new wxButton(sbSizer91->GetStaticBox(), wxID_ANY, wxT("Calculate ETA"), wxDefaultPosition, wxDefaultSize, 0);
	sbSizer91->Add(m_bCalcETA, 0, wxALL| wxEXPAND, 5);


	sbSizer2->Add(sbSizer91, 1, wxEXPAND, 5);


	sbSizer3->Add(sbSizer2, 1, wxEXPAND, 5);


	sbSizer4->Add(sbSizer3, 1, wxEXPAND, 5);

	
	fgSizer1->Add(sbSizer4, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer1);
	this->Layout();

	m_menubar3 = new wxMenuBar(0);
	
	m_menu3 = new wxMenu();

	wxMenuItem* m_mSummary;
	m_mSummary = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("Summary")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_mSummary);

	wxMenuItem* m_mNewRoute;
	m_mNewRoute = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("View Route Tables")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_mNewRoute);

	wxMenuItem* m_mDeleteAllRoutes;
	m_mDeleteAllRoutes = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("Delete All Routes")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_mDeleteAllRoutes);

	m_menubar3->Append(m_menu3, wxT("Routes"));

	m_mHelp = new wxMenu();

	wxMenuItem* m_mInformation;
	m_mInformation = new wxMenuItem(m_mHelp, wxID_ANY, wxString(_("&Guide")) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_NORMAL);
	m_mHelp->Append(m_mInformation);

	wxMenuItem* m_mAbout;
	m_mAbout = new wxMenuItem(m_mHelp, wxID_ANY, wxString(_("&About")), wxEmptyString, wxITEM_NORMAL);
	m_mHelp->Append(m_mAbout);
	
	m_menubar3->Append(m_mHelp, wxT("Help"));
	
	this->SetMenuBar(m_menubar3);


	this->Centre(wxBOTH);


	// Connect Events
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(otidalrouteUIDialogBase::OnClose));
	this->Connect(wxEVT_SIZE, wxSizeEventHandler(otidalrouteUIDialogBase::OnSize));

	m_bCalcDR->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::DRCalculate), NULL, this);
	m_bCalcETA->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::ETACalculate), NULL, this);

	this->Connect(m_mSummary->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnSummary));
	this->Connect(m_mNewRoute->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnShowTables));
	this->Connect(m_mDeleteAllRoutes->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnDeleteAllRoutes));
	this->Connect(m_mInformation->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnInformation));
	this->Connect(m_mAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnAbout));

}

otidalrouteUIDialogBase::~otidalrouteUIDialogBase()
{
	// Disconnect Events
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(otidalrouteUIDialogBase::OnClose));
	this->Disconnect(wxEVT_SIZE, wxSizeEventHandler(otidalrouteUIDialogBase::OnSize));

	m_bCalcDR->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::DRCalculate), NULL, this);
	m_bCalcETA->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::ETACalculate), NULL, this);

	this->Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnSummary));
	this->Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnShowTables));
	this->Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnDeleteAllRoutes));
	this->Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnInformation));
	this->Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnAbout));

}

otidalroutePreferencesDialogBase::otidalroutePreferencesDialogBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer(0, 1, 0, 0);
	fgSizer8->SetFlexibleDirection(wxBOTH);
	fgSizer8->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);


	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer(0, 1, 0, 0);
	fgSizer6->SetFlexibleDirection(wxBOTH);
	fgSizer6->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

	m_cbUseRate = new wxCheckBox(this, wxID_ANY, _("Display tidal rates (knots)"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer6->Add(m_cbUseRate, 0, wxALL, 5);

	m_cbUseDirection = new wxCheckBox(this, wxID_ANY, _("Display tidal direction"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer6->Add(m_cbUseDirection, 0, wxALL, 5);

	m_cbFillColour = new wxCheckBox(this, wxID_ANY, _("Fill Colour"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer6->Add(m_cbFillColour, 0, wxALL, 5);

	fgSizer8->Add(fgSizer6, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Tidal Current Arrow Colour")), wxVERTICAL);

	myColourPicker0 = new wxColourPickerCtrl(this, wxID_ANY, *wxRED,
		wxDefaultPosition, wxDefaultSize, 0);

	myColourPicker1 = new wxColourPickerCtrl(this, wxID_ANY, *wxRED,
		wxDefaultPosition, wxDefaultSize, 0);

	myColourPicker2 = new wxColourPickerCtrl(this, wxID_ANY, *wxRED,
		wxDefaultPosition, wxDefaultSize, 0);

	myColourPicker3 = new wxColourPickerCtrl(this, wxID_ANY, *wxRED,
		wxDefaultPosition, wxDefaultSize, 0);

	myColourPicker4 = new wxColourPickerCtrl(this, wxID_ANY, *wxRED,
		wxDefaultPosition, wxDefaultSize, 0);

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer(0, 2, 0, 0);

	m_staticText4 = new wxStaticText(this, wxID_ANY, _("      < 0.5 knots"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->Wrap(-1);
	gSizer1->Add(m_staticText4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	gSizer1->Add(myColourPicker0, 0, wxALL, 5);

	m_staticText41 = new wxStaticText(this, wxID_ANY, _(">= 0.5 and < 1.5"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText41->Wrap(-1);
	gSizer1->Add(m_staticText41, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	gSizer1->Add(myColourPicker1, 0, wxALL, 5);

	m_staticText42 = new wxStaticText(this, wxID_ANY, _(">= 1.5 and < 2.5"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText42->Wrap(-1);
	gSizer1->Add(m_staticText42, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	gSizer1->Add(myColourPicker2, 0, wxALL, 5);

	m_staticText43 = new wxStaticText(this, wxID_ANY, _(">= 2.5 and < 3.5"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText43->Wrap(-1);
	gSizer1->Add(m_staticText43, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	gSizer1->Add(myColourPicker3, 0, wxALL, 5);

	m_staticText44 = new wxStaticText(this, wxID_ANY, _(">= 3.5 knots    "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText44->Wrap(-1);
	gSizer1->Add(m_staticText44, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	gSizer1->Add(myColourPicker4, 0, wxALL, 5);

	sbSizer1->Add(gSizer1, 1, wxEXPAND, 5);
	bSizer2->Add(sbSizer1, 1, wxEXPAND, 5);

	fgSizer8->Add(bSizer2, 1, wxALL | wxEXPAND, 5);

	wxStdDialogButtonSizer* m_sdbSizer2;

	wxButton* m_sdbSizer2OK;
	wxButton* m_sdbSizer2Cancel;
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton(this, wxID_OK);
	m_sdbSizer2->AddButton(m_sdbSizer2OK);
	m_sdbSizer2Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer2->AddButton(m_sdbSizer2Cancel);
	m_sdbSizer2->Realize();

	fgSizer8->Add(m_sdbSizer2, 1, wxALL | wxEXPAND, 5);

	this->SetSizer(fgSizer8);
	this->Layout();
	fgSizer8->Fit(this);

	this->Centre(wxBOTH);

}

otidalroutePreferencesDialogBase::~otidalroutePreferencesDialogBase()
{

}


///////////////////////////////////////////////////////////////////////////

ConfigurationDialog::ConfigurationDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* fgSizer95;
	fgSizer95 = new wxFlexGridSizer(3, 1, 0, 0);
	fgSizer95->AddGrowableCol(0);
	fgSizer95->AddGrowableCol(1);
	fgSizer95->SetFlexibleDirection(wxBOTH);
	fgSizer95->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticBoxSizer* sbSizer29;
	sbSizer29 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Routes")), wxVERTICAL);

	m_lRoutes = new wxListBox(sbSizer29->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 | wxLB_ALWAYS_SB);
	sbSizer29->Add(m_lRoutes, 1, wxALL | wxEXPAND, 5);


	fgSizer95->Add(sbSizer29, 1, wxEXPAND, 5);

	wxFlexGridSizer* fgSizer78;
	fgSizer78 = new wxFlexGridSizer(1, 0, 0, 0);
	fgSizer78->SetFlexibleDirection(wxBOTH);
	fgSizer78->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_bDelete = new wxButton(this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bDelete, 0, wxALL, 5);

	m_bSelect = new wxButton(this, wxID_ANY, _("Route Table"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bSelect, 0, wxALL, 5);

	m_bGenerate = new wxButton(this, wxID_ANY, _("Chart Route"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bGenerate, 0, wxALL, 5);

	m_bClose = new wxButton(this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bClose, 0, wxALL, 5);

	fgSizer95->Add(fgSizer78, 1, wxEXPAND, 5);

	this->SetSizer(fgSizer95);
	this->Layout();
	fgSizer95->Fit(this);
	this->Centre(wxBOTH);

	// Connect Events
	m_bDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnDelete), NULL, this);
	m_bSelect->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnInformation), NULL, this);
	m_bGenerate->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnGenerate), NULL, this);
	m_bClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnClose), NULL, this);

}

ConfigurationDialog::~ConfigurationDialog()
{
	// Disconnect Events
	m_bDelete->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnDelete), NULL, this);
	m_bSelect->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnInformation), NULL, this);
	m_bGenerate->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnGenerate), NULL, this);
	m_bClose->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnClose), NULL, this);

}




void ConfigurationDialog::OnDelete(wxCommandEvent& event)
{
	wxString rn;
	int s;
	s = m_lRoutes->GetSelection();
	rn = m_lRoutes->GetString(s);
	
	for (std::list<TidalRoute>::iterator it = pPlugIn->m_potidalrouteDialog->m_TidalRoutes.begin();
		it != pPlugIn->m_potidalrouteDialog->m_TidalRoutes.end(); it++) {
		if ((*it).Name == rn ) {
			pPlugIn->m_potidalrouteDialog->m_TidalRoutes.erase(it);
			m_lRoutes->Delete(s);

			pPlugIn->m_potidalrouteDialog->SaveXML(pPlugIn->m_potidalrouteDialog->m_default_configuration_path);
		}
	}
}
void ConfigurationDialog::OnInformation(wxCommandEvent& event)
{
	wxString rn;
	int s;
	s = m_lRoutes->GetSelection();

	if (s == -1){ 		
		wxMessageBox(_("Please select a route"));
		return; 
	}
	
	rn = m_lRoutes->GetString(s);
	/*
	
	if (m_lRoutes->IsEmpty()){
		wxMessageBox(_("Please select positions and generate a route"));
		return;
	}
	*/
	pPlugIn->m_potidalrouteDialog->GetTable(rn);
	return; //
}


void ConfigurationDialog::OnGenerate(wxCommandEvent& event)
{
	wxString rn;
	int s;
	s = m_lRoutes->GetSelection();

	if (s == -1) {
		wxMessageBox(_("Please select a route"));
		return;
	}

	rn = m_lRoutes->GetString(s);

	pPlugIn->m_potidalrouteDialog->AddChartRoute(rn);
}


void ConfigurationDialog::OnClose(wxCommandEvent& event)
{
	Hide();
}


///////////////////////////////////////////////////////////////////////////

AboutDialogBase::AboutDialogBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* fgSizer90;
	fgSizer90 = new wxFlexGridSizer(0, 1, 0, 0);
	fgSizer90->SetFlexibleDirection(wxBOTH);
	fgSizer90->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxFlexGridSizer* fgSizer109;
	fgSizer109 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer109->SetFlexibleDirection(wxBOTH);
	fgSizer109->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText135 = new wxStaticText(this, wxID_ANY, _("oTidalRoute Plugin Version"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText135->Wrap(-1);
	fgSizer109->Add(m_staticText135, 0, wxALL, 5);

	m_stVersion = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_stVersion->Wrap(-1);
	fgSizer109->Add(m_stVersion, 0, wxALL, 5);


	fgSizer90->Add(fgSizer109, 1, wxEXPAND, 5);

	m_staticText110 = new wxStaticText(this, wxID_ANY, _("The oTidalRoute plugin for opencpn is intended to calculate routes based on computerized tidal data.\n\nLicense: GPLv3+\n\nSource Code:\nhttps://github.com/rasbats/oTidalRoute_pi\n\nAuthor:\nMike Rossiter\n\nMany thanks to all translators and testers."), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText110->Wrap(400);
	fgSizer90->Add(m_staticText110, 0, wxALL, 5);

	wxFlexGridSizer* fgSizer91;
	fgSizer91 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer91->SetFlexibleDirection(wxBOTH);
	fgSizer91->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_bAboutAuthor = new wxButton(this, wxID_ANY, _("About Author"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer91->Add(m_bAboutAuthor, 0, wxALL, 5);

	m_bClose = new wxButton(this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer91->Add(m_bClose, 0, wxALL, 5);


	fgSizer90->Add(fgSizer91, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer90);
	this->Layout();
	fgSizer90->Fit(this);

	this->Centre(wxBOTH);

	// Connect Events
	m_bAboutAuthor->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnAboutAuthor), NULL, this);
	m_bClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnClose), NULL, this);
}

AboutDialogBase::~AboutDialogBase()
{
	// Disconnect Events
	m_bAboutAuthor->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnAboutAuthor), NULL, this);
	m_bClose->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnClose), NULL, this);

}