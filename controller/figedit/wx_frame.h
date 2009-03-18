#ifndef _WX_FRAME_H
#define _WX_FRAME_H     1

/**
 * @file wx_frame.h
 * @brief The frame where our animation is displayed.
 */
#include <iostream>
#include <string>
#include <wx/wx.h>

class MyCanvas;

class MyFrame: public wxFrame
{
public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string path);

    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLine(wxCommandEvent& event);
    void OnCircle(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);
    void OnSize(wxCommandEvent& event);

    bool LoadFigure(char* path);
    bool SaveFigure(char* path);
    MyCanvas* m_canvas;
    wxToolBar* m_toolbar;

private:
    DECLARE_EVENT_TABLE()

    std::string path_;
};

enum {
    ID_Quit= 1,
    ID_Open,
    ID_Save,
    ID_About,
    ID_Line,
    ID_Circle,
    ID_Select,
    ID_Size,
    ID_Toolbar,
};

#endif  // _WX_FRAME_H
