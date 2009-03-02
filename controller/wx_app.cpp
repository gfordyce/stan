/**
 * render a stick figure
 *
 * stick definition:
 *    line l1 (0, 0), (5, 5) @color=RED
 *    line l2(0, 0), (5, 5), (3, 4), 9, 3) @width=5
 *    circle c1 (5, 5), 3
 *    color=RED
 */

#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "wx/wx.h"

#include "wx_frame.h"
#include "wx_canvas.h"
#include "animation.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Open, MyFrame::OnOpen)
    EVT_MENU(ID_Load, MyFrame::OnLoad)
    EVT_MENU(ID_Quit, MyFrame::OnQuit)
    EVT_MENU(ID_About, MyFrame::OnAbout)
END_EVENT_TABLE()

class MyApp: public wxApp
{
public:
 
    // from wxApp
    virtual bool OnInit();
    virtual int OnExit();
};

bool MyApp::OnInit()
{
    char path[100];

    // Specifying a file to load at the command line is optional
    // and is otherwise done through File->Load in the UI
    sprintf(path, "");
    if (argc > 1)
    {
        wxString s(argv[1]);
        strncpy( path, (const char*)s.mb_str(wxConvUTF8), 100 );
        std::cout << "OnInit: path is " << path << std::endl;
    }

    MyFrame *frame = new MyFrame( _T("Stick'em Up"), wxPoint(50,50), wxSize(640,480), std::string(path) );
    frame->Show(TRUE);
    SetTopWindow(frame);

    return TRUE;
} 

int MyApp::OnExit()
{
    // cleanup
    return 0;
}

IMPLEMENT_APP(MyApp)

// END of this file -----------------------------------------------------------
