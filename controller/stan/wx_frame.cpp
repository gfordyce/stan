#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <wx/spinctrl.h>
#include <wx/colordlg.h>

#include "wx_frame.h"
#include "wx_canvas.h"
#include "wx_render.h"
#include "animation.h"
#include "thumbnaildlg.h"
#include "filmstripctrl.h"

// frame tools
#include "cut.xpm"
#include "copy.xpm"
#include "play.xpm"
#include "stop.xpm"
#include "forward.xpm"
#include "reverse.xpm"
#include "sound.xpm"
#include "image.xpm"

// figure tools
#include "line.xpm"
#include "circle.xpm"
#include "select.xpm"
#include "size.xpm"
#include "color.xpm"
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
    anim_(NULL),
    timer_(this, TIMER_ID),
    image_()
{
    // File menu
    wxMenu* menuFile = new wxMenu;
    menuFile->Append( ID_New, _T("&New...") );
    menuFile->Append( ID_Open, _T("&Open...") );
    menuFile->Append( ID_Save, _T("Save &As...") );
    menuFile->Append( ID_About, _T("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _T("E&xit") );

    // Frame menu
    wxMenu *menuFrame = new wxMenu;
    menuFrame->Append( ID_CutFrame, _T("Cut\tCtrl+X") );
    menuFrame->Append( ID_CopyFrame, _T("&Copy\tCtrl+C") );
    menuFrame->Append( ID_PasteFrame, _T("&Paste\tCtrl+V") );
    menuFrame->AppendSeparator();
    menuFrame->Append( ID_DupFrame, _T("&Duplicate\tCtrl+D") );
    menuFrame->AppendSeparator();
    menuFrame->Append( ID_PrevFrame, _T("&Previous frame\tCtrl+P") );
    menuFrame->Append( ID_NextFrame, _T("&Next frame\tCtrl+N") );
    menuFrame->AppendSeparator();
    menuFrame->Append( ID_Background, _T("&Background...") );
    menuFrame->Append( ID_Sound, _T("&Sound...") );

    // Figure menu
    wxMenu* menuFig = new wxMenu;
    menuFig->Append( wxID_CUT, _T("Cut\tCtrl+X") );
    menuFig->Append( wxID_COPY, _T("Copy\tCtrl+C") );
    menuFig->Append( wxID_PASTE, _T("Paste\tCtrl+V") );
    menuFig->AppendSeparator();
    menuFig->Append( ID_Load, _T("&Load...") );
    menuFig->Append( ID_SelectImage, _T("Select image...") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _T("&File") );
    menuBar->Append( menuFrame, _T("F&rame") );
    menuBar->Append( menuFig, _T("F&igure") );
    SetMenuBar( menuBar );

    // Frame editor toolbar
    wxToolBar* m_toolbar;
    m_toolbar = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL, ID_FrameTools );
    m_toolbar->SetToolBitmapSize(wxSize(16, 16));
    m_toolbar->SetToolPacking(50);

    wxBitmap copyBitmap(copy_xpm);
    m_toolbar->AddTool(ID_DupFrame, _T(""), copyBitmap, _("Create a new frame"), wxITEM_NORMAL);
    
    wxBitmap cutBitmap(cut_xpm);
    m_toolbar->AddTool(ID_CutFrame, _T(""), cutBitmap, _("Delete a frame"), wxITEM_NORMAL);
    
    m_toolbar->AddSeparator();

    wxBitmap reverseBitmap(reverse_xpm);
    m_toolbar->AddTool(ID_PrevFrame, _T(""), reverseBitmap, _("Previous frame"), wxITEM_NORMAL);

    wxBitmap forwardBitmap(forward_xpm);
    m_toolbar->AddTool(ID_NextFrame, _T(""), forwardBitmap, _("Forward frame"), wxITEM_NORMAL);

    wxBitmap stopBitmap(stop_xpm);
    m_toolbar->AddTool(ID_Stop, _T(""), stopBitmap, _("Stop"), wxITEM_NORMAL);

    wxBitmap playBitmap(play_xpm);
    m_toolbar->AddTool(ID_Play, _T(""), playBitmap, _("Play"), wxITEM_NORMAL);

    m_toolbar->AddSeparator();

    wxBitmap imageBitmap(image_xpm);
    m_toolbar->AddTool(ID_Background, _T(""), imageBitmap, _("Background image"), wxITEM_NORMAL);

    wxBitmap soundBitmap(sound_xpm);
    m_toolbar->AddTool(ID_Sound, _T(""), soundBitmap, _("Sound"), wxITEM_NORMAL);

    m_toolbar->AddSeparator();

    wxStaticText* rateText = new wxStaticText(m_toolbar, wxID_ANY, wxT("Frame rate:"));
    m_toolbar->AddControl(rateText);

    frameRate_ = new wxSpinCtrl(m_toolbar, ID_FrameRate, wxT("4"), wxDefaultPosition, wxSize(60, 24), wxSP_ARROW_KEYS, 1, 60, 1);
    m_toolbar->AddControl(frameRate_);
 
    repeat_ = new wxCheckBox(m_toolbar, ID_Repeat, wxT("&Repeat"), wxDefaultPosition, wxDefaultSize);
    repeat_->SetValue(false);
    m_toolbar->AddControl(repeat_);

    m_toolbar->Realize();
    SetToolBar(m_toolbar);

    //
    // Figure editor toolbar
    //
    wxBitmap lineBitmap(line_xpm);
    wxBitmap circleBitmap(circle_xpm);
    wxBitmap selectBitmap(select_xpm);
    wxBitmap sizeBitmap(size_xpm);
    wxBitmap colorBitmap(color_xpm);
    // wxBitmap cutBitmap(cut_xpm);
    wxBitmap breakBitmap(break_xpm);
    wxBitmap thinBitmap(thin_xpm);
    wxBitmap thickBitmap(thick_xpm);
    wxBitmap shrinkBitmap(shrink_xpm);
    wxBitmap growBitmap(grow_xpm);
    wxBitmap cwBitmap(cw_xpm);
    wxBitmap ccwBitmap(ccw_xpm);

    wxToolBar* m_figureTools = new wxToolBar(this, ID_FigureTools, wxDefaultPosition, wxDefaultSize, wxTB_VERTICAL);
    m_figureTools->SetToolBitmapSize(wxSize(16, 16));
    m_figureTools->AddTool(ID_Select, _T(""), selectBitmap, _("Select tool"), wxITEM_RADIO);
    m_figureTools->AddTool(ID_Size, _T(""), sizeBitmap, _("Size tool"), wxITEM_RADIO);
    m_figureTools->AddTool(ID_Line, _T(""), lineBitmap, _("Line tool"), wxITEM_RADIO);
    m_figureTools->AddTool(ID_Circle, _T(""), circleBitmap, _("Circle tool"), wxITEM_RADIO);
    m_figureTools->AddTool(ID_Cut, _T(""), cutBitmap, _("Cut tool"), wxITEM_RADIO);
    m_figureTools->AddTool(ID_Break, _T(""), breakBitmap, _("Break tool"), wxITEM_RADIO);
    m_figureTools->AddTool(ID_Image, _T(""), imageBitmap, _("Image tool"), wxITEM_RADIO);

    m_toolbar->AddSeparator();

    // line thickness tools
    m_figureTools->AddTool(ID_Thin, _T(""), thinBitmap, _("Thinner lines"), wxITEM_NORMAL);
    m_figureTools->AddTool(ID_Thick, _T(""), thickBitmap, _("Thicker lines"), wxITEM_NORMAL);

    // Scaling tools
    m_figureTools->AddTool(ID_Shrink, _T(""), shrinkBitmap, _("Shrink figure"), wxITEM_NORMAL);
    m_figureTools->AddTool(ID_Grow, _T(""), growBitmap, _("Grow figure"), wxITEM_NORMAL);

    // Rotation tools
    m_figureTools->AddTool(ID_CW, _T(""), cwBitmap, _("Rotate clockwise"), wxITEM_NORMAL);
    m_figureTools->AddTool(ID_CCW, _T(""), ccwBitmap, _("Rotate counter-clockwise"), wxITEM_NORMAL);

    // choose a color, display in static
    color_display_ = new wxStaticText(m_figureTools, wxID_ANY, _T("         "));
    color_display_->SetOwnBackgroundColour(wxColour(0, 0, 0));
    m_figureTools->AddTool(ID_Color, _T(""), colorBitmap, _("Choose a color"), wxITEM_NORMAL);
    m_figureTools->AddControl(color_display_);

    m_figureTools->Realize();

    // Top-level sizer formats vertically: 1) thumbs, and 2) the rest (controls and frame)
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

   	// the thumbnail control
	wxBoxSizer* thumbSizer = new wxBoxSizer(wxHORIZONTAL);
    frameBrowser_ = new wxFilmstripCtrl(this, ID_FRAME_THUMB, wxDefaultPosition, wxSize(150, 125),
				wxSUNKEN_BORDER | wxHSCROLL | wxFS_MULTIPLE_SELECT);
	frameBrowser_->SetFilmstripImageSize(wxSize(150, 100));
    mainSizer->Add(frameBrowser_, 0, wxEXPAND|wxALL, 0);

    // This dude manages the control panel and animation frame (2nd row)
    wxBoxSizer* frameSizer = new wxBoxSizer(wxHORIZONTAL);

    // Animation frames rendered on fixed size canvas (640 x 480)
    m_canvas = new MyCanvas( this, wxID_ANY, wxDefaultPosition, wxSize(640, 480) );
    m_canvas->SetScrollbars( 0, 0, 0, 0 );
    m_canvas->SetBackgroundColour(wxColour(255, 255, 255));

  	frameSizer->Add(m_figureTools, 0, wxFIXED_MINSIZE, 0, 0);
    frameSizer->Add(m_canvas, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5);
    mainSizer->Add(frameSizer, 1, wxEXPAND|wxALL, 0);

    CreateStatusBar();
    SetStatusText( _T("Welcome to Stick'm Up!") );

    if (path_ != "NA") {
        LoadAnimation(const_cast<char *>(path_.c_str()));
    }
}

bool MyFrame::LoadAnimation(char *path)
{
    std::cout << "Loading animation from: " << path << std::endl;

    bool ret = false;
    if (anim_ != NULL) {
        delete anim_;
    }
    anim_ = NULL;

	std::ifstream ifs(path);
	if (ifs.good())	{
		boost::archive::xml_iarchive ia(ifs);
        ia >> boost::serialization::make_nvp("animation", anim_);
        if (anim_ == NULL) {
            std::cerr << "Error loading " << path_ << std::endl;
            return false;
        }
        
        // create the image cache
        meta_store* meta = anim_->get_meta_store();
        WxRender::init_meta_cache(meta);

        m_canvas->set_animation(anim_);
        m_canvas->set_frame(anim_->get_first_frame());

		frameBrowser_->Clear();
		frameBrowser_->Freeze();

		// Set some bright colors
		frameBrowser_->SetUnselectedFilmstripBackgroundColour(*wxWHITE);
		frameBrowser_->SetSelectedFilmstripBackgroundColour(*wxWHITE, *wxWHITE);

        // create the browser objects
		std::list<frame*> frames = anim_->get_frames();
		BOOST_FOREACH(frame* fr, frames) {
			frameBrowser_->Append(new wxStanFilmstripItem(fr));
            // for each frame, load the image cache for each figure
       		BOOST_FOREACH(figure* f, fr->get_figures()) {
               meta_store* meta = f->get_meta_store();
               WxRender::init_meta_cache(meta);
            }
        }

		frameBrowser_->Select(0);
		frameBrowser_->Thaw();
        ret = true;
	}
    ifs.close();

    return ret;
}

bool MyFrame::SaveAnimation(char* path)
{
    std::cout << "Saving animation to: " << path << std::endl;

    if (anim_ != NULL) {
        std::ofstream ofs(path);
        assert(ofs.good());
        {
            boost::archive::xml_oarchive oa(ofs);
            oa << boost::serialization::make_nvp("animation", anim_);

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

        m_canvas->add_figure(fig);
        ret = true;
	}
    ifs.close();

    return ret;
}

void MyFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
    if (anim_ != NULL) {
        delete anim_;
    }

    frameBrowser_->Clear();
	anim_ = new animation();

	frame* fr = new frame(0, 0, 640, 480);
	anim_->add_frame(fr);
    m_canvas->set_animation(anim_);

	frameBrowser_->Append(new wxStanFilmstripItem(fr));
    select_frame(0);
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    char path[200];
    sprintf_s(path, 200, "%s\\animations", data_path_.c_str());
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("ANI files (*.ani)|*.ani|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(path);
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK) {
        wxString wx_path = dialog.GetPath();

        char path[200];
        strncpy_s( path, 200, (const char*)wx_path.mb_str(wxConvUTF8), 200 );
        LoadAnimation(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    char path[200];
    sprintf_s(path, 200, "%s\\figures", data_path_.c_str());
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("FIG files (*.fig)|*.fig|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(path);
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK) {
        wxString wx_path = dialog.GetPath();

        char path[200];
        strncpy_s( path, (const char*)wx_path.mb_str(wxConvUTF8), 200 );
        LoadFigure(path);

        m_canvas->Refresh();
    }
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString caption = wxT("Save as ?");
    wxString wildcard = wxT("ANI files (*.ani)|*.ani|XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(".");
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK) {
        wxString wx_path = dialog.GetPath();

        char path[100];
        strcpy_s( path, 100, (const char*)wx_path.mb_str(wxConvUTF8) );
        SaveAnimation(path);

        m_canvas->Refresh();
    }
}

frame* MyFrame::select_frame(int index)
{
    frame *fr = NULL;
    if ( (index >= 0) && (index < frameBrowser_->GetCount()) ) {
        frameBrowser_->ClearSelections();
        frameBrowser_->EnsureVisible(index);
        frameBrowser_->Select(index);
        wxStanFilmstripItem* item = static_cast<wxStanFilmstripItem*>(frameBrowser_->GetItem(index));
        fr = item->get_frame();
    }
    m_canvas->set_frame(fr);

    return fr;
}

frame* MyFrame::first_frame()
{
    frame* fr = NULL;
    if (frameBrowser_->GetCount() > 0) {
        fr = select_frame(0);
    }
    return fr;
}

frame* MyFrame::next_frame()
{
    frame *fr = NULL;
    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        sel += 1;
        if (sel < frameBrowser_->GetCount()) {
            fr = select_frame(sel);
        }
    }
    return fr;
}

frame* MyFrame::prev_frame()
{
    frame *fr = NULL;
    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        sel -= 1;
        if (sel >= 0) {
            fr = select_frame(sel);
        }
    }
    return fr;
}

void MyFrame::OnNextFrame(wxCommandEvent& WXUNUSED(event))
{
    if (next_frame() != NULL) {
        m_canvas->Refresh();
    }
}

void MyFrame::OnPrevFrame(wxCommandEvent& WXUNUSED(event))
{
    if (prev_frame() != NULL) {
        m_canvas->Refresh();
    }
}

void MyFrame::OnCutFrame(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Cut selected frames." << std::endl;

    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        wxStanFilmstripItem* item = static_cast<wxStanFilmstripItem*>(frameBrowser_->GetItem(sel));
        frame* fr = item->get_frame();
        
        anim_->del_frame(fr);
        frameBrowser_->Delete(sel);

        int count = frameBrowser_->GetCount();
        if (sel >= count) {
            sel = count - 1;
        }

        select_frame(sel);
    }
}

void MyFrame::OnCopyFrame(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Copy frame." << std::endl;
    frame* fr = m_canvas->get_frame();
    if (fr != NULL) {
        // TODO: save frame in clipboard
    }
}

void MyFrame::OnPasteFrame(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Copy frame." << std::endl;
    frame* fr = m_canvas->get_frame();
    if (fr != NULL) {
        // TODO: paste frame from clipboard after selected item
    }
}

void MyFrame::OnDupFrame(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Duplicate currently selected frame." << std::endl;

    int sel = frameBrowser_->GetSelection();
    if (sel != -1) {
        wxStanFilmstripItem* item = static_cast<wxStanFilmstripItem*>(frameBrowser_->GetItem(sel));
        if (item != NULL) {
            frame* sel_frame  = item->get_frame();
            frame* fr = new frame(*sel_frame);
            anim_->insert_frame_after(sel_frame, fr);
   			frameBrowser_->Insert(new wxStanFilmstripItem(fr), sel);
            select_frame(sel + 1);
        }
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is Stick'em Up, by Ben, Simon, and Jerry Fordyce."),
                 _T("About Stick'em Up"), wxOK | wxICON_INFORMATION, this);
}

/**
 * Figure operations
 */

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

void MyFrame::OnCut(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Cut" << std::endl;
    m_canvas->cut_figure();
}

void MyFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Copy" << std::endl;
    m_canvas->copy_figure();
}

void MyFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    std::cout << "Paste" << std::endl;
    m_canvas->paste_figure();
}

void MyFrame::OnPlay(wxCommandEvent& event)
{
    if (timer_.IsRunning()) {
        timer_.Stop();
    }
    else {
        int frame_rate = frameRate_->GetValue();
        std::cout << "Play animation with rate " << frame_rate << std::endl;
        first_frame();
        m_canvas->set_animating(true);
        timer_.Start(1000 / frame_rate);

        // FIXME
        // load and play a sound
        //wxSound s(_T("C:\\Dev\\stan\\data\\sounds\\42847__FreqMan__psycho_scream_1.wav"));
        //if (s.IsOk()) {
        //    s.Play(wxSOUND_SYNC);
        //}
    }
}

void MyFrame::OnStop(wxCommandEvent& event)
{
    std::cout << "Stop animation." << std::endl;
    if (timer_.IsRunning()) {
        m_canvas->set_animating(false);
        timer_.Stop();
        m_canvas->Refresh();
    }
}

void MyFrame::OnThumbNailSelected(wxFilmstripEvent& event)
{
    std::cout << "Thumbnail selected with item index: " << (int)event.GetIndex() << std::endl;
    wxStanFilmstripItem* item = (wxStanFilmstripItem*)frameBrowser_->GetItem(event.GetIndex());
    m_canvas->set_frame(item->get_frame());
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
    // Draw the current frame
    m_canvas->Refresh();

    // With frame now displayed, launch any audios
    // TODO: might want to disable the timer while playing audio
    m_canvas->play_frame_audio();

    // Sequence to next frame
    if (next_frame() == NULL) {
        if (repeat_->GetValue() == true) {
            first_frame();
        }
        else {
            timer_.Stop();
            m_canvas->set_animating(false);
            first_frame();
        }
    }
}

void MyFrame::OnSelectImage(wxCommandEvent& event)
{
    char path[200];
    sprintf_s(path, 200, "%s\\images", data_path_.c_str());

    wxThumbnailBrowserDialog dialog(this, wxID_ANY, wxT("Choose an image..."));
    dialog.SetSelection(wxT(path));

    if (dialog.ShowModal() == wxID_OK) {
        wxString wx_path = dialog.GetSelection();

        char path[100];
        strcpy_s( path, 100, (const char*)wx_path.mb_str(wxConvUTF8) );

        // create an image object from the path
        int index = WxRender::cache_anim_metadata(anim_, std::string(path), META_IMAGE);

        // associate the image with the selected frame
        frame* fr = m_canvas->get_frame();
        fr->set_image_index(index);
        m_canvas->Refresh();
    }
}

void MyFrame::OnSelectSound(wxCommandEvent& event)
{
    char path[200];
    sprintf_s(path, 200, "%s\\sounds", data_path_.c_str());
    wxString caption = wxT("Choose a file");
    wxString wildcard = wxT("WAV files (*.wav)|*.wav");
    wxString defaultDir = wxT(path);
    wxString defaultFilename = wxEmptyString;
    wxFileDialog dialog(this, caption, defaultDir, defaultFilename, wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_OK) {
        wxString wx_path = dialog.GetPath();

        char path[200];
        strncpy_s( path, (const char*)wx_path.mb_str(wxConvUTF8), 200 );
        std::cout << "Sound file was selected: " << path << std::endl;

        // create a sound object from the path
        int index = WxRender::cache_anim_metadata(anim_, std::string(path), META_SOUND);

        // associate the image with the selected frame
        frame* fr = m_canvas->get_frame();
        fr->set_sound_index(index);
        
        // TODO: draw a sound image somewhere to denote a sound for this frame?
        m_canvas->Refresh();
    }
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

void MyFrame::OnCutTool(wxCommandEvent& event)
{
    std::cout << "Cut tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_CUT);
}

void MyFrame::OnBreak(wxCommandEvent& event)
{
    std::cout << "Break tool." << std::endl;
    m_canvas->set_mode(MyCanvas::M_BREAK);
}

void MyFrame::OnSelectFigureImage(wxCommandEvent& event)
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
