#ifndef _WX_FRAME_H
#define _WX_FRAME_H     1

/**
 * @file wx_frame.h
 * @brief The frame where our animation is displayed.
 */
#include <iostream>
#include <string>
#include <wx/wx.h>
#include "colorStaticText.h"

class MyCanvas;

class MyFrame: public wxFrame
{
public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string data_path, std::string path);

    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLine(wxCommandEvent& event);
    void OnCircle(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);
    void OnSize(wxCommandEvent& event);
    void OnColor(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnBreak(wxCommandEvent& event);
    void OnImage(wxCommandEvent& event);
    void OnSelectImage(wxCommandEvent& event);

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

    bool LoadFigure(char* path);
    bool SaveFigure(char* path);
    MyCanvas* m_canvas;
    wxToolBar* m_toolbar;
    wxImage* image_;

private:
    DECLARE_EVENT_TABLE()

    std::string path_;
    std::string data_path_;
    wxStaticText* color_display_;
};

enum {
    ID_Quit= 1,
    ID_New,
    ID_Open,
    ID_Save,
    ID_About,
    ID_Line,
    ID_Circle,
    ID_Select,
    ID_Size,
    ID_Color,
    ID_Cut,
    ID_Break,
    ID_Image,
    ID_SelectImage,
    ID_Thin,
    ID_Thick,
    ID_Shrink,
    ID_Grow,
    ID_CW,
    ID_CCW,
    ID_Toolbar,
};

#endif  // _WX_FRAME_H
