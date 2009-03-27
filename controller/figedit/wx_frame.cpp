#include <iostream>
#include <fstream>
#include <wx/colordlg.h>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "wx_frame.h"
#include "wx_canvas.h"
#include "animation.h"
#include "line.xpm"
#include "circle.xpm"
#include "select.xpm"
#include "size.xpm"
#include "color.xpm"
#include "style.xpm"
#include "break.xpm"

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string path) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    path_(path)
{
    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_New, _T("&New...") );
    menuFile->Append( ID_Open, _T("&Open...") );
    menuFile->Append( ID_Save, _T("Save &As...") );
    menuFile->Append( ID_About, _T("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _T("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );

    SetMenuBar( menuBar );

    // Attempt a toolbar
    m_toolbar = CreateToolBar( wxTB_FLAT|wxTB_VERTICAL, ID_Toolbar );
    m_toolbar->SetToolBitmapSize(wxSize(16, 16));
    wxBitmap lineBitmap(line_xpm);
    wxBitmap circleBitmap(circle_xpm);
    wxBitmap selectBitmap(select_xpm);
    wxBitmap sizeBitmap(size_xpm);
    wxBitmap colorBitmap(color_xpm);
    wxBitmap styleBitmap(style_xpm);
    wxBitmap breakBitmap(break_xpm);
    // wxBitmap lineBitmap(wxT("bitmaps/line.png"), wxBITMAP_TYPE_PNG);
    // wxBitmap lineBitmap(GetBitmapResource(wxT("bitmaps/line.png")));
    //
    color_display_ = new colorStaticText(m_toolbar, wxID_ANY, _T(" "));
    m_toolbar->AddControl(color_display_);

    m_toolbar->AddTool(ID_Select, _T(""), selectBitmap, _("Select tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Size, _T(""), sizeBitmap, _("Size tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Line, _T(""), lineBitmap, _("Line tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Circle, _T(""), circleBitmap, _("Circle tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Style, _T(""), styleBitmap, _("Style tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Break, _T(""), breakBitmap, _("Break tool"), wxITEM_RADIO);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(ID_Color, _T(""), colorBitmap, _("Choose a color"), wxITEM_NORMAL);
    m_toolbar->Realize();
    SetToolBar(m_toolbar);

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 240 );

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);
    // topSizer->Add(m_toolbar, 0, wxEXPAND|wxALL, 0);
    topSizer->Add(m_canvas, 1, wxEXPAND|wxALL, 0);

    CreateStatusBar();
    SetStatusText( _T("Welcome to the Stick'm Up Figure editor") );

    if (path_ != "") {
        LoadFigure(const_cast<char *>(path_.c_str()));
    }
}

bool MyFrame::SaveFigure(char* path)
{
    std::cout << "Saving Figure to: " << path << std::endl;

    figure* fig = m_canvas->get_figure();
    if (fig != NULL) {
        std::ofstream ofs(path);
        assert(ofs.good());
        {
            boost::archive::xml_oarchive oa(ofs);
            oa << boost::serialization::make_nvp("figure", fig);

        }
        ofs.close();
    }
    return true;
}

bool MyFrame::LoadFigure(char *path)
{
    std::cout << "Loading figure from: " << path << std::endl;

    bool ret = false;
    figure* fig;
	std::ifstream ifs(path);
	if (ifs.good())
	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("figure", fig);
        if (fig == NULL) {
            std::cerr << "Error loading " << path << std::endl;
            return false;
        }
        std::cout << "Loaded the figure: " << *fig << std::endl;
        m_canvas->set_figure(fig);
        ret = true;
	}
    ifs.close();

    return ret;
}

void MyFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
    bool doSave = true;
    if (m_canvas->is_dirty()) {
        // ask user if they want to save
        // set doSave to false if they choose Cancel
    }
    if (doSave)
        m_canvas->new_figure();
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("FIG files (*.fig)|*.fig|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy( path, (const char*)wx_path.mb_str(wxConvUTF8) );
        LoadFigure(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Save as ?");
    wxString wildcard = wxT("FIG files (*.fig)|*.fig|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy( path, (const char*)wx_path.mb_str(wxConvUTF8) );
        SaveFigure(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is Stick'em Up, the stick figure animator."),
                 _T("About Stick'em Up"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnLine(wxCommandEvent& event)
{
    std::cout << "Line tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_LINE);
}

void MyFrame::OnCircle(wxCommandEvent& event)
{
    std::cout << "Circle tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_CIRCLE);
}

void MyFrame::OnSelect(wxCommandEvent& event)
{
    std::cout << "Select tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_SELECT);
}

void MyFrame::OnSize(wxCommandEvent& event)
{
    std::cout << "Size tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_SIZE);
}

void MyFrame::OnColor(wxCommandEvent& event)
{
    std::cout << "Color tool." << std::endl;
    // m_canvas->set_mode(MyCanvas::M_COLOR);

    wxColourData data;
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++) {
        wxColour colour(i*16, i*16, i*16);
        data.SetCustomColour(i, colour);
    }
      
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK) {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();
        std::cout << "Changing color to: " << (int)col.Red() << ", " << (int)col.Green() << ", " << (int)col.Blue() << std::endl;
        color_display_->SetOwnBackgroundColour(col);
        color_display_->ClearBackground();
        color_display_->Refresh();
        m_canvas->set_color(col);
    }
}

void MyFrame::OnStyle(wxCommandEvent& event)
{
    std::cout << "Style tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_STYLE);
}

void MyFrame::OnBreak(wxCommandEvent& event)
{
    std::cout << "Break tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_BREAK);
}

// END of this file -----------------------------------------------------------
