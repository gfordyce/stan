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
class wxThumbnailCtrl;

class MyFrame: public wxFrame
{
public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string path);

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnLoad(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnNextFrame(wxCommandEvent& event);
    void OnPrevFrame(wxCommandEvent& event);
    void OnCopyFrame(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLine(wxCommandEvent& event);
    void OnCircle(wxCommandEvent& event);

    bool LoadAnimation(char* path);
    bool SaveAnimation(char* path);
    bool LoadFigure(char* path);
    MyCanvas* m_canvas;
    wxToolBar* m_toolbar;
	wxThumbnailCtrl* frameBrowser_;

private:
    DECLARE_EVENT_TABLE()

    std::string path_;
};

enum {
    ID_Quit= 1,
	ID_New,
    ID_Open,
    ID_Load,
    ID_Save,
    ID_NextFrame,
    ID_PrevFrame,
    ID_CopyFrame,
    ID_About,
    ID_Line,
    ID_Circle,
    ID_Toolbar,
};

#endif  // _WX_FRAME_H
