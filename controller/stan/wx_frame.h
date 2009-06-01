#ifndef _WX_FRAME_H
#define _WX_FRAME_H     1

/**
 * @file wx_frame.h
 * @brief The frame where our animation is displayed.
 */
#include <iostream>
#include <string>
#include <wx/wx.h>

#include "animation.h"

using namespace stan;

class MyCanvas;
class wxThumbnailCtrl;
class wxThumbnailEvent;

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
    void OnNewFrame(wxCommandEvent& event);
    void OnDelFrame(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLine(wxCommandEvent& event);
    void OnCircle(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnImage(wxCommandEvent& event);
    void OnThumbNailSelected(wxThumbnailEvent& event);
    void OnTimer(wxTimerEvent& event);

    bool LoadAnimation(char* path);
    bool SaveAnimation(char* path);
    bool LoadFigure(char* path);
    MyCanvas* m_canvas;
    wxToolBar* m_toolbar;
	wxThumbnailCtrl* frameBrowser_;
    wxSpinCtrl* frameRate_;
    wxCheckBox* repeat_;

private:
    DECLARE_EVENT_TABLE()

    /**
     * Animation API
     */
    frame *select_frame(int index);
    frame* first_frame();
    frame* next_frame();
    frame* prev_frame();


    std::string path_;
    animation* anim_;
    wxTimer timer_;
    wxImage image_;
};

const int TIMER_ID = 1000;

enum {
    ID_Quit= 1,
	ID_New,
    ID_Open,
    ID_Load,
    ID_Save,
    ID_NextFrame,
    ID_PrevFrame,
    ID_CopyFrame,
	ID_NewFrame,
    ID_DelFrame,
    ID_FrameRate,
    ID_Repeat,
	ID_Play,
    ID_Stop,
	ID_Rate,
    ID_About,
    ID_Line,
    ID_Circle,
    ID_Image,
    ID_Toolbar,
    ID_FRAME_THUMB,
};

#endif  // _WX_FRAME_H
