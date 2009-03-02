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
    void OnLoad(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void SetShapeFilePath(std::string path) 
    {
        path_ = path;
        std::cout << "SetShapeFilePath: path is " << path_ << std::endl;
    }

    bool LoadShapes();
    bool LoadFigure(char* path);
    MyCanvas   *m_canvas;

private:
    DECLARE_EVENT_TABLE()

    std::string path_;
};

enum {
    ID_Quit= 1,
    ID_Open,
    ID_Load,
    ID_About,
};

#endif  // _WX_FRAME_H
