/**
 * figedit application
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
    EVT_MENU(ID_New, MyFrame::OnNew)
    EVT_MENU(ID_Open, MyFrame::OnOpen)
    EVT_MENU(ID_Save, MyFrame::OnSave)
    EVT_MENU(ID_Quit, MyFrame::OnQuit)
    EVT_MENU(ID_About, MyFrame::OnAbout)
    EVT_MENU(ID_Select, MyFrame::OnSelect)
    EVT_MENU(ID_Line, MyFrame::OnLine)
    EVT_MENU(ID_Circle, MyFrame::OnCircle)
    EVT_MENU(ID_Size, MyFrame::OnSize)
    EVT_MENU(ID_Color, MyFrame::OnColor)
    EVT_MENU(ID_Cut, MyFrame::OnCut)
    EVT_MENU(ID_Break, MyFrame::OnBreak)
    EVT_MENU(ID_Image, MyFrame::OnImage)
    EVT_MENU(ID_Thin, MyFrame::OnThin)
    EVT_MENU(ID_Thick, MyFrame::OnThick)
    EVT_MENU(ID_Shrink, MyFrame::OnShrink)
    EVT_MENU(ID_Grow, MyFrame::OnGrow)
    EVT_MENU(ID_CW, MyFrame::OnRotateCW)
    EVT_MENU(ID_CCW, MyFrame::OnRotateCCW)
    EVT_MENU(ID_SelectImage, MyFrame::OnSelectImage)
END_EVENT_TABLE()

class MyApp: public wxApp
{
public:
 
    // from wxApp
    virtual bool OnInit();
    virtual int OnExit();
};

IMPLEMENT_APP_CONSOLE(MyApp)

bool MyApp::OnInit()
{
    char path1[100];
    char path2[100];

    // path to data
    sprintf_s(path1, 100, ".");
    if (argc > 1) {
        wxString s(argv[1]);
        strncpy_s( path1, 100, (const char*)s.mb_str(wxConvUTF8), 100 );
        std::cout << "OnInit: data path is " << path1 << std::endl;
    }

    // Specifying a file to load at the command line is optional
    // and is otherwise done through File->Load in the UI
    sprintf_s(path2, 100, "NO FILE");
    if (argc > 2) {
        wxString s(argv[2]);
        strncpy_s( path2, 100, (const char*)s.mb_str(wxConvUTF8), 100 );
        std::cout << "OnInit: default path is " << path2 << std::endl;
    }

    MyFrame *frame = new MyFrame( _T("Stick'em Up"), wxPoint(50,50), wxSize(640,480),
                                  std::string(path1), std::string(path2) );
    frame->Show(TRUE);
    SetTopWindow(frame);

    return TRUE;
} 

int MyApp::OnExit()
{
    // cleanup
    return 0;
}

// END of this file -----------------------------------------------------------
