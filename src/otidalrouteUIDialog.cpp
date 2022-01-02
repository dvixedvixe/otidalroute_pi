/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  otidalroute Object
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2016 by Mike Rossiter  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 *
 */
#include <wx/intl.h>
#include "wx/wx.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/debug.h>
#include <wx/graphics.h>
#include <wx/stdpaths.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "otidalroute_pi.h"
#include "icons.h"
#include <wx/arrimpl.cpp>

#ifdef __WXMSW__
#include <windows.h>
#endif
#include <memory.h> 

#include <wx/colordlg.h>
#include <wx/event.h>
#include "AboutDialog.h"


class GribRecordSet;
class TidalRoute;
class ConfigurationDialog;
class RouteProp;
class AboutDialog;

using namespace std;



#define FAIL(X) do { error = X; goto failed; } while(0)

//date/time in the desired time zone format
static wxString TToString( const wxDateTime date_time, const int time_zone )
{
    wxDateTime t( date_time );
    t.MakeFromTimezone( wxDateTime::UTC );
    if( t.IsDST() ) t.Subtract( wxTimeSpan( 1, 0, 0, 0 ) );
    switch( time_zone ) {
        case 0: return t.Format( _T(" %a %d-%b-%Y  %H:%M LOC"), wxDateTime::Local );
        case 1:
        default: return t.Format( _T(" %a %d-%b-%Y %H:%M  UTC"), wxDateTime::UTC );
    }
}


static double deg2rad(double degrees)
{
	return M_PI * degrees / 180.0;
}

static double rad2deg(double radians)
{
	return 180.0 * radians / M_PI;
}

static void CTSWithCurrent(double BG, double &VBG, double C, double VC, double &BC, double VBC)
{
	if (VC == 0) { // short-cut if no current
		BC = BG, VBG = VBC;
		return;
	}
	
	// Thanks to Geoff Sargent at "tidalstreams.net"

	double B5 = VC / VBC;
	double C1 = deg2rad(BG);
	double C2 = deg2rad(C);

	double C6 = asin(B5 * sin(C1 - C2));
	double B6 = rad2deg(C6);
	if ((BG + B6) > 360){
		BC = BG + B6 - 360;
	}
	else {
		BC = BG + B6;
	}
	VBG = (VBC * cos(C6)) + (VC * cos(C1 - C2));

}

static void CMGWithCurrent(double &BG, double &VBG, double C, double VC, double BC, double VBC)
{
	if (VC == 0) { // short-cut if no current
		BG = BC, VBG = VBC;
		return;
	}

	// Thanks to Geoff Sargent at "tidalstreams.net"
	// BUT this function has not been tested !!!

	double B5 = VC / VBC;
	double C1 = deg2rad(BC);
	double C2 = deg2rad(C);

	double B3 = VC;
	double B4 = VBC;

	double C5 = sqr(B3) + sqr(B4) + 2 * B3*B4*cos(C1 - C2);
	double D5 = C5;
	double E5 = sqrt(C5);
	double E6 = B3*sin(C2 - C1) / E5;
	double E7 = asin(E6);
	double E8 = rad2deg(E7);
	if ((BC + E8) > 360){
		BG = BC + E8 - 360;
	}
	else {
		BG = BC + E8;
	}
	VBG = E5;
}

#if !wxCHECK_VERSION(2,9,4) /* to work with wx 2.8 */
#define SetBitmap SetBitmapLabel
#endif


otidalrouteUIDialog::otidalrouteUIDialog(wxWindow *parent, otidalroute_pi *ppi)
	: otidalrouteUIDialogBase(parent), m_ConfigurationDialog(this, wxID_ANY, _("Tidal Routes"),
	wxDefaultPosition, wxSize(-1, -1), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	pParent = parent;
    pPlugIn = ppi;

    wxFileConfig *pConf = GetOCPNConfigObject();

    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/otidalroute" ) );

		pConf->Read ( _T ( "otidalrouteUseRate" ), &m_bUseRate );
        pConf->Read ( _T ( "otidalrouteUseDirection" ), &m_bUseDirection);
		pConf->Read ( _T ( "otidalrouteUseFillColour" ), &m_bUseFillColour);

		pConf->Read( _T("VColour0"), &myVColour[0], myVColour[0] );
		pConf->Read( _T("VColour1"), &myVColour[1], myVColour[1] );
		pConf->Read( _T("VColour2"), &myVColour[2], myVColour[2] );
		pConf->Read( _T("VColour3"), &myVColour[3], myVColour[3] );
		pConf->Read( _T("VColour4"), &myVColour[4], myVColour[4] );
		
		myUseColour[0] = myVColour[0];
		myUseColour[1] = myVColour[1];
		myUseColour[2] = myVColour[2];
		myUseColour[3] = myVColour[3];
		myUseColour[4] = myVColour[4];

    }


	m_default_configuration_path = ppi->StandardPath()
		+ _T("otidalroute_config.xml");
	
	if (!OpenXML(m_default_configuration_path, false)) {
		// create directory for plugin files if it doesn't already exist 
		wxFileName fn(m_default_configuration_path);
		wxFileName fn2 = fn.GetPath();
		if (!fn.DirExists()) {
			fn2.Mkdir();
			fn.Mkdir();
		}
	}
	
	m_ConfigurationDialog.pPlugIn = ppi;   

    this->Connect( wxEVT_MOVE, wxMoveEventHandler( otidalrouteUIDialog::OnMove ) );

	m_tSpeed->SetValue(_T("5"));
	m_dtNow = wxDateTime::Now(); 

	wxString initStartDate = m_dtNow.Format(_T("%Y-%m-%d  %H:%M"));
	m_textCtrl1->SetValue(initStartDate);

	b_showTidalArrow = false;

	DimeWindow( this );

    Fit();
    SetMinSize( GetBestSize() );
	
}

otidalrouteUIDialog::~otidalrouteUIDialog()
{
    wxFileConfig *pConf = GetOCPNConfigObject();;

    if(pConf) {

        pConf->SetPath ( _T ( "/Settings/otidalroute" ) );

		pConf->Write ( _T ( "otidalrouteUseRate" ), m_bUseRate );
		pConf->Write ( _T ( "otidalrouteUseDirection" ), m_bUseDirection );
		pConf->Write ( _T ( "otidalrouteUseFillColour" ), m_bUseFillColour );

		pConf->Write( _T("VColour0"), myVColour[0] );
		pConf->Write( _T("VColour1"), myVColour[1] );
		pConf->Write( _T("VColour2"), myVColour[2] );
		pConf->Write( _T("VColour3"), myVColour[3] );
		pConf->Write( _T("VColour4"), myVColour[4] );
		
    }
	SaveXML(m_default_configuration_path);
	
}

void otidalrouteUIDialog::SetCursorLatLon( double lat, double lon )
{
    m_cursor_lon = lon;
    m_cursor_lat = lat;
}

void otidalrouteUIDialog::SetViewPort( PlugIn_ViewPort *vp )
{
    if(m_vp == vp)  return;

    m_vp = new PlugIn_ViewPort(*vp);
}

void otidalrouteUIDialog::OnClose( wxCloseEvent& event )
{
    pPlugIn->OnotidalrouteDialogClose();
}

void otidalrouteUIDialog::OnShowTables(wxCommandEvent& event)
{
	b_showTidalArrow = false;
	GetParent()->Refresh();
	m_ConfigurationDialog.Show();
}

void otidalrouteUIDialog::OnDeleteAllRoutes(wxCommandEvent& event)
{
	if (m_TidalRoutes.empty()){
		wxMessageBox(_("No routes have been calculated"));
		return;
	}
	wxMessageDialog mdlg(this, _("Delete all routes?\n"),
		_("Delete All Routes"), wxYES | wxNO | wxICON_WARNING);
	if (mdlg.ShowModal() == wxID_YES) {
		m_TidalRoutes.clear();
		m_ConfigurationDialog.m_lRoutes->Clear();
	}

	GetParent()->Refresh();

}
void otidalrouteUIDialog::OnMove( wxMoveEvent& event )
{
    //    Record the dialog position
    wxPoint p = GetPosition();
    pPlugIn->SetotidalrouteDialogX( p.x );
    pPlugIn->SetotidalrouteDialogY( p.y );

    event.Skip();
}

void otidalrouteUIDialog::OnSize( wxSizeEvent& event )
{
    //    Record the dialog size
    wxSize p = event.GetSize();
    pPlugIn->SetotidalrouteDialogSizeX( p.x );
    pPlugIn->SetotidalrouteDialogSizeY( p.y );

    event.Skip();
}

void otidalrouteUIDialog::OpenFile(bool newestFile)
{
	m_bUseRate = pPlugIn->GetCopyRate();
	m_bUseDirection = pPlugIn->GetCopyDirection();
	m_bUseFillColour = pPlugIn->GetCopyColour();

	m_FolderSelected = pPlugIn->GetFolderSelected();
	m_IntervalSelected = pPlugIn->GetIntervalSelected();
}

wxString otidalrouteUIDialog::MakeDateTimeLabel(wxDateTime myDateTime)
{			
		wxDateTime dt = myDateTime;

		wxString s2 = dt.Format ( _T( "%Y-%m-%d"));
		wxString s = dt.Format(_T("%H:%M")); 
		wxString dateLabel = s2 + _T(" ") + s;	

		m_textCtrl1->SetValue(dateLabel);				

		return dateLabel;	
}

void otidalrouteUIDialog::OnInformation(wxCommandEvent& event)
{
	
	wxString infolocation = *GetpSharedDataLocation()
		+ _T("plugins/otidalroute_pi/data/") + _("TidalRoutingInformation.html");
	wxLaunchDefaultBrowser(_T("file:///") + infolocation);
	
}

void otidalrouteUIDialog::OnAbout(wxCommandEvent& event)
{
	AboutDialog dlg(GetParent());
	dlg.ShowModal();
}

void otidalrouteUIDialog::OnSummary(wxCommandEvent& event)
{
	TableRoutes* tableroutes = new TableRoutes(this, 7000, _T(" Route Summary"), wxPoint(200, 200), wxSize(650, 200), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	
	wxString RouteName;
	wxString From;
	wxString Towards;
	wxString StartTime;
	wxString EndTime;
	wxString Duration;
	wxString Distance;
	wxString Type;

	if (m_TidalRoutes.empty()){

		wxMessageBox(_("No routes found. Please make a route"));
		return;
	}		

	int in = 0;
	
	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		

		RouteName = (*it).Name;
		From = (*it).Start;
		Towards = (*it).End;
		StartTime = (*it).StartTime;
		EndTime = (*it).EndTime;
		Duration = (*it).Time;
		Distance = (*it).Distance;
		Type = (*it).Type;

		tableroutes->m_wpList->InsertItem(in, _T(""), -1);
		tableroutes->m_wpList->SetItem(in, 0, RouteName);
		tableroutes->m_wpList->SetItem(in, 1, From);
		tableroutes->m_wpList->SetItem(in, 2, Towards);
		tableroutes->m_wpList->SetItem(in, 3, StartTime);
		tableroutes->m_wpList->SetItem(in, 4, EndTime);
		tableroutes->m_wpList->SetItem(in, 5, Duration);
		tableroutes->m_wpList->SetItem(in, 6, Distance);
		tableroutes->m_wpList->SetItem(in, 7, Type);

		in++;	
		
	}

	tableroutes->Show();

	GetParent()->Refresh();
		
}


void otidalrouteUIDialog::OnShowRouteTable(){
	
	wxString name;

	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {

		if (!m_TidalRoutes.empty()){
			name = (*it).Name;
			break;
		}
		else {
			wxMessageBox(_("Please select or generate a route"));
			return;
		}
	}

	RouteProp* routetable = new RouteProp(this, 7000, _T("Tidal Routes"), wxPoint(200, 200), wxSize(650, 800), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	int in = 0;

	wxString lat;
	wxString lon;
	wxString etd;
	wxString cts;
	wxString smg;
	wxString dis;
	wxString brg;
	wxString set;
	wxString rat;

	routetable->m_PlanSpeedCtl->SetValue(pPlugIn->m_potidalrouteDialog->m_tSpeed->GetValue());

	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		name = (*it).Name;
		if (m_tRouteName->GetValue() == name){

			routetable->m_RouteNameCtl->SetValue(name);
			routetable->m_RouteStartCtl->SetValue((*it).Start);
			routetable->m_RouteDestCtl->SetValue((*it).End);

			routetable->m_TotalDistCtl->SetValue((*it).Distance);
			routetable->m_TimeEnrouteCtl->SetValue((*it).Time);
			routetable->m_StartTimeCtl->SetValue((*it).StartTime);			
			routetable->m_TypeRouteCtl->SetValue((*it).Type);

			for (std::list<Position>::iterator itp = (*it).m_positionslist.begin();
				itp != (*it).m_positionslist.end(); itp++) {

				name = (*itp).name;
				lat = (*itp).lat;
				lon = (*itp).lon;
				etd = (*itp).time;
				cts = (*itp).CTS;
				smg = (*itp).SMG;
				dis = (*itp).distTo;
				brg = (*itp).brgTo;
				set = (*itp).set;
				rat = (*itp).rate;

				routetable->m_wpList->InsertItem(in, _T(""), -1);
				routetable->m_wpList->SetItem(in, 1, name);
				routetable->m_wpList->SetItem(in, 2, dis);
				routetable->m_wpList->SetItem(in, 4, lat);
				routetable->m_wpList->SetItem(in, 5, lon);
				routetable->m_wpList->SetItem(in, 6, etd);
				routetable->m_wpList->SetItem(in, 8, cts);
				routetable->m_wpList->SetItem(in, 9, set);
				routetable->m_wpList->SetItem(in, 10, rat);

				in++;
			}
		}
	}

	routetable->Show();

}

void otidalrouteUIDialog::GetTable(wxString myRoute){

	wxString name;

	if (m_TidalRoutes.empty()){
		wxMessageBox(_("Please select or generate a route"));
		return;
	}	

	RouteProp* routetable = new RouteProp(this, 7000, _T("Tidal Route Table"), wxPoint(200, 200), wxSize(650, 800), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	int in = 0;

	wxString lat;
	wxString lon;
	wxString etd;
	wxString cts;
	wxString smg;
	wxString dis;
	wxString brg;
	wxString set;
	wxString rat;

	routetable->m_PlanSpeedCtl->SetValue(pPlugIn->m_potidalrouteDialog->m_tSpeed->GetValue());

	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		name = (*it).Name;
		if (myRoute == name){

			routetable->m_RouteNameCtl->SetValue(name);
			routetable->m_RouteStartCtl->SetValue((*it).Start);
			routetable->m_RouteDestCtl->SetValue((*it).End);

			routetable->m_TotalDistCtl->SetValue((*it).Distance);
			routetable->m_TimeEnrouteCtl->SetValue((*it).Time);
			routetable->m_StartTimeCtl->SetValue((*it).StartTime);
			routetable->m_TypeRouteCtl->SetValue((*it).Type);

			for (std::list<Position>::iterator itp = (*it).m_positionslist.begin();
				itp != (*it).m_positionslist.end(); itp++) {

				name = (*itp).name;
				lat = (*itp).lat;
				lon = (*itp).lon;
				etd = (*itp).time;
				cts = (*itp).CTS;
				smg = (*itp).SMG;
				dis = (*itp).distTo;
				brg = (*itp).brgTo;
				set = (*itp).set;
				rat = (*itp).rate;

				routetable->m_wpList->InsertItem(in, _T(""), -1);
				routetable->m_wpList->SetItem(in, 1, name);
				routetable->m_wpList->SetItem(in, 2, dis);	
				routetable->m_wpList->SetItem(in, 3, brg);
				routetable->m_wpList->SetItem(in, 4, lat);
				routetable->m_wpList->SetItem(in, 5, lon);
				routetable->m_wpList->SetItem(in, 6, etd);
				routetable->m_wpList->SetItem(in, 7, smg);
				routetable->m_wpList->SetItem(in, 8, cts);
				routetable->m_wpList->SetItem(in, 9, set);
				routetable->m_wpList->SetItem(in, 10, rat);

				in++;
			}
		}
	}

	routetable->Show();

}


void otidalrouteUIDialog::AddChartRoute(wxString myRoute) {

	//wxMessageBox(_("Route name exists"));

	PlugIn_Route* newRoute = new PlugIn_Route; // for adding a route on OpenCPN chart display
	PlugIn_Waypoint*  wayPoint = new PlugIn_Waypoint;
	
	double lati, loni, value, value1;

	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {

		if ((*it).Name == myRoute) {
			newRoute->m_GUID = (*it).m_GUID;
			newRoute->m_NameString = (*it).Name;
			newRoute->m_StartString = (*it).Start;
			newRoute->m_EndString = (*it).End;

			

			for (std::list<Position>::iterator itp = (*it).m_positionslist.begin();
				itp != (*it).m_positionslist.end(); itp++) {

				PlugIn_Waypoint*  wayPoint = new PlugIn_Waypoint;
				
				wayPoint->m_MarkName = (*itp).name;
				if (!(*itp).lat.ToDouble(&value)) { /* error! */ }
				lati = value;
				if (!(*itp).lon.ToDouble(&value1)) { /* error! */ }
				loni = value1;
				wayPoint->m_lat = lati;
				wayPoint->m_lon = loni;
				wayPoint->m_MarkDescription = (*itp).time;
				wayPoint->m_GUID = (*itp).guid;
				wayPoint->m_IsVisible = (*itp).show_name;				
				wayPoint->m_IconName = (*itp).icon_name;				

				newRoute->pWaypointList->Append(wayPoint);
			}
 
			AddPlugInRoute(newRoute, true);

			if ((*it).Type == wxT("ETA")) {
				wxMessageBox(_("ETA Route has been charted!"));
			}
			else if ((*it).Type == wxT("DR")) {
				wxMessageBox(_("DR Route has been charted!"));
			}
			GetParent()->Refresh();
			break;

		}

	}

}


void otidalrouteUIDialog::AddTidalRoute(TidalRoute tr)
{
	m_TidalRoutes.push_back(tr);
	wxString it = tr.Name;
	m_ConfigurationDialog.m_lRoutes->Append(it);

}

void otidalrouteUIDialog::RequestGrib(wxDateTime time)
{
        Json::Value v;
	time = time.FromUTC();

	v["Day"] = time.GetDay();
	v["Month"] = time.GetMonth();
	v["Year"] = time.GetYear();
	v["Hour"] = time.GetHour();
	v["Minute"] = time.GetMinute();
	v["Second"] = time.GetSecond();

        Json::FastWriter w;

	SendPluginMessage("GRIB_TIMELINE_RECORD_REQUEST", w.write(v));

	Lock();
	m_bNeedsGrib = false;
	Unlock();
}

void otidalrouteUIDialog::DRCalculate(wxCommandEvent& event)
{
	//wxMessageBox(wxT("here"));
	bool fGPX = m_cbGPX->GetValue();
	if (fGPX) {
		CalcDR(event, true, 1);
	}
	else {
		CalcDR(event, false, 1);
	}
}

void otidalrouteUIDialog::ETACalculate(wxCommandEvent& event)
{
	//wxMessageBox(wxT("here"));
	bool fGPX = m_cbGPX->GetValue();
	if (fGPX) {
		CalcETA(event, true, 1);
	}
	else {
		CalcETA(event, false, 1);
	}
}

void otidalrouteUIDialog::CalcDR(wxCommandEvent& event, bool write_file, int Pattern)
{
	
	if (m_tRouteName->GetValue() == wxEmptyString) {
		wxMessageBox(_("Please enter a name for the route!"));
		return;
	}

	
	m_choiceDepartureTimes->SetStringSelection(_T("1")); // we only need one DR route

	TidalRoute tr; // tidal route for saving in the config file
	PlugIn_Route* newRoute = new PlugIn_Route; // for immediate use as a route on OpenCPN chart display

	Position ptr;

	wxString m_RouteName;

	gotMyGPXFile = false; // only load the raw gpx file once for a DR route

	if (m_TidalRoutes.empty()) {

		m_RouteName = m_tRouteName->GetValue() + wxT(".") +wxT("DR");
		tr.Name = m_RouteName;
		tr.Type = _("DR");
	}
	else {
		for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
			it != m_TidalRoutes.end(); it++) {
			m_RouteName = m_tRouteName->GetValue() + wxT(".") + wxT("DR");
			if ((*it).Name == m_RouteName) {
				wxMessageBox(_("Route name already exists, please edit the name"));
				return;
			}
			else {
				tr.m_positionslist.clear();
				tr.Name = m_RouteName;
				tr.Type = _("DR");
			}
		}
	}

	newRoute->m_NameString = tr.Name;
	newRoute->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));

	tr.Start = wxT("Start");
	tr.End = wxT("End");
	tr.m_GUID = newRoute->m_GUID;


	if (OpenXML(gotMyGPXFile)) {

		bool error_occured = false;

		double lat1, lon1;



		int num_hours = 1;
		int n = 0;

		lat1 = 0.0;
		lon1 = 0.0;

		wxString s;
		if (write_file) {
			wxFileDialog dlg(this, _("Export DR Positions in GPX file as"), wxEmptyString, wxEmptyString, _T("GPX files (*.gpx)|*.gpx|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (dlg.ShowModal() == wxID_CANCEL) {
				error_occured = true;     // the user changed idea...
				return;
			}

			s = dlg.GetPath();
			if (dlg.GetPath() == wxEmptyString) { error_occured = true; if (dbg) printf("Empty Path\n"); }
		}

		//Validate input ranges
		if (!error_occured) {
			if (std::abs(lat1) > 90) { error_occured = true; }
			if (std::abs(lon1) > 180) { error_occured = true; }
			if (error_occured) wxMessageBox(_("error in input range validation"));
		}

		//Start writing GPX
		TiXmlDocument doc;
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
		doc.LinkEndChild(decl);
		TiXmlElement * root = new TiXmlElement("gpx");
		TiXmlElement * Route = new TiXmlElement("rte");
		TiXmlElement * RouteName = new TiXmlElement("name");
		TiXmlText * text4 = new TiXmlText(this->m_tRouteName->GetValue().ToUTF8());

		if (write_file) {
			doc.LinkEndChild(root);
			root->SetAttribute("version", "0.1");
			root->SetAttribute("creator", "otidalroute_pi by Rasbats");
			root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			root->SetAttribute("xmlns:gpxx", "http://www.garmin.com/xmlschemas/GpxExtensions/v3");
			root->SetAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
			root->SetAttribute("xmlns:opencpn", "http://www.opencpn.org");
			Route->LinkEndChild(RouteName);
			RouteName->LinkEndChild(text4);

			
		}

		switch (Pattern) {
		case 1:
		{
			if (dbg) cout << "DR Calculation\n";
			double speed = 0;
			int    interval = 1;

			if (!this->m_tSpeed->GetValue().ToDouble(&speed)) { speed = 5.0; } // 5 kts default speed

			speed = speed * interval;			

			double lati, loni;
			double latN[200], lonN[200];
			double latF, lonF;

			Position my_point;

			double value, value1;

			for (std::vector<Position>::iterator it = my_positions.begin(); it != my_positions.end(); it++) {

				if (!(*it).lat.ToDouble(&value)) { /* error! */ }
				lati = value;
				if (!(*it).lon.ToDouble(&value1)) { /* error! */ }
				loni = value1;

				waypointName[n] = (*it).wpt_num;
				//wxMessageBox(waypointName[n]);

				latN[n] = lati;
				lonN[n] = loni;

				n++;
			}

			my_positions.clear();
			n--;

			int routepoints = n + 1;

			double myDist, myBrng, myDistForBrng;
			myBrng = 0;
			myDist = 0;

			double myLast, route_dist;

			route_dist = 0;
			myLast = 0;
			myDistForBrng = 0;
			double total_dist = 0;
			int i, c;
			bool skip = false;
			bool inloop = false;
			bool setF = false;

			latF = latN[0];
			lonF = lonN[0];

			lati = latN[0];
			loni = lonN[0];

			double VBG;
			VBG = 0;
			int tc_index = 0;
			c = 0;
			double myD, myB;
			double myDI;

			bool skipleg = false;
			double lastVBG = 0;
			double lastVBG1 = 0;
			double VBG2 = 0;
			double rdHours = 0;
			double rdHours1 = 0;
			double rdHours2 = 0;
			double rdHours3 = 0;
			double fttg = 0;
			double ttg = 0;
			double ttg1 = 0;
			double ttg2 = 0;
			double ttg3 = 0;
			double rdMiles = 0;
			double bitDist1 = 0;
			double bitDist2 = 0;
			double bitDist3 = 0;
			double lastBrg = 0;
			int skipCount = 0;

			bool skip0 = false;
			bool skip1 = false;
			int count3MinuteSteps = 0;
			double spd, dir;
			spd = 0;
			dir = 0;

			double iDist = 0;
			double tdist = 0; // For accumulating the total distance by adding the distance for each leg
			double ptrDist = 0;
			double skippedDistance = 0;

			double iTime = 0;
			double ptrTime = 0;
			long longMinutes = 0;


			int epNumber = 0;
			wxString epName;

			wxDateTime dt, iPointDT, gdt;

			wxString ddt, sdt;
			wxTimeSpan HourSpan, MinuteSpan, threeMinuteSpan;
			HourSpan = wxTimeSpan::Hours(1);
			MinuteSpan = wxTimeSpan::Minutes(30);
			threeMinuteSpan = wxTimeSpan::Minutes(3);
			wxTimeSpan pointMinuteSpan = wxTimeSpan::Minutes(3);

			bool madeETA = false;

			wxDateTime dtStart, dtEnd, interimDateTimeETA;
			wxTimeSpan trTime;
			double trTimeHours;

			sdt = m_textCtrl1->GetValue(); // date/time route starts
			dt.ParseDateTime(sdt);

			sdt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
			tr.StartTime = sdt;

			dtStart = dt;

			//
			// Start of new logic
			//

			VBG = speed;

			for (i = 0; i < n; i++) {	// n is number of routepoints					

				lastBrg = myBrng;
				bitDist2 = myDist;
				lastVBG1 = VBG;
				DistanceBearingMercator(latN[i + 1], lonN[i + 1], latN[i], lonN[i], &myDist, &myBrng);

				//
				// set up the waypoint
				//
				//

				PlugIn_Waypoint*  newPoint = new PlugIn_Waypoint
				(latN[i], lonN[i], wxT("Circle"), waypointName[i]);
				newPoint->m_IconName = wxT("Circle");
				newPoint->m_MarkDescription = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
				newPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
				newRoute->pWaypointList->Append(newPoint);


				//
				// set up a tidal route to save in the config file
				//

				if (i == 0) {

					interimDateTimeETA = dt;

					ptr.name = waypointName[i];
					ptr.time = sdt;
					ptr.lat = wxString::Format(_T("%8.4f"), latN[0]);
					ptr.lon = wxString::Format(_T("%8.4f"), lonN[0]);
					ptr.guid = newPoint->m_GUID;
					ptr.CTS = wxString::Format(_T("%03.0f"), myBrng);
					ptr.SMG = wxString::Format(_T("%5.1f"), VBG);
					ptr.distTo = _T("----");
					ptr.brgTo = _T("----");
					ptr.set = wxString::Format(_T("%03.0f"), dir);;
					ptr.rate = wxString::Format(_T("%5.1f"), spd);;
					ptr.icon_name = wxT("Circle");
					ptr.show_name = true;
					tr.m_positionslist.push_back(ptr);
					tr.Start = wxString::Format(wxT("%i"), i);
				}
				else {

					ptr.name = wxString::Format(wxT("%i"), i);
					if (i == 1 && skipleg) {

						longMinutes = (long)(iTime);
						pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
						iPointDT = dt + pointMinuteSpan;
						ptr.time = iPointDT.Format(_T(" %a %d-%b-%Y  %H:%M"));
					}
					else {
						if (!madeETA) {

							longMinutes = (long)(ptrTime);
							pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
							iPointDT = interimDateTimeETA + pointMinuteSpan;
							ptr.time = iPointDT.Format(_T(" %a %d-%b-%Y  %H:%M"));

						}
						else {  // we made an ETA before we got here

							longMinutes = (long)(ptrTime);
							pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
							iPointDT = dt + pointMinuteSpan;
							ptr.time = iPointDT.Format(_T(" %a %d-%b-%Y  %H:%M"));

							interimDateTimeETA = iPointDT;

						}

						//wxMessageBox(wxString::Format(wxT("%i"), longMinutes), wxString::Format(wxT("%i"), i));

					}


					ptr.name = waypointName[i];
					ptr.lat = wxString::Format(_T("%8.4f"), latN[i]);
					ptr.lon = wxString::Format(_T("%8.4f"), lonN[i]);
					ptr.guid = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
					ptr.CTS = wxString::Format(_T("%03.0f"), myBrng);
					ptr.SMG = wxString::Format(_T("%5.1f"), VBG);
					ptr.distTo = wxString::Format(_T("%5.1f"), ptrDist);
					ptr.brgTo = wxString::Format(_T("%03.0f"), myBrng);
					ptr.set = wxString::Format(_T("%03.0f"), dir);
					ptr.rate = wxString::Format(_T("%5.1f"), spd);
					ptr.icon_name = wxT("Circle");
					ptr.show_name = true;
					tr.m_positionslist.push_back(ptr);

					iDist = 0;
					ptrDist = 0;
					iTime = 0;
					ptrTime = 0;

					madeETA = false;

				}

				//
				// save the GPX file routepoint
				//

				my_point.lat = wxString::Format(wxT("%f"), latN[i]);
				my_point.lon = wxString::Format(wxT("%f"), lonN[i]);
				my_point.routepoint = 1;
				my_point.wpt_num = waypointName[i];
				my_point.name = waypointName[i];
				my_points.push_back(my_point);

				latF = latN[i];
				lonF = lonN[i];

				//
				// we are allowing for two legs to be skipped without the calculation stopping
				// (this is two consecutive legs of 3 minutes duration)
				//

				if (myDist < VBG / 20 && i == 0) {  // test whether first leg needs to be skipped
					fttg = myDist / VBG;
					//wxMessageBox(wxString::Format(wxT("%f"), fttg), wxString::Format(wxT("%i"), i));
					//longMinutes = (long)(fttg * 60);
					iTime = fttg * 60;
					//pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
					skipleg = true;
					skippedDistance = myDist;
				}

				if (skipleg) {  // previous leg was skipped
					//
					// sailing a leg after the skipped leg
					//
					if (i == 0) {
						//
						//
						iDist = skippedDistance;
						// 
						//
					}

					else {

						if (i == 1) {
							rdHours1 = bitDist2 / lastVBG1;
							ttg = 0.05 - (rdHours1);
							rdMiles = ttg * VBG;

							fttg = (rdHours1)+myDist / VBG;

							if (fttg < 0.05) {

								wxMessageBox(_("Unable to calculate ETA over two legs at this speed. \n\n Aborting"), wxT("Problem"));
								return;
							}
							//pointMinuteSpan = wxTimeSpan::Minutes(fttg*60);

							destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
							iDist = rdMiles;  // skipped first leg so no need for skippedDistance this time
							iTime = (iDist / VBG) * 60;
							skippedDistance = 0;
							skipleg = false;

						}
						else {

							rdHours1 = bitDist1 / lastVBG;
							rdHours2 = bitDist2 / lastVBG1;
							ttg = 0.05 - (rdHours1 + rdHours2);
							rdMiles = ttg * VBG;

							fttg = rdHours1 + rdHours2 + (myDist / VBG);
							if (fttg < 0.05) {

								wxMessageBox(_("Unable to calculate ETA over two legs at this speed. \n\n Aborting"), wxT("Problem"));
								return;
							}

							destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
							iDist = skippedDistance + rdMiles;
							iTime = (iDist / VBG) * 60;
							skippedDistance = 0;
							skipleg = false;

						}

					}

				}
				else {
					//
					// Not sailing a skipped leg
					//

					if (i == 1) {
						if (skipleg) {
							rdHours1 = bitDist2 / lastVBG1;
							ttg = 0.05 - rdHours1;
							rdMiles = ttg * VBG;

							fttg = rdHours1 + (myDist / VBG);
							destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);

							if (fttg < 0.05) {

								skipleg = true; // two skips and you are out ... first and second legs
								wxMessageBox(_("Unable to calculate ETA over two legs at this speed. \n\n Aborting"), wxT("Problem"));
								return;

							}
							else {
								pointMinuteSpan = wxTimeSpan::Minutes(fttg * 60);
								iDist = skippedDistance + rdMiles;
								iTime = (iDist / VBG) * 60;
								skippedDistance = 0;
								skipleg = false;
							}

						}
						else {
							rdHours1 = bitDist1 / lastVBG;
							ttg = 0.05 - rdHours1;
							rdMiles = ttg * VBG;

							fttg = rdHours1 + (myDist / VBG);
							destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);


							if (fttg < 0.05) {
								skippedDistance = rdMiles;
								//iTime = (skippedDistance / VBG) * 60;
								//iDist = skippedDistance;
								skipleg = true;

							}
							else {
								iDist = skippedDistance + rdMiles;
								iTime = (iDist / VBG) * 60;
								skippedDistance = 0;
								skipleg = false;
							}

						}

					}
					else {
						//
						// i !=1
						//
						if (i == 0) {

							rdMiles = VBG / 20;

							fttg = myDist / VBG;
							destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
							iDist = rdMiles;
							iTime = (iDist / VBG) * 60;
						}
						else {
							rdHours1 = bitDist1 / lastVBG;
							ttg = 0.05 - rdHours1;
							rdMiles = ttg * VBG;

							fttg = rdHours1 + (myDist / VBG);
							destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
							iDist = rdMiles;
							iTime = (iDist / VBG) * 60;

							if (fttg < 0.05) {
								skippedDistance = rdMiles;
								skipleg = true;

							}
							else {
								iDist = skippedDistance + rdMiles;
								iTime = (iDist / VBG) * 60;

								skippedDistance = 0;
								skipleg = false;
							}

						}
					}

				}

				tdist += iDist;
				ptrDist = iDist;
				ptrTime = iTime;

				//wxMessageBox(wxString::Format(wxT("%f"), skippedDistance), wxString::Format(wxT("%i"), i));

				while (fttg > 0.05 && skipleg == false) {

					count3MinuteSteps++;

					latF = lati;
					lonF = loni;

					DistanceBearingMercator(latN[i + 1], lonN[i + 1], lati, loni, &myD, &myB);

					myDist = myD;

					bitDist1 = myD;

					if (count3MinuteSteps == 20) {

						epNumber++;
						epName = wxString::Format(wxT("%i"), epNumber);
						PlugIn_Waypoint*  epPoint = new PlugIn_Waypoint
						(lati, loni, wxT("Symbol-X-Large-Magenta"), (_T("DR") + epName));
						epPoint->m_IconName = wxT("Symbol-X-Large-Magenta");
						epPoint->m_MarkDescription = dt.Format(_T("%a %d-%b-%Y  %H:%M"));
						epPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
						newRoute->pWaypointList->Append(epPoint);

						// print mid points for the GPX file
						my_point.lat = wxString::Format(wxT("%f"), lati);
						my_point.lon = wxString::Format(wxT("%f"), loni);
						my_point.routepoint = 0;
						my_point.wpt_num = _T("DR") + wxString::Format(_T("%i"), epNumber);
						my_point.name = _T("DR") + wxString::Format(_T("%i"), epNumber);
						my_points.push_back(my_point);

						//tc_index++;
						count3MinuteSteps = 0;
						dt.Add(HourSpan);
						ddt = dt.Format(_T("%a %d-%b-%Y  %H:%M"));
						

						ptr.name = _T("DR") + epName;
						ptr.lat = wxString::Format(_T("%8.4f"), lati);
						ptr.lon = wxString::Format(_T("%8.4f"), loni);
						ptr.time = ddt;
						ptr.guid = epPoint->m_GUID;
						ptr.distTo = wxString::Format(_T("%.1f"), ptrDist);
						ptr.brgTo = wxString::Format(_T("%03.0f"), myBrng);
						ptr.CTS = wxString::Format(_T("%03.0f"), myBrng);
						ptr.SMG = wxString::Format(_T("%5.1f"), VBG);
						ptr.set = wxString::Format(_T("%03.0f"), dir);
						ptr.rate = wxString::Format(_T("%5.1f"), spd);
						ptr.icon_name = wxT("Symbol-X-Large-Magenta");
						ptr.show_name = true;

						tr.m_positionslist.push_back(ptr);

						iDist = 0;
						iTime = 0;
						ptrDist = 0;
						ptrTime = 0;
						madeETA = true;

						
					}

					destLoxodrome(latF, lonF, myB, VBG / 20, &lati, &loni);
					// for distances travelled
					myDI = VBG / 20;
					//DistanceBearingMercator(latF, lonF, lati, loni, &myDI, &myBI);


					fttg = myD / VBG;

					double t;
					if (fttg < 0.05) {
						tdist += myD;
						ptrDist += myD;
						t = fttg * 60;
					}
					else {
						tdist += myDI;
						ptrDist += myDI;
						t = 3;
					}

					ptrTime += t;

					lastVBG = VBG;  // VBG for the next ETA point		

				}

			}


			// End of new logic
			// print the last routepoint
			my_point.lat = wxString::Format(wxT("%f"), latN[i]);
			my_point.lon = wxString::Format(wxT("%f"), lonN[i]);
			my_point.routepoint = 1;
			my_point.wpt_num = waypointName[n];
			my_point.name = waypointName[n];
			my_points.push_back(my_point);
			//
			PlugIn_Waypoint*  endPoint = new PlugIn_Waypoint
			(latN[n], lonN[n], wxT("Circle"), tr.End);


			double minutesToEnd;

			minutesToEnd = (ptrDist / VBG) * 60;

			dt.Add(wxTimeSpan::Minutes((long)minutesToEnd));
			ddt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));

			endPoint->m_IconName = wxT("Circle");
			endPoint->m_MarkName = waypointName[n];
			endPoint->m_MarkDescription = ddt;
			endPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));

			newRoute->pWaypointList->Append(endPoint);

			tr.EndTime = ddt;

			dtEnd = dt;
			trTime = dtEnd - dtStart;
			trTimeHours = (double)trTime.GetMinutes() / 60;
			tr.Time = wxString::Format(_("%.1f"), trTimeHours);

			tdist += iDist;
			tr.Distance = wxString::Format(_("%.1f"), tdist);

			ptr.name = waypointName[i];
			ptr.lat = wxString::Format(_T("%8.4f"), latN[n]);
			ptr.lon = wxString::Format(_T("%8.4f"), lonN[n]);
			ptr.time = ddt;
			ptr.guid = endPoint->m_GUID;
			ptr.set = wxString::Format(_T("%03.0f"), dir);
			ptr.rate = wxString::Format(_T("%5.1f"), spd);
			ptr.CTS = _T("----");
			ptr.SMG = _T("----");
			ptr.distTo = wxString::Format(_T("%.1f"), ptrDist);
			ptr.brgTo = wxString::Format(_T("%03.0f"), myBrng);
			ptr.icon_name = wxT("Circle");
			ptr.show_name = true;

			tr.m_positionslist.push_back(ptr);
			tr.End = wxString::Format(wxT("%i"), i);
			tr.Type = wxT("DR");
			m_TidalRoutes.push_back(tr);

			//AddPlugInRoute(newRoute); // add the route to OpenCPN routes and display the route on the chart			

			SaveXML(m_default_configuration_path); // add the route and extra detail (times, CTS etc) to the configuration file

			m_ConfigurationDialog.m_lRoutes->Append(tr.Name);
			m_ConfigurationDialog.Refresh();
			GetParent()->Refresh();

			for (std::vector<Position>::iterator itOut = my_points.begin(); itOut != my_points.end(); itOut++) {
				//wxMessageBox((*it).lat, _T("*it.lat"));

				double value, value1;
				if (!(*itOut).lat.ToDouble(&value)) { /* error! */ }
				lati = value;
				if (!(*itOut).lon.ToDouble(&value1)) { /* error! */ }
				loni = value1;

				if ((*itOut).routepoint == 1) {
					if (write_file) { Addpoint(Route, wxString::Format(wxT("%f"), lati), wxString::Format(wxT("%f"), loni), (*itOut).name, wxT("Diamond"), _T("WPT")); }
				}
				else {
					if ((*itOut).routepoint == 0) {
						if (write_file) { Addpoint(Route, wxString::Format(wxT("%f"), lati), wxString::Format(wxT("%f"), loni), (*itOut).name, wxT("Symbol-X-Large-Magenta"), _T("WPT")); }
					}
				}

			}

			my_points.clear();
			break;
		}

		default:
		{            // Note the colon, not a semicolon
			cout << "Error, bad input, quitting\n";
			break;
		}
		}

		if (write_file) {

			TiXmlElement * Extensions = new TiXmlElement("extensions");

			TiXmlElement * StartN = new TiXmlElement("opencpn:start");
			TiXmlText * text5 = new TiXmlText(waypointName[0].ToUTF8());
			Extensions->LinkEndChild(StartN);
			StartN->LinkEndChild(text5);

			TiXmlElement * EndN = new TiXmlElement("opencpn:end");			
			TiXmlText * text6 = new TiXmlText(waypointName[n].ToUTF8());
			Extensions->LinkEndChild(EndN);
			EndN->LinkEndChild(text6);

			Route->LinkEndChild(Extensions);

			root->LinkEndChild(Route);
			// check if string ends with .gpx or .GPX
			if (!wxFileExists(s)) {
				//s = s + _T(".gpx");
			}
			wxCharBuffer buffer = s.ToUTF8();
			if (dbg) std::cout << buffer.data() << std::endl;
			doc.SaveFile(buffer.data());
		}



		//end of if no error occured

		if (error_occured == true) {
			wxLogMessage(_("Error in calculation. Please check input!"));
			wxMessageBox(_("Error in calculation. Please check input!"));
		}
	}

	wxMessageBox(_("DR Route has been calculated!"));
}

void otidalrouteUIDialog::CalcETA(wxCommandEvent& event, bool write_file, int Pattern)
{
	if (m_tRouteName->GetValue() == wxEmptyString) {
		wxMessageBox(_("Please enter a name for the route!"));
		return;
	}

	if (m_textCtrl1->GetValue() == wxEmptyString) {
		wxMessageBox(_("Open the GRIB plugin and select a time!"));
		return;
	}
	
	wxString s_departureTimes = m_choiceDepartureTimes->GetStringSelection();
	int m_departureTimes = wxAtoi(s_departureTimes);
	int r = 0;
	wxString m_RouteName;
	gotMyGPXFile = false; // only load the raw gpx file once

	for (r = 0; r < m_departureTimes; r++) {

		TidalRoute tr; // tidal route for saving in the config file
		PlugIn_Route* newRoute = new PlugIn_Route; // for immediate use as a route on OpenCPN chart display

		Position ptr;

		if (m_TidalRoutes.empty()) {



			m_RouteName = m_tRouteName->GetValue() + wxT(".") + wxString::Format(wxT("%i"), r) + wxT(".") + wxT("EP");
			tr.Name = m_RouteName;
			tr.Type = _("ETA");
		}
		else {
			for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
				it != m_TidalRoutes.end(); it++) {
				m_RouteName = m_tRouteName->GetValue() + wxT(".") + wxString::Format(wxT("%i"), r) + wxT(".") + wxT("EP");
				if ((*it).Name == m_RouteName) {
					wxMessageBox(_("Route name already exists, please edit the name"));
					return;
				}
				else {
					tr.m_positionslist.clear();
					tr.Name = m_RouteName;
					tr.Type = _("ETA");
				}
			}
		}

		newRoute->m_NameString = tr.Name;
		newRoute->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));

		tr.Start = wxT("Start");
		tr.End = wxT("End");
		tr.m_GUID = newRoute->m_GUID;

		if (OpenXML(gotMyGPXFile)) {

			bool error_occured = false;

			double lat1, lon1;



			int num_hours = 1;
			int n = 0;

			lat1 = 0.0;
			lon1 = 0.0;

			wxString s;
			if (write_file) {
				wxFileDialog dlg(this, _("Export ETA Positions in GPX file as"), wxEmptyString, wxEmptyString, _T("GPX files (*.gpx)|*.gpx|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (dlg.ShowModal() == wxID_CANCEL) {
					error_occured = true;     // the user changed idea...
					return;
				}

				s = dlg.GetPath();
				if (dlg.GetPath() == wxEmptyString) { error_occured = true; if (dbg) printf("Empty Path\n"); }
			}

			//Validate input ranges
			if (!error_occured) {
				if (std::abs(lat1) > 90) { error_occured = true; }
				if (std::abs(lon1) > 180) { error_occured = true; }
				if (error_occured) wxMessageBox(_("error in input range validation"));
			}

			//Start writing GPX
			TiXmlDocument doc;
			TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
			doc.LinkEndChild(decl);
			TiXmlElement * root = new TiXmlElement("gpx");
			TiXmlElement * Route = new TiXmlElement("rte");
			TiXmlElement * RouteName = new TiXmlElement("name");
			TiXmlText * text4 = new TiXmlText(this->m_tRouteName->GetValue().ToUTF8());

			if (write_file) {
				doc.LinkEndChild(root);
				root->SetAttribute("version", "0.1");
				root->SetAttribute("creator", "otidalroute_pi by Rasbats");
				root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
				root->SetAttribute("xmlns:gpxx", "http://www.garmin.com/xmlschemas/GpxExtensions/v3");
				root->SetAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
				root->SetAttribute("xmlns:opencpn", "http://www.opencpn.org");
				Route->LinkEndChild(RouteName);
				RouteName->LinkEndChild(text4);				
			}

			switch (Pattern) {
			case 1:
			{
				if (dbg) cout << "ETA Calculation\n";
				double speed = 0;
				int    interval = 1;

				if (!this->m_tSpeed->GetValue().ToDouble(&speed)) { speed = 5.0; } // 5 kts default speed

				speed = speed * interval;				

				double lati, loni;
				double latN[200], lonN[200];
				double latF, lonF;

				Position my_point;

				double value, value1;

				for (std::vector<Position>::iterator it = my_positions.begin(); it != my_positions.end(); it++) {

					if (!(*it).lat.ToDouble(&value)) { /* error! */ }
					lati = value;
					if (!(*it).lon.ToDouble(&value1)) { /* error! */ }
					loni = value1;

					waypointName[n] = (*it).wpt_num;
					//wxMessageBox(waypointName[n]);

					latN[n] = lati;
					lonN[n] = loni;

					n++;
				}

				my_positions.clear();
				n--;

				int routepoints = n + 1;

				double myDist, myBrng, myDistForBrng;
				myBrng = 0;
				myDist = 0;

				double myLast, route_dist;

				route_dist = 0;
				myLast = 0;
				myDistForBrng = 0;
				double total_dist = 0;
				int i, c;
				bool skip = false;
				bool inloop = false;
				bool setF = false;

				latF = latN[0];
				lonF = lonN[0];

				lati = latN[0];
				loni = lonN[0];

				double VBG, BC;
				VBG = 0;
				int tc_index = 0;
				c = 0;
				double myD, myB;
				double myDI;

				bool skipleg = false;
				double lastVBG = 0;
				double lastVBG1 = 0;
				double VBG2 = 0;
				double rdHours = 0;
				double rdHours1 = 0;
				double rdHours2 = 0;
				double rdHours3 = 0;
				double fttg = 0;
				double ttg = 0;
				double ttg1 = 0;
				double ttg2 = 0;
				double ttg3 = 0;
				double rdMiles = 0;
				double bitDist1 = 0;
				double bitDist2 = 0;
				double bitDist3 = 0;
				double lastBrg = 0;
				int skipCount = 0;

				bool skip0 = false;
				bool skip1 = false;
				int count3MinuteSteps = 0;
				bool m_bGrib;
				double spd, dir;
				spd = 0;
				dir = 0;

				double iDist = 0;
				double tdist = 0; // For accumulating the total distance by adding the distance for each leg
				double ptrDist = 0;
				double skippedDistance = 0;

				double iTime = 0;
				double ptrTime = 0;
				long longMinutes = 0;


				int epNumber = 0;
				wxString epName;

				wxDateTime dt, iPointDT, gdt;

				wxString ddt, sdt;
				wxTimeSpan HourSpan, MinuteSpan, threeMinuteSpan;
				HourSpan = wxTimeSpan::Hours(1);
				MinuteSpan = wxTimeSpan::Minutes(30);
				threeMinuteSpan = wxTimeSpan::Minutes(3);
				wxTimeSpan pointMinuteSpan = wxTimeSpan::Minutes(3);

				bool madeETA = false;

				wxDateTime dtStart, dtEnd, interimDateTimeETA;
				wxTimeSpan trTime;
				double trTimeHours;

				sdt = m_textCtrl1->GetValue(); // date/time route starts
				dt.ParseDateTime(sdt);

				dt = dt + wxTimeSpan::Hours(r);

				sdt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
				tr.StartTime = sdt;

				dtStart = dt;

				//
				// Start of new logic
				//

				for (i = 0; i < n; i++) {	// n is number of routepoints					

					lastBrg = myBrng;
					bitDist2 = myDist;
					lastVBG1 = VBG;
					DistanceBearingMercator(latN[i + 1], lonN[i + 1], latN[i], lonN[i], &myDist, &myBrng);

					gdt = dt + MinuteSpan;
					m_bGrib = GetGribSpdDir(gdt, latN[i], lonN[i], spd, dir);
					if (!m_bGrib) {
						wxMessageBox(_("Route start date is not compatible with this Grib \n Or Grib is not available for part of the route"));
						return;
					}

					//wxMessageBox(wxString::Format(wxT("%f"), dir));

					CTSWithCurrent(myBrng, VBG, dir, spd, BC, speed);

					if (isnan(VBG)) {
						wxString str = _("Unable to compute speed over ground with this tide and boat speed, \n\n Aborting");
						wxMessageBox(str, wxT("Problem"));
					}

					//
					// set up the waypoint
					//
					//

					PlugIn_Waypoint*  newPoint = new PlugIn_Waypoint
					(latN[i], lonN[i], wxT("Circle"), waypointName[i]);
					newPoint->m_IconName = wxT("Circle");
					newPoint->m_MarkDescription = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
					newPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
					newRoute->pWaypointList->Append(newPoint);

					//
					// set up a tidal route to save in the config file
					//

					if (i == 0) {

						interimDateTimeETA = dt;

						ptr.name = waypointName[i];
						ptr.time = sdt;
						ptr.lat = wxString::Format(_T("%8.4f"), latN[0]);
						ptr.lon = wxString::Format(_T("%8.4f"), lonN[0]);
						ptr.guid = newPoint->m_GUID;
						ptr.CTS = wxString::Format(_T("%03.0f"), BC);
						ptr.SMG = wxString::Format(_T("%5.1f"), VBG);
						ptr.distTo = _T("----");
						ptr.brgTo = _T("----");
						ptr.set = wxString::Format(_T("%03.0f"), dir);;
						ptr.rate = wxString::Format(_T("%5.1f"), spd);;
						ptr.icon_name = wxT("Circle");
						tr.m_positionslist.push_back(ptr);
						tr.Start = wxString::Format(wxT("%i"), i);
					}
					else {

						ptr.name = wxString::Format(wxT("%i"), i);
						if (i == 1 && skipleg) {

							longMinutes = (long)(iTime);
							pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
							iPointDT = dt + pointMinuteSpan;
							ptr.time = iPointDT.Format(_T(" %a %d-%b-%Y  %H:%M"));
						}
						else {
							if (!madeETA) {

								longMinutes = (long)(ptrTime);
								pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
								iPointDT = interimDateTimeETA + pointMinuteSpan;
								ptr.time = iPointDT.Format(_T(" %a %d-%b-%Y  %H:%M"));

							}
							else {  // we made an ETA before we got here

								longMinutes = (long)(ptrTime);
								pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
								iPointDT = dt + pointMinuteSpan;
								ptr.time = iPointDT.Format(_T(" %a %d-%b-%Y  %H:%M"));

								interimDateTimeETA = iPointDT;

							}

							//wxMessageBox(wxString::Format(wxT("%i"), longMinutes), wxString::Format(wxT("%i"), i));

						}


						ptr.name = waypointName[i];
						ptr.lat = wxString::Format(_T("%8.4f"), latN[i]);
						ptr.lon = wxString::Format(_T("%8.4f"), lonN[i]);
						ptr.guid = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
						ptr.CTS = wxString::Format(_T("%03.0f"), BC);
						ptr.SMG = wxString::Format(_T("%5.1f"), VBG);
						ptr.distTo = wxString::Format(_T("%5.1f"), ptrDist);
						ptr.brgTo = wxString::Format(_T("%03.0f"), myBrng);
						ptr.set = wxString::Format(_T("%03.0f"), dir);
						ptr.rate = wxString::Format(_T("%5.1f"), spd);
						ptr.icon_name = wxT("Circle");
						tr.m_positionslist.push_back(ptr);

						iDist = 0;
						ptrDist = 0;
						iTime = 0;
						ptrTime = 0;

						madeETA = false;

					}

					//
					// save the GPX file routepoint
					//

					my_point.lat = wxString::Format(wxT("%f"), latN[i]);
					my_point.lon = wxString::Format(wxT("%f"), lonN[i]);
					my_point.routepoint = 1;
					my_point.wpt_num = waypointName[i];
					my_point.name = waypointName[i];
					my_points.push_back(my_point);

					latF = latN[i];
					lonF = lonN[i];

					//
					// we are allowing for two legs to be skipped without the calculation stopping
					// (this is two consecutive legs of 3 minutes duration)
					//

					if (myDist < VBG / 20 && i == 0) {  // test whether first leg needs to be skipped
						fttg = myDist / VBG;
						//wxMessageBox(wxString::Format(wxT("%f"), fttg), wxString::Format(wxT("%i"), i));
						//longMinutes = (long)(fttg * 60);
						iTime = fttg * 60;
						//pointMinuteSpan = wxTimeSpan::Minutes(longMinutes);
						skipleg = true;
						skippedDistance = myDist;
					}

					if (skipleg) {  // previous leg was skipped
						//
						// sailing a leg after the skipped leg
						//
						if (i == 0) {
							//
							//
							iDist = skippedDistance;
							// 
							//
						}

						else {

							if (i == 1) {
								rdHours1 = bitDist2 / lastVBG1;
								ttg = 0.05 - (rdHours1);
								rdMiles = ttg * VBG;

								fttg = (rdHours1)+myDist / VBG;

								if (fttg < 0.05) {

									wxMessageBox(_("Unable to calculate ETA over two legs at this speed. \n\n Aborting"), wxT("Problem"));
									return;
								}
								//pointMinuteSpan = wxTimeSpan::Minutes(fttg*60);

								destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
								iDist = rdMiles;  // skipped first leg so no need for skippedDistance this time
								iTime = (iDist / VBG) * 60;
								skippedDistance = 0;
								skipleg = false;

							}
							else {

								rdHours1 = bitDist1 / lastVBG;
								rdHours2 = bitDist2 / lastVBG1;
								ttg = 0.05 - (rdHours1 + rdHours2);
								rdMiles = ttg * VBG;

								fttg = rdHours1 + rdHours2 + (myDist / VBG);
								if (fttg < 0.05) {

									wxMessageBox(_("Unable to calculate ETA over two legs at this speed. \n\n Aborting"), wxT("Problem"));
									return;
								}

								destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
								iDist = skippedDistance + rdMiles;
								iTime = (iDist / VBG) * 60;
								skippedDistance = 0;
								skipleg = false;

							}

						}

					}
					else {
						//
						// Not sailing a skipped leg
						//

						if (i == 1) {
							if (skipleg) {
								rdHours1 = bitDist2 / lastVBG1;
								ttg = 0.05 - rdHours1;
								rdMiles = ttg * VBG;

								fttg = rdHours1 + (myDist / VBG);
								destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);

								if (fttg < 0.05) {

									skipleg = true; // two skips and you are out ... first and second legs
									wxMessageBox(_("Unable to calculate ETA over two legs at this speed. \n\n Aborting"), wxT("Problem"));
									return;

								}
								else {
									pointMinuteSpan = wxTimeSpan::Minutes(fttg * 60);
									iDist = skippedDistance + rdMiles;
									iTime = (iDist / VBG) * 60;
									skippedDistance = 0;
									skipleg = false;
								}

							}
							else {
								rdHours1 = bitDist1 / lastVBG;
								ttg = 0.05 - rdHours1;
								rdMiles = ttg * VBG;

								fttg = rdHours1 + (myDist / VBG);
								destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);


								if (fttg < 0.05) {
									skippedDistance = rdMiles;
									//iTime = (skippedDistance / VBG) * 60;
									//iDist = skippedDistance;
									skipleg = true;

								}
								else {
									iDist = skippedDistance + rdMiles;
									iTime = (iDist / VBG) * 60;
									skippedDistance = 0;
									skipleg = false;
								}

							}

						}
						else {
							//
							// i !=1
							//
							if (i == 0) {

								rdMiles = VBG / 20;

								fttg = myDist / VBG;
								destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
								iDist = rdMiles;
								iTime = (iDist / VBG) * 60;
							}
							else {
								rdHours1 = bitDist1 / lastVBG;
								ttg = 0.05 - rdHours1;
								rdMiles = ttg * VBG;

								fttg = rdHours1 + (myDist / VBG);
								destLoxodrome(latF, lonF, myBrng, rdMiles, &lati, &loni);
								iDist = rdMiles;
								iTime = (iDist / VBG) * 60;

								if (fttg < 0.05) {
									skippedDistance = rdMiles;
									skipleg = true;

								}
								else {
									iDist = skippedDistance + rdMiles;
									iTime = (iDist / VBG) * 60;

									skippedDistance = 0;
									skipleg = false;
								}

							}
						}

					}

					tdist += iDist;
					ptrDist = iDist;
					ptrTime = iTime;

					//wxMessageBox(wxString::Format(wxT("%f"), skippedDistance), wxString::Format(wxT("%i"), i));

					while (fttg > 0.05 && skipleg == false) {

						count3MinuteSteps++;

						latF = lati;
						lonF = loni;

						DistanceBearingMercator(latN[i + 1], lonN[i + 1], lati, loni, &myD, &myB);

						myDist = myD;

						bitDist1 = myD;

						if (count3MinuteSteps == 20) {

							epNumber++;
							epName = wxString::Format(wxT("%i"), epNumber);
							PlugIn_Waypoint*  epPoint = new PlugIn_Waypoint
							(lati, loni, wxT("Triangle"), (_T("EP") + epName));
							epPoint->m_IconName = wxT("Triangle");
							epPoint->m_MarkDescription = dt.Format(_T("%a %d-%b-%Y  %H:%M"));
							epPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
							newRoute->pWaypointList->Append(epPoint);

							// print mid points for the GPX file
							my_point.lat = wxString::Format(wxT("%f"), lati);
							my_point.lon = wxString::Format(wxT("%f"), loni);						
							my_point.routepoint = 0;
							my_point.wpt_num = _T("EP") + wxString::Format(_T("%i"), epNumber);
							my_point.name = _T("EP") + wxString::Format(_T("%i"), epNumber);
							my_points.push_back(my_point);

							//tc_index++;
							count3MinuteSteps = 0;
							dt.Add(HourSpan);
							ddt = dt.Format(_T("%a %d-%b-%Y  %H:%M"));
							gdt = dt + MinuteSpan;
							m_bGrib = GetGribSpdDir(gdt, latF, lonF, spd, dir);
							//wxMessageBox(wxString::Format(wxT("%f"), dir));
							if (!m_bGrib) {
								wxMessageBox(_("Route start date is not compatible with this Grib \n Or Grib is not available for part of the route"));
								return;
							}
							CTSWithCurrent(myB, VBG, dir, spd, BC, speed);

							ptr.name = _T("EP") + epName;
							ptr.lat = wxString::Format(_T("%8.4f"), lati);
							ptr.lon = wxString::Format(_T("%8.4f"), loni);
							ptr.time = ddt;
							ptr.guid = epPoint->m_GUID;
							ptr.distTo = wxString::Format(_T("%.1f"), ptrDist);
							ptr.brgTo = wxString::Format(_T("%03.0f"), myBrng);
							ptr.CTS = wxString::Format(_T("%03.0f"), BC);
							ptr.SMG = wxString::Format(_T("%5.1f"), VBG);
							ptr.set = wxString::Format(_T("%03.0f"), dir);
							ptr.rate = wxString::Format(_T("%5.1f"), spd);
							ptr.icon_name = wxT("Triangle");

							tr.m_positionslist.push_back(ptr);

							iDist = 0;
							iTime = 0;
							ptrDist = 0;
							ptrTime = 0;
							madeETA = true;

							if (isnan(VBG)) {

								//wxString str2 = wxString::Format(wxT("%i"), tc_index);
								//wxMessageBox(str2, wxT("Problem"));
								wxString str = _("Unable to compute speed over ground with this tide and boat speed, \n\n Aborting");
								wxMessageBox(str, wxT("Problem"));
								return;
							}
						}

						destLoxodrome(latF, lonF, myB, VBG / 20, &lati, &loni);
						// for distances travelled
						myDI = VBG / 20;
						//DistanceBearingMercator(latF, lonF, lati, loni, &myDI, &myBI);


						fttg = myD / VBG;

						double t;
						if (fttg < 0.05) {
							tdist += myD;
							ptrDist += myD;
							t = fttg * 60;
						}
						else {
							tdist += myDI;
							ptrDist += myDI;
							t = 3;
						}

						ptrTime += t;

						lastVBG = VBG;  // VBG for the next ETA point		

					}

				}

				// End of new logic
				// print the last routepoint
				my_point.lat = wxString::Format(wxT("%f"), latN[i]);
				my_point.lon = wxString::Format(wxT("%f"), lonN[i]);
				my_point.routepoint = 1;
				my_point.wpt_num = waypointName[n];
				my_point.name = waypointName[n];
				my_points.push_back(my_point);
				//
				PlugIn_Waypoint*  endPoint = new PlugIn_Waypoint
				(latN[n], lonN[n], wxT("Circle"), tr.End);


				double minutesToEnd;

				minutesToEnd = (ptrDist / VBG) * 60;

				dt.Add(wxTimeSpan::Minutes((long)minutesToEnd));
				ddt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));

				endPoint->m_IconName = wxT("Circle");
				endPoint->m_MarkName = waypointName[n];
				endPoint->m_MarkDescription = ddt;
				endPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));

				newRoute->pWaypointList->Append(endPoint);

				tr.EndTime = ddt;

				dtEnd = dt;
				trTime = dtEnd - dtStart;
				trTimeHours = (double)trTime.GetMinutes() / 60;
				tr.Time = wxString::Format(_("%.1f"), trTimeHours);

				tdist += iDist;
				tr.Distance = wxString::Format(_("%.1f"), tdist);

				ptr.name = waypointName[i];
				ptr.lat = wxString::Format(_T("%8.4f"), latN[n]);
				ptr.lon = wxString::Format(_T("%8.4f"), lonN[n]);
				ptr.time = ddt;
				ptr.guid = endPoint->m_GUID;
				ptr.set = wxString::Format(_T("%03.0f"), dir);
				ptr.rate = wxString::Format(_T("%5.1f"), spd);
				ptr.CTS = _T("----");
				ptr.SMG = wxString::Format(_T("%5.1f"), VBG);
				ptr.distTo = wxString::Format(_T("%.1f"), ptrDist);
				ptr.brgTo = wxString::Format(_T("%03.0f"), myBrng);
				ptr.icon_name = wxT("Circle");

				tr.m_positionslist.push_back(ptr);
				tr.End = wxString::Format(wxT("%i"), i);
				tr.Type = wxT("ETA");
				m_TidalRoutes.push_back(tr);

				//AddPlugInRoute(newRoute); // add the route to OpenCPN routes and display the route on the chart			

				SaveXML(m_default_configuration_path); // add the route and extra detail (times, CTS etc) to the configuration file

				m_ConfigurationDialog.m_lRoutes->Append(tr.Name);
				m_ConfigurationDialog.Refresh();
				GetParent()->Refresh();

				for (std::vector<Position>::iterator itOut = my_points.begin(); itOut != my_points.end(); itOut++) {
					//wxMessageBox((*it).lat, _T("*it.lat"));

					double value, value1;
					if (!(*itOut).lat.ToDouble(&value)) { /* error! */ }
					lati = value;
					if (!(*itOut).lon.ToDouble(&value1)) { /* error! */ }
					loni = value1;

					if ((*itOut).routepoint == 1) {
						if (write_file) { Addpoint(Route, wxString::Format(wxT("%f"), lati), wxString::Format(wxT("%f"), loni), (*itOut).name, wxT("Diamond"), _T("WPT")); }
					}
					else {
						if ((*itOut).routepoint == 0) {
							if (write_file) { Addpoint(Route, wxString::Format(wxT("%f"), lati), wxString::Format(wxT("%f"), loni), (*itOut).name, wxT("Triangle"), _T("WPT")); }
						}
					}

				}

				my_points.clear();
				break;
			}

			default:
			{            // Note the colon, not a semicolon
				cout << "Error, bad input, quitting\n";
				break;
			}
			}

			if (write_file) {

				TiXmlElement * Extensions = new TiXmlElement("extensions");

				TiXmlElement * StartN = new TiXmlElement("opencpn:start");
				TiXmlText * text5 = new TiXmlText(waypointName[0].ToUTF8());
				Extensions->LinkEndChild(StartN);
				StartN->LinkEndChild(text5);

				TiXmlElement * EndN = new TiXmlElement("opencpn:end");
				TiXmlText * text6 = new TiXmlText(waypointName[n].ToUTF8());
				Extensions->LinkEndChild(EndN);
				EndN->LinkEndChild(text6);

				Route->LinkEndChild(Extensions);

				root->LinkEndChild(Route);
				// check if string ends with .gpx or .GPX
				if (!wxFileExists(s)) {
					//s = s + _T(".gpx");
				}
				wxCharBuffer buffer = s.ToUTF8();
				if (dbg) std::cout << buffer.data() << std::endl;
				doc.SaveFile(buffer.data());
			}

			//end of if no error occured

			if (error_occured == true) {
				wxLogMessage(_("Error in calculation. Please check input!"));
				wxMessageBox(_("Error in calculation. Please check input!"));
			}
		}
		GetParent()->Refresh();
		pPlugIn->m_potidalrouteDialog->Show();
		
	}
	wxMessageBox(_("ETA Routes have been calculated!"));
}


bool otidalrouteUIDialog::OpenXML(bool gotGPXFile)
{
	Position my_position;

	my_positions.clear();

	if (!gotGPXFile) {

		int response = wxID_CANCEL;

		wxArrayString file_array;
		wxString filename;
		wxFileDialog openDialog(this, _("Import GPX Route file"), m_gpx_path, wxT(""),
			wxT("GPX files (*.gpx)|*.gpx|All files (*.*)|*.*"),
			wxFD_OPEN | wxFD_MULTIPLE);
		response = openDialog.ShowModal();

		if (response == wxID_OK) {
			openDialog.GetPaths(file_array);

			//    Record the currently selected directory for later use
			if (file_array.GetCount()) {
				wxFileName fn(file_array[0]);
				filename = file_array[0];
				m_gpx_path = fn.GetPath();
				rawGPXFile = filename;
				gotMyGPXFile = true;
			}

		}
		else if (response == wxID_CANCEL) {
			gotMyGPXFile = false;
			return false;
		}

	}

	TiXmlDocument doc;
	wxString error;
	wxProgressDialog *progressdialog = NULL;


	if (!doc.LoadFile(rawGPXFile.mb_str())) {
		FAIL(_("Failed to load file: ") + rawGPXFile);
	}
	else {
		TiXmlElement *root = doc.RootElement();
		if (!strcmp(root->Value(), "rte"))
			FAIL(_("rte Invalid xml file"));

		int count = 0;
		for (TiXmlElement* e = root->FirstChildElement(); e; e = e->NextSiblingElement())
			count++;

		int i = 0;
		for (TiXmlElement* e = root->FirstChildElement(); e; e = e->NextSiblingElement(), i++) {
			if (progressdialog) {
				if (!progressdialog->Update(i))
					return true;
			}
			else {
				if (1) {
					progressdialog = new wxProgressDialog(
						_("Route"), _("Loading"), count, this,
						wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME);
				}
			}

			wxString rte_name;
			
			for (TiXmlElement* f = e->FirstChildElement(); f; f = f->NextSiblingElement()) {
				if (!strcmp(f->Value(), "rtept")) {
					wxString rte_lat = wxString::FromUTF8(f->Attribute("lat"));
					wxString rte_lon = wxString::FromUTF8(f->Attribute("lon"));
					for (TiXmlElement* g = f->FirstChildElement(); g; g = g->NextSiblingElement()) {

						if (!strcmp(g->Value(), "name")) {
							rte_name = wxString::FromUTF8(g->GetText());
							my_position.wpt_num = rte_name;
						}
					}

					my_position.lat = rte_lat;
					my_position.lon = rte_lon;
					
					my_positions.push_back(my_position);
				}  	
			}

		}
	}

	delete progressdialog;
	return true;

failed:
	delete progressdialog;

	wxMessageDialog mdlg(this, error, _("ETA"), wxOK | wxICON_ERROR);
	mdlg.ShowModal();

	return false;
}



void otidalrouteUIDialog::Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon, wxString ptname, wxString ptsym, wxString pttype) {
	//add point
	TiXmlElement * RoutePoint = new TiXmlElement("rtept");
	RoutePoint->SetAttribute("lat", ptlat.mb_str());
	RoutePoint->SetAttribute("lon", ptlon.mb_str());


	TiXmlElement * Name = new TiXmlElement("name");
	TiXmlText * text = new TiXmlText(ptname.mb_str());
	RoutePoint->LinkEndChild(Name);
	Name->LinkEndChild(text);

	TiXmlElement * Symbol = new TiXmlElement("sym");
	TiXmlText * text1 = new TiXmlText(ptsym.mb_str());
	RoutePoint->LinkEndChild(Symbol);
	Symbol->LinkEndChild(text1);

	TiXmlElement * Type = new TiXmlElement("type");
	TiXmlText * text2 = new TiXmlText(pttype.mb_str());
	RoutePoint->LinkEndChild(Type);
	Type->LinkEndChild(text2);

	Route->LinkEndChild(RoutePoint);
	//done adding point
}


bool otidalrouteUIDialog::GetGribSpdDir(wxDateTime dt, double lat, double lon, double &spd, double &dir)
{	
	wxDateTime dtime = dt;
	
	pPlugIn->m_grib_lat = lat;
	pPlugIn->m_grib_lon = lon;
	RequestGrib(dtime);
	if (pPlugIn->m_bGribValid){
		spd = pPlugIn->m_tr_spd;
		dir = pPlugIn->m_tr_dir;
		return true;
	}
	else {		
		return false;
	}

}


int otidalrouteUIDialog::GetRandomNumber(int range_min, int range_max)
{
	long u = (long)wxRound(((double)rand() / ((double)(RAND_MAX)+1) * (range_max - range_min)) + range_min);
	return (int)u;
}


/* C   - Sea Current Direction over ground
VC  - Velocity of Current

provisions to compute boat movement over ground

BG  - boat direction over ground
BGV - boat speed over ground (gps velocity)  
*/


void otidalrouteUIDialog::OverGround(double B, double VB, double C, double VC, double &BG, double &VBG)
{
	if (VC == 0) { // short-cut if no currents
		BG = B, VBG = VB;
		return;
	}

	double Cx = VC * cos(deg2rad(C));
	double Cy = VC * sin(deg2rad(C));
	double BGx = VB * cos(deg2rad(B)) + Cx;
	double BGy = VB * sin(deg2rad(B)) + Cy;
	BG = rad2deg(atan2(BGy, BGx));
	VBG = distance(BGx, BGy);
}

double otidalrouteUIDialog::AttributeDouble(TiXmlElement *e, const char *name, double def)
{
	const char *attr = e->Attribute(name);
	if (!attr)
		return def;
	char *end;
	double d = strtod(attr, &end);
	if (end == attr)
		return def;
	return d;
}

bool otidalrouteUIDialog::OpenXML(wxString filename, bool reportfailure)
{
	Position pos;
	list<Position> m_pos;

	TiXmlDocument doc;
	wxString error;

	wxFileName fn(filename);

	SetTitle(_("oTidalRoute"));

	wxProgressDialog *progressdialog = NULL;
	wxDateTime start = wxDateTime::UNow();

	if (!doc.LoadFile(filename.mb_str()))
		FAIL(_("Failed to load file."));
	else {
		TiXmlHandle root(doc.RootElement());

		if (strcmp(root.Element()->Value(), "OpenCPNotidalrouteConfiguration"))
			FAIL(_("Invalid xml file"));

		Positions.clear();

		int count = 0;
		for (TiXmlElement* e = root.FirstChild().Element(); e; e = e->NextSiblingElement())
			count++;

		int i = 0;
		for (TiXmlElement* e = root.FirstChild().Element(); e; e = e->NextSiblingElement(), i++) {
			if (progressdialog) {
				if (!progressdialog->Update(i))
					return true;
			}
			else {
				wxDateTime now = wxDateTime::UNow();
				/* if it's going to take more than a half second, show a progress dialog */
				if ((now - start).GetMilliseconds() > 250 && i < count / 2) {
					progressdialog = new wxProgressDialog(
						_("Load"), _("otidalroute"), count, this,
						wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME);
				}
			}

			if (!strcmp(e->Value(), "Position")) {
				wxString name = wxString::FromUTF8(e->Attribute("Name"));
				double lat = AttributeDouble(e, "Latitude", NAN);
				double lon = AttributeDouble(e, "Longitude", NAN);

				for (std::vector<RouteMapPosition>::iterator it = Positions.begin();
					it != Positions.end(); it++) {
					if ((*it).Name == name) {
						static bool warnonce = true;
						if (warnonce) {
							warnonce = false;
							wxMessageDialog mdlg(this, _("File contains duplicate position name, discarding\n"),
								_("otidalroute"), wxOK | wxICON_WARNING);
							mdlg.ShowModal();
						}

						goto skipadd;
					}
				}
				

			skipadd:;

			}

			else				

				if (!strcmp(e->Value(), "TidalRoute")) {
					TidalRoute tr;
					m_pos.clear();
					wxString nm = wxString::FromUTF8(e->Attribute("Name"));
					wxString tp = wxString::FromUTF8(e->Attribute("Type"));
					wxString st = wxString::FromUTF8(e->Attribute("Start"));
					wxString en = wxString::FromUTF8(e->Attribute("End"));
					wxString tm = wxString::FromUTF8(e->Attribute("Time"));
					wxString tms = wxString::FromUTF8(e->Attribute("StartTime"));
					wxString tme = wxString::FromUTF8(e->Attribute("EndTime"));
					wxString dn = wxString::FromUTF8(e->Attribute("Distance"));
					tr.Name = nm;
					tr.Type = tp;
					tr.Start = st;
					tr.End = en;
					tr.Time = tm;
					tr.StartTime = tms;
					tr.EndTime = tme;
					tr.Distance = dn;

					for (TiXmlElement* f = e->FirstChildElement(); f; f = f->NextSiblingElement()) {
						
						if (!strcmp(f->Value(), "Route")) {
							pos.name = wxString::FromUTF8(f->Attribute("Waypoint"));
							pos.lat = wxString::FromUTF8(f->Attribute("Latitude"));
							pos.lon = wxString::FromUTF8(f->Attribute("Longitude"));
							pos.time = wxString::FromUTF8(f->Attribute("ETD"));
							pos.guid = wxString::FromUTF8(f->Attribute("GUID"));
							pos.CTS = wxString::FromUTF8(f->Attribute("CTS"));
							pos.SMG = wxString::FromUTF8(f->Attribute("SMG"));
							pos.distTo = wxString::FromUTF8(f->Attribute("Dist"));
							pos.brgTo = wxString::FromUTF8(f->Attribute("Brng"));
							pos.set = wxString::FromUTF8(f->Attribute("Set"));
							pos.rate = wxString::FromUTF8(f->Attribute("Rate"));
							pos.icon_name = wxString::FromUTF8(f->Attribute("icon_name"));

							m_pos.push_back(pos);
							
						}  																
					}
					tr.m_positionslist = m_pos;
					AddTidalRoute(tr);
				}
		

			else
				FAIL(_("Unrecognized xml node"));
		}
	}

	delete progressdialog;
	return true;
failed:

	if (reportfailure) {
		wxMessageDialog mdlg(this, error, _("otidalroute"), wxOK | wxICON_ERROR);
		mdlg.ShowModal();
	}
	return false;
	
}

void otidalrouteUIDialog::SaveXML(wxString filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
	doc.LinkEndChild(decl);

	TiXmlElement * root = new TiXmlElement("OpenCPNotidalrouteConfiguration");
	doc.LinkEndChild(root);

	char version[24];
	sprintf(version, "%d.%d", PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR);
	root->SetAttribute("version", version);
	root->SetAttribute("creator", "Opencpn otidalroute plugin");

	for (std::vector<RouteMapPosition>::iterator it = Positions.begin();
		it != Positions.end(); it++) { 
		TiXmlElement *c = new TiXmlElement("Position");

		c->SetAttribute("Name", (*it).Name.mb_str());
		c->SetAttribute("Latitude", wxString::Format(_T("%.5f"), (*it).lat).mb_str());
		c->SetAttribute("Longitude", wxString::Format(_T("%.5f"), (*it).lon).mb_str());

		root->LinkEndChild(c);
	}
	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {

			TiXmlElement * TidalRoute = new TiXmlElement("TidalRoute");
			TidalRoute->SetAttribute("Name", (*it).Name);
			TidalRoute->SetAttribute("Type", (*it).Type);
			TidalRoute->SetAttribute("Start", (*it).Start);
			TidalRoute->SetAttribute("End", (*it).End);
			TidalRoute->SetAttribute("Time", (*it).Time);
			TidalRoute->SetAttribute("StartTime", (*it).StartTime);
			TidalRoute->SetAttribute("EndTime", (*it).EndTime);
			TidalRoute->SetAttribute("Distance", (*it).Distance);

			for (std::list<Position>::iterator itp = (*it).m_positionslist.begin();
				itp != (*it).m_positionslist.end(); itp++) {
				TiXmlElement *cp = new TiXmlElement("Route");

				cp->SetAttribute("Waypoint", (*itp).name);
				cp->SetAttribute("Latitude", (*itp).lat);
				cp->SetAttribute("Longitude", (*itp).lon);
				cp->SetAttribute("ETD", (*itp).time);
				cp->SetAttribute("GUID", (*itp).guid);
				cp->SetAttribute("CTS", (*itp).CTS);
				cp->SetAttribute("SMG", (*itp).SMG);
				cp->SetAttribute("Dist", (*itp).distTo);
				cp->SetAttribute("Brng", (*itp).brgTo);
				cp->SetAttribute("Set", (*itp).set);
				cp->SetAttribute("Rate", (*itp).rate);
				cp->SetAttribute("icon_name", (*itp).icon_name);
		
				TidalRoute->LinkEndChild(cp);
			}

		root->LinkEndChild(TidalRoute);
	}
	/*
	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		TiXmlElement *config = new TiXmlElement("Configuration");

		config->SetAttribute("Start", (*it).Start.mb_str());
		config->SetAttribute("End", (*it).End.mb_str());

		root->LinkEndChild(config);
	}
	*/
	if (!doc.SaveFile(filename.mb_str())) {
		wxMessageDialog mdlg(this, _("Failed to save xml file: ") + filename,
			_("otidalroute"), wxOK | wxICON_ERROR);
		mdlg.ShowModal();
	}
}




enum { WIND, CURRENT };

