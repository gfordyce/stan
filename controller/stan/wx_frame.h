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
class wxFilmstripCtrl;
class wxFilmstripEvent;

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string data_path, std::string path);

    /*
     * Application commands
     */
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    /**
     * Frame commands
     */
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnPasteAll(wxCommandEvent& event);

    void OnNextFrame(wxCommandEvent& event);
    void OnPrevFrame(wxCommandEvent& event);
    void OnCutFrame(wxCommandEvent& event);
    void OnCopyFrame(wxCommandEvent& event);
    void OnPasteFrame(wxCommandEvent& event);
    void OnDupFrame(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnSelectImage(wxCommandEvent& event);
    void OnSelectSound(wxCommandEvent& event);
    void OnThumbNailSelected(wxFilmstripEvent& event);
    void OnTimer(wxTimerEvent& event);

    /**
     * Figure commands
     */
    void OnLoadFigure(wxCommandEvent& event);
    void OnSaveFigure(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);
    void OnLine(wxCommandEvent& event);
    void OnCircle(wxCommandEvent& event);
    void OnSize(wxCommandEvent& event);
    void OnColor(wxCommandEvent& event);
    void OnCutTool(wxCommandEvent& event);
    void OnBreak(wxCommandEvent& event);
    void OnImage(wxCommandEvent& event);
    void OnSelectFigureImage(wxCommandEvent& event);

    /**
     * Make selected figure line weight thinner or thicker.
     */
    void OnThin(wxCommandEvent& event);
    void OnThick(wxCommandEvent& event);

    /**
     * Scale selected figure.
     */
    void OnShrink(wxCommandEvent& event);
    void OnGrow(wxCommandEvent& event);

    /**
     * Rotate
     */
    void OnRotateCW(wxCommandEvent& event);
    void OnRotateCCW(wxCommandEvent& event);

    bool LoadAnimation(char* path);
    bool SaveAnimation(char* path);
    bool LoadFigure(char* path);
    bool SaveFigure(char* path);

    MyCanvas* m_canvas;
    wxToolBar* m_toolbar;
    wxFilmstripCtrl* frameBrowser_;
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
    int get_sel_frame();


    wxStaticText* color_display_;
    std::string path_;
    animation* anim_;
    wxTimer timer_;
    wxImage image_;
    std::string data_path_;
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
    ID_CutFrame,
    ID_CopyFrame,
    ID_PasteFrame,
	ID_DupFrame,
    ID_FrameRate,
    ID_Repeat,
	ID_Play,
    ID_Stop,
	ID_Rate,
    ID_About,
    ID_CutFigure,
    ID_CopyFigure,
    ID_PasteFigure,
    ID_PasteFigureAll,
    ID_Line,
    ID_Circle,
    ID_Select,
    ID_Size,
    ID_Color,
    ID_Cut,
    ID_Break,
    ID_Image,
    ID_LoadFigure,
    ID_SaveFigure,
    ID_SelectImage,
    ID_Thin,
    ID_Thick,
    ID_Shrink,
    ID_Grow,
    ID_CW,
    ID_CCW,
    ID_Background,
    ID_Sound,
    ID_FrameTools,
    ID_FigureTools,
    ID_FRAME_THUMB,
};

#endif  // _WX_FRAME_H
