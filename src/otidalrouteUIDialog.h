/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  otidalroute Plugin Friends
 * Author:   David Register, Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 */

#ifndef __otidalrouteUIDIALOG_H__
#define __otidalrouteUIDIALOG_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif  // precompiled headers

#include <wx/fileconf.h>
#include <wx/glcanvas.h>

#include "otidalrouteUIDialogBase.h"
#include "routeprop.h"
#include "NavFunc.h"

#include <wx/progdlg.h>
#include <list>
#include <vector>
#include "GribRecordSet.h"
#include "tcmgr.h"
#include "wx/dateevt.h"
#include "wx/stattext.h"
#include "ocpn_plugin.h"
#include "wx/dialog.h"
#include <wx/calctrl.h>
#include "wx/window.h"
#include <wx/colordlg.h>
#include <wx/event.h>
#include "json/json.h"
#include "tinyxml.h"
#include <wx/scrolwin.h>
#include <wx/datetime.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <wx/listctrl.h>
#include "tableroutes.h"

/* XPM */
static const char* eye[] = {"20 20 7 1",
                            ". c none",
                            "# c #000000",
                            "a c #333333",
                            "b c #666666",
                            "c c #999999",
                            "d c #cccccc",
                            "e c #ffffff",
                            "....................",
                            "....................",
                            "....................",
                            "....................",
                            ".......######.......",
                            ".....#aabccb#a#.....",
                            "....#deeeddeebcb#...",
                            "..#aeeeec##aceaec#..",
                            ".#bedaeee####dbcec#.",
                            "#aeedbdabc###bcceea#",
                            ".#bedad######abcec#.",
                            "..#be#d######dadb#..",
                            "...#abac####abba#...",
                            ".....##acbaca##.....",
                            ".......######.......",
                            "....................",
                            "....................",
                            "....................",
                            "....................",
                            "...................."};

using namespace std;

#ifndef PI
#define PI 3.1415926535897931160E0 /* pi */
#endif

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif

#define RT_RCDATA2 MAKEINTRESOURCE(999)

/* Maximum value that can be returned by the rand function. */
#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

#define distance(X, Y) \
  sqrt((X) * (X) +     \
       (Y) * (Y))  // much faster than hypot#define distance(X, Y) sqrt((X)*(X)
                   // + (Y)*(Y)) // much faster than hypot

class otidalrouteOverlayFactory;
class PlugIn_ViewPort;
class PositionRecordSet;

class wxFileConfig;
class otidalroute_pi;
class wxGraphicsContext;
class routeprop;
class TableRoutes;
class ConfigurationDialog;
class NewPositionDialog;

class Position {
public:
  wxString lat, lon, wpt_num;
  wxString name;
  wxString guid;
  wxString time;
  wxString etd;
  wxString CTS;
  wxString SMG;
  wxString distTo;
  wxString brgTo;
  wxString set;
  wxString rate;
  wxString icon_name;
  bool show_name;
  int routepoint;
};

struct RouteMapPosition {
  RouteMapPosition(wxString n, double lat0, double lon0)
      : Name(n), lat(lat0), lon(lon0) {}

public:
  wxString Name;
  double lat, lon;
};

struct Arrow {
  wxDateTime m_dt;
  double m_dir;
  double m_force;
  double m_lat;
  double m_lon;
  double m_cts;
  double m_tforce;
};

struct TotalTideArrow {
  double m_dir;
  double m_force;
};

class TidalRoute {
public:
  wxString Name, Type, Start, StartTime, End, EndTime, Time, Distance, m_GUID;

  list<Position> m_positionslist;
};

static const wxString column_names[] = {
    _T(""),        _("Start"), _("Start Time"), _("End"),
    _("End Time"), _("Time"),  _("Distance")  //,
};

class otidalrouteUIDialog : public otidalrouteUIDialogBase {
public:
  otidalrouteUIDialog(wxWindow* parent, otidalroute_pi* ppi);
  ~otidalrouteUIDialog();

  enum { POSITION_NAME = 0, POSITION_LAT, POSITION_LON };

  enum {
    VISIBLE = 0,
    START,
    STARTTIME,
    END,
    ENDTIME,
    TIME,
    DISTANCE,
    NUM_COLS
  };

  long columns[NUM_COLS];

  void OpenFile(bool newestFile = false);

  void SetCursorLatLon(double lat, double lon);

  void SetViewPort(PlugIn_ViewPort* vp);
  PlugIn_ViewPort* vp;

  // int round(double c);

  bool m_bUseRate;
  bool m_bUseDirection;
  bool m_bUseFillColour;

  wxString myUseColour[5];

  wxDateTime m_dtNow;
  double m_dInterval;

  bool onNext;
  bool onPrev;

  wxString m_FolderSelected;
  int m_IntervalSelected;

  time_t myCurrentTime;

  wxString MakeDateTimeLabel(wxDateTime myDateTime);
  void OnInformation(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnShowRouteTable();
  void GetTable(wxString myRoute);
  void GetTides(wxString myRoute);
  void AddChartRoute(wxString myRoute);
  void AddTidalRoute(TidalRoute tr);

  void RequestGrib(wxDateTime time);
  virtual void Lock() { routemutex.Lock(); }
  virtual void Unlock() { routemutex.Unlock(); }
  void OverGround(double B, double VB, double C, double VC, double& BG,
                  double& VBG);
  bool OpenXML(wxString filename, bool reportfailure);
  void SaveXML(wxString filename);

  double AttributeDouble(TiXmlElement* e, const char* name, double def);
  vector<RouteMapPosition> Positions;
  wxString m_default_configuration_path;
  list<Arrow> m_arrowList;
  list<Arrow> m_cList;
  list<TotalTideArrow> m_totaltideList;
  list<TidalRoute> m_TidalRoutes;
  bool b_showTidalArrow;
  RouteProp* routetable;
  wxDateTime m_GribTimelineTime;
  ConfigurationDialog m_ConfigurationDialog;

  vector<Position> my_positions;
  vector<Position> my_points;

  wxString rte_start;
  wxString rte_end;

  bool OpenXML(bool gotGPXFile);
  bool gotMyGPXFile;
  wxString rawGPXFile;
  void Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon,
                wxString ptname, wxString ptsym, wxString pttype);

protected:
  bool m_bNeedsGrib;

private:
  wxMutex routemutex;

  void OnClose(wxCloseEvent& event);
  void OnMove(wxMoveEvent& event);
  void OnSize(wxSizeEvent& event);

  void OnSummary(wxCommandEvent& event);
  void OnShowTables(wxCommandEvent& event);

  void OnDeleteAllRoutes(wxCommandEvent& event);
  void CalcDR(wxCommandEvent& event, bool write_file, int Pattern);
  void CalcETA(wxCommandEvent& event, bool write_file, int Pattern);

  void DRCalculate(wxCommandEvent& event);
  void ETACalculate(wxCommandEvent& event);

  bool GetGribSpdDir(wxDateTime dt, double lat, double lon, double& spd,
                     double& dir);

  int GetRandomNumber(int range_min, int range_max);

  //    Data
  wxWindow* pParent;
  otidalroute_pi* pPlugIn;

  PlugIn_ViewPort* m_vp;

  double m_cursor_lat, m_cursor_lon;
  wxString g_SData_Locn;
  TCMgr* ptcmgr;
  wxString* pTC_Dir;

  int m_corr_mins;
  wxString m_stz;
  int m_t_graphday_00_at_station;
  wxDateTime m_graphday;
  int m_plot_y_offset;

  bool isNowButton;
  wxTimeSpan myTimeOfDay;
  bool btc_valid;

  bool error_found;
  bool dbg;
  wxString m_gpx_path;

  wxString waypointName[200];
  Plugin_WaypointExList* myList;
  bool m_bUsingFollow;
  unique_ptr<PlugIn_Route_Ex> thisRoute;
  vector<PlugIn_Waypoint_Ex*> theWaypoints;
  int countRoutePoints;
  int nextRoutePointIndex;
};

class GetRouteDialog : public wxDialog {
public:
  GetRouteDialog(wxWindow* parent, wxWindowID id, const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_DIALOG_STYLE);

  wxListView* dialogText;

private:
};

#endif
