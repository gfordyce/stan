#include <iostream>
#include <fstream>
#include <wx/colordlg.h>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "thumbnaildlg.h"
#include "wx_frame.h"
#include "wx_canvas.h"
#include "wx_render.h"
#include "animation.h"
#include "line.xpm"
#include "circle.xpm"
#include "select.xpm"
#include "size.xpm"
#include "color.xpm"
#include "cut.xpm"
#include "break.xpm"
#include "thin_line.xpm"
#include "thick_line.xpm"
#include "shrink.xpm"
#include "grow.xpm"
#include "rotate_cw.xpm"
#include "rotate_ccw.xpm"

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, std::string data_path, std::string path) :
    wxFrame((wxFrame *)NULL, -1, title, pos, size),
    data_path_(data_path),
    path_(path),
    image_(new wxImage())
{
    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_New, _T("&New...") );
    menuFile->Append( ID_Open, _T("&Open...") );
    menuFile->Append( ID_Save, _T("Save &As...") );
    menuFile->Append( ID_SelectImage, _T("&Select Image...") );
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
    wxBitmap cutBitmap(cut_xpm);
    wxBitmap breakBitmap(break_xpm);
    wxBitmap thinBitmap(thin_xpm);
    wxBitmap thickBitmap(thick_xpm);
    wxBitmap shrinkBitmap(shrink_xpm);
    wxBitmap growBitmap(grow_xpm);
    wxBitmap cwBitmap(cw_xpm);
    wxBitmap ccwBitmap(ccw_xpm);

    // image button with a picture on it
    wxImage image;
    image.LoadFile(wxT("c:\\dev\\stan\\data\\images\\ben2.bmp"));
    image.Rescale(16, 16);
    wxBitmap imageBitmap(image);

	color_display_ = new wxStaticText(m_toolbar, wxID_ANY, _T("         "));
    color_display_->SetOwnBackgroundColour(wxColour(0, 0, 0));

    m_toolbar->AddTool(ID_Select, _T(""), selectBitmap, _("Select tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Size, _T(""), sizeBitmap, _("Size tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Line, _T(""), lineBitmap, _("Line tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Circle, _T(""), circleBitmap, _("Circle tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Cut, _T(""), cutBitmap, _("Cut tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Break, _T(""), breakBitmap, _("Break tool"), wxITEM_RADIO);
    m_toolbar->AddTool(ID_Image, _T(""), imageBitmap, _("Image tool"), wxITEM_RADIO);
   
    m_toolbar->AddSeparator();

    // line thickness tools
    m_toolbar->AddTool(ID_Thin, _T(""), thinBitmap, _("Thinner lines"), wxITEM_NORMAL);
    m_toolbar->AddTool(ID_Thick, _T(""), thickBitmap, _("Thicker lines"), wxITEM_NORMAL);

    // Scaling tools
    m_toolbar->AddTool(ID_Shrink, _T(""), shrinkBitmap, _("Shrink figure"), wxITEM_NORMAL);
    m_toolbar->AddTool(ID_Grow, _T(""), growBitmap, _("Grow figure"), wxITEM_NORMAL);

    // Rotation tools
    m_toolbar->AddTool(ID_CW, _T(""), cwBitmap, _("Rotate clockwise"), wxITEM_NORMAL);
    m_toolbar->AddTool(ID_CCW, _T(""), ccwBitmap, _("Rotate counter-clockwise"), wxITEM_NORMAL);

    // choose a color, display in static
    m_toolbar->AddTool(ID_Color, _T(""), colorBitmap, _("Choose a color"), wxITEM_NORMAL);
    m_toolbar->AddControl(color_display_);

	m_toolbar->Realize();
    SetToolBar(m_toolbar);

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 240 );
    // m_canvas->Connect( ID_Thin, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyCanvas::OnThin) );

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
		std::cout << *fig << std::endl;

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

        // create the image cache
        meta_store* meta = fig->get_meta_store();
        WxRender::init_meta_cache(meta);

        std::cout << "Loaded the figure: " << *fig << std::endl;
        m_canvas->set_figure(fig);
        ret = true;
	}
    ifs.close();

    return ret;
}

bool MyFrame::SaveFigure(char* path)
{
    std::cout << "Saving Figure to: " << path << std::endl;

    figure* fig = m_canvas->get_figure();
    if (fig != NULL) {
		std::cout << *fig << std::endl;

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
    char path[200];
    sprintf_s(path, 200, "%s\\figures", data_path_.c_str());
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("FIG files (*.fig)|*.fig|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(path);
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy_s( path, 100, (const char*)wx_path.mb_str(wxConvUTF8) );
        LoadFigure(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    char path[200];
    sprintf_s(path, 200, "%s\\figures", data_path_.c_str());
    wxString caption = wxT("Save as ?");
    wxString wildcard = wxT("FIG files (*.fig)|*.fig|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(path);
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy_s( path, 100, (const char*)wx_path.mb_str(wxConvUTF8) );
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

void MyFrame::OnCut(wxCommandEvent& event)
{
    std::cout << "Cut tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_CUT);
}

void MyFrame::OnBreak(wxCommandEvent& event)
{
    std::cout << "Break tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_BREAK);
}

void MyFrame::OnSelectImage(wxCommandEvent& event)
{
    char path[200];
    sprintf_s(path, 200, "%s\\images", data_path_.c_str());

    wxThumbnailBrowserDialog dialog(this, wxID_ANY, wxT("Choose an image..."));
    dialog.SetSelection(wxT(path));

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString wx_path = dialog.GetSelection();

        char path[100];
        strcpy_s( path, 100, (const char*)wx_path.mb_str(wxConvUTF8) );

        m_canvas->set_image(std::string(path));
    }
}

void MyFrame::OnImage(wxCommandEvent& event)
{
    std::cout << "Image tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_IMAGE);
}

void MyFrame::OnThin(wxCommandEvent& event)
{
    m_canvas->thinner();
}

void MyFrame::OnThick(wxCommandEvent& event)
{
    m_canvas->thicker();
}

void MyFrame::OnShrink(wxCommandEvent& event)
{
    m_canvas->shrink();
}

void MyFrame::OnGrow(wxCommandEvent& event)
{
    m_canvas->grow();
}

void MyFrame::OnRotateCW(wxCommandEvent& event)
{
    m_canvas->rotateCW();
}

void MyFrame::OnRotateCCW(wxCommandEvent& event)
{
    m_canvas->rotateCCW();
}

// END of this file -----------------------------------------------------------
