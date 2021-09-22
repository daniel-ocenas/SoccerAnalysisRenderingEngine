#include "gui.hpp"


Gui::Gui()
{
}

Gui::~Gui()
{
    glDeleteTextures(1, &play.textureId);
    glDeleteTextures(1, &pause.textureId);
    glDeleteTextures(1, &circle.textureId);
    glDeleteTextures(1, &replay.textureId);
    glDeleteTextures(1, &remove.textureId);
    glDeleteTextures(1, &edit.textureId);
    glDeleteTextures(1, &flag.textureId);
    glDeleteTextures(1, &arrow.textureId);
}


void Gui::Init(GLFWwindow *window, std::string path)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init("#version 460");

    InitializeTextureIcon(path + "\\graphics\\icons\\play.png", play);
    InitializeTextureIcon(path + "\\graphics\\icons\\pause.png", pause);
    InitializeTextureIcon(path + "\\graphics\\icons\\circle.png", circle);
    InitializeTextureIcon(path + "\\graphics\\icons\\replay.png", replay);
    InitializeTextureIcon(path + "\\graphics\\icons\\remove.png", remove);
    InitializeTextureIcon(path + "\\graphics\\icons\\edit.png", edit);
    InitializeTextureIcon(path + "\\graphics\\icons\\flag.png", flag);
    InitializeTextureIcon(path + "\\graphics\\icons\\arrow.png", arrow);
    _path = path;

    for (int i = 1; i <= sliderSize; i++)
    {
        _slider.push_back(0);
    }
    _slider.at(0) = 0;
    _slider.at(1) = -100;
    _slider.at(2) = -18;

}


int Gui::GetSliderVal(int index) 
{ 
    if (index < sliderSize)
    {
        return _slider.at(index); 
    }
    return -1;
}

void Gui::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void Gui::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Gui::MainMenu(double videoTime, GraphicObject& soccerPitch, double viewError)
{
    // flags for static windows
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 295));
    ImGui::Begin("Main menu", nullptr, flags);

    if (ImGui::Button("Open video from file"))
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".mp4", ".");

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            _filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

            // check if file was actually selected
            std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
            if (fileName.compare(".mp4") != 0)
            {
                _filePathName = std::string(_path + "\\graphics\\videos\\01.mp4");
                OpenVideoFunc(soccerPitch);
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));


    ImGui::Text("Open demo video");
    if (ImGui::Button("01.mp4"))
        openDemoVideo = 1;
    ImGui::SameLine();
    if (ImGui::Button("02.mp4"))
        openDemoVideo = 2;
    ImGui::SameLine();
    if (ImGui::Button("03.mp4"))
        openDemoVideo = 3;

    if (openDemoVideo != 0)
    {
        ImVec2 windowSize(300.f, 100.f);
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(ImVec2(_windowSize.x / 2.f - (windowSize.x / 2), _windowSize.y / 2.f - (windowSize.y / 2)));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor::HSV(0.62f, 0.91f, 0.88f));
        ImGui::Begin("Popup window");

        ImGui::Text(std::string("Do you want to open video file: 0" + std::to_string(openDemoVideo) + ".mp4?").c_str());
        ImGui::PopStyleColor();
        if (ImGui::Button("OPEN"))
        {
            _filePathName = std::string(_path + "\\graphics\\videos\\0" + std::to_string(openDemoVideo) + ".mp4");
            OpenVideoFunc(soccerPitch);
            openDemoVideo = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("CANCEL"))
        {
            openDemoVideo = 0;
        }

        ImGui::End();
    }

    ImGui::Dummy(ImVec2(0.0f, 5.f));
    if (ImGui::Button("Edit pitch points"))
    {
        editingPitchPoints = true;
    }

    if (editingPitchPoints)
    {

        ImGui::SetNextWindowPos(ImVec2(0, 3));
        ImGui::SetNextWindowSize(ImVec2(300, 292));
        ImGui::Begin("Edit pitch points menu");


        if (ImGui::Button("Clear points"))
            clearPitchPoints = true;

        if (selectedPitchPointsCount < 4)
        {
            ImGui::Text("Enter 4 pitch corner points starting bottom left ");
            ImGui::Text("corner clockwise. Save afterwards! ");
            ImGui::Text("Select point %d ", selectedPitchPointsCount + 1);
        }
        else
        {
            ImGui::Text("Points can be now adjusted by dragging");
            ImGui::Text("Current view error %.2f", viewError);
        }


        if (ImGui::Button("Save points"))
        {
            if (selectedPitchPointsCount == 4)
            {
                _recalculateProjections = true;
                 editingPitchPoints = false;
            }
            else
            {
                clearPitchPoints = true;
                editingPitchPoints = true;
            }
        }

        ImGui::End();
    }


    ImGui::Dummy(ImVec2(0.0f, 5.f));
    if (ImGui::BeginMenu("Pitch dimensions"))
    {
        ImGui::Text("Lenght(m): "); ImGui::SameLine();
        ImGui::PushItemWidth(80);
        if (ImGui::InputInt("", &fieldLength))
        {
            _recalculateProjections = true;
        }
        ImGui::PopItemWidth();
        ImGui::Text("Width(m): "); ImGui::SameLine();
        ImGui::PushItemWidth(80);
        if (ImGui::InputInt("", &fieldWidth))
        {
            _recalculateProjections = true;
        }
        ImGui::PopItemWidth();
        ImGui::EndMenu();
    }
    ImGui::Dummy(ImVec2(0.0f, 5.f));

    ImGui::Text("Virtual scene pitch");
    if (ImGui::Button((soccerPitch.instances.at(0).active) ? "Hide" : "Show"))
    {
        for (int i = 0; i < soccerPitch.instances.size(); i++)
        {
            soccerPitch.instances.at(i).active = !soccerPitch.instances.at(i).active;
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 10.));


    ImGui::PushID("PAUSEbtn");
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.7f, 1.f, 0.8f));
    if (ImGui::ImageButton((void*)(intptr_t)((play.boolean) ? pause.textureId : play.textureId), ImVec2(32, 32)))
    {
        play.boolean = !play.boolean;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    ImGui::PushID("REPLAYbtn");
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.7f, 1.f, 0.8f));
    ImGui::SameLine();
    if (ImGui::ImageButton((void*)(intptr_t)replay.textureId, ImVec2(32, 32)))
    {
        replay.boolean = !replay.boolean;
        _videoFileEnded = true;
        _videoFileUsed = true;
        _newVideoFileSelected = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();


    ImGui::Text("Time: %.2f", videoTime);

    ImGui::End();

}

void Gui::ArrowMenu(double videoTime, Arrow& arrows)
{
    // flags for static windows
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings;
    ImGui::SetNextWindowPos(ImVec2(300, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 295));
    ImGui::Begin("Arrow menu", nullptr, flags);

    ImGui::Image((void*)(intptr_t)arrow.textureId, ImVec2(64, 64));

    if (ImGui::BeginMenu("Create new arrow"))
    {
        ImGui::Text("Arrow color");
        const char* names[] = { colors[0].name, colors[1].name, colors[2].name, colors[3].name, colors[4].name, colors[5].name, colors[6].name, colors[7].name };
        static const char* arrowItem = colors[0].name;

        if (ImGui::BeginCombo("##combo1", arrowItem))
        {
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                bool is_selected = (arrowItem == names[n]);
                if (ImGui::Selectable(names[n], is_selected)) {
                    arrowItem = names[n];
                    arrowNewColor = n;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        if (ImGui::Button("Select points by clicking", ImVec2(0.f, 25.f))) {
            _mouseSelect = 1;
            _arrowPointSelection = true;

            if (!_arrowInCreation)
            {
                _arrowInCreation = true;
            }
        }
        
        ImGui::Dummy(ImVec2(0.0f, 5.));


        ImGui::Text("(%.1f,%.1f) (%.1f,%.1f)", p1.x, p1.y, p2.x, p2.y);
        ImGui::SameLine();


        ImGui::EndMenu();
    }


    if (ArrowEdit != -1)
    {

        ImGui::SetNextWindowPos(ImVec2(300, 0));
        ImGui::SetNextWindowSize(ImVec2(300, 295));
        ImGui::Begin("Arrow Edit Menu");

        ImGui::Text("Arrow color");
        const char* names[] = { colors[0].name, colors[1].name, colors[2].name, colors[3].name, colors[4].name, colors[5].name, colors[6].name, colors[7].name };
        
        std::cout << arrows.GetInstanceColorId(ArrowEdit) << std::endl;
        static const char* EditArrowItem = colors[arrows.GetInstanceColorId(ArrowEdit)].name;

        if (ImGui::BeginCombo("##comboEditArrow" + ArrowEdit, EditArrowItem))
        {
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                bool is_selected = (EditArrowItem == names[n]);
                if (ImGui::Selectable(names[n], is_selected)) {
                    EditArrowItem = names[n];
                    arrows.SetMaterial(n, ArrowEdit);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        if (ImGui::Button((arrows.GetInstanceAnimate(ArrowEdit)) ? "End grow animation": "Start grow animation"))
        {
            arrows.SetInstanceAnimate(ArrowEdit, !arrows.GetInstanceAnimate(ArrowEdit));
        };

        //if (ImGui::Button("Select points by clicking", ImVec2(0.f, 25.f))) {
        //    _arrowInCreation = false;
        //    _mouseSelect = 1;
        //    _arrowPointSelection = true;

        //}
        if(ImGui::Button("Conform edit")) { ArrowEdit = -1; };

        ImGui::End();

    }

    ImGui::Text("Arrows list");
    
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.7f, 1.f, 0.8f));
    for (int i = arrows.higlightingArrowsOffset; i < arrows.instancesCnt; i++)
    {
        glm::vec4 arrowPoints = arrows.GetArrowPoints(i);
        ImGui::Text("(%.1f,%.1f) (%.1f,%.1f)", arrowPoints.x, arrowPoints.y, arrowPoints.z, arrowPoints.w);
        ImGui::SameLine();
        ImGui::PushID("Delete"+i);
        if (ImGui::ImageButton((void*)(intptr_t)remove.textureId, ImVec2(16, 16)))
        {
            arrows.DeleteArrowInstance(i);
        }
        ImGui::PopID();
        ImGui::SameLine();

        // Edit arrow
        ImGui::PushID("Edit" + i);
        if (ImGui::ImageButton((void*)(intptr_t)edit.textureId, ImVec2(16, 16)))
        {
            ArrowEdit = i;
            

            ImGui::EndMenu();
        }
        ImGui::PopID();
    }
    ImGui::PopStyleColor(3);


    ImGui::End();
}



void Gui::MatchMenu(GraphicObject& highlits, Arrow& arrows)
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings;
    ImGui::SetNextWindowPos(ImVec2(600, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 295));
    ImGui::Begin("Match menu", nullptr, flags);

    //ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Image((void*)(intptr_t)circle.textureId, ImVec2(64, 64));

    if (ImGui::Button((_playerHighlighting) ? "Hide player highlighting" : "Show player highlighting"))
    {
        _playerHighlighting = !_playerHighlighting;
        for (int i = 0; i < highlits.instances.size(); i++)
        {
            highlits.instances.at(i).active = !highlits.instances.at(i).active;
        }
    }


    if (ImGui::Button((_arrowDrawing) ? "Hide highliting arrows" : "Show highliting arrows"))
    {
        _arrowDrawing = !_arrowDrawing;        
        for (int i = 0; i < arrows.higlightingArrowsOffset; i++)
        {
            arrows.SetActive(_arrowDrawing,i);
        }

    }


    ImGui::Text("Team jersey colors");
    const char* names[] = { colors[0].name, colors[1].name, colors[2].name, colors[3].name, colors[4].name, colors[5].name };
    static const char* teamItem[] = { "Team one", "Team two" };
    if (teamColor[0] != -1)
        teamItem[0] = names[teamColor[0]];
    if (teamColor[1] != -1)
        teamItem[1] = names[teamColor[1]];

    if (ImGui::BeginCombo("##combo1", teamItem[0]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(names); n++)
        {
            bool is_selected = (teamItem[0] == names[n]);
            if (ImGui::Selectable(names[n], is_selected)) {
                teamItem[0] = names[n];
                teamColor[0] = n;
                _newTeamColorSelected = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("##combo2", teamItem[1]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(names); n++)
        {
            bool is_selected = (teamItem[1] == names[n]);
            if (ImGui::Selectable(names[n], is_selected)) {
                teamItem[1] = names[n];
                teamColor[1] = n;
                _newTeamColorSelected = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::End();
}


void Gui::NewVideoMenu(double viewError)
{
    if (_newVideoFileSelectedMenu)
    {

        play.boolean = false;
        ImVec2 windowSize(400.f, 295.f);
        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(ImVec2(_windowSize.x / 2.f - (windowSize.x / 2), 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor::HSV(0.62f,0.91f, 0.88f ));
        ImGui::Begin("New video menu");
        ImGui::PopStyleColor();

        bool fileKnown = false;
        if (_filePathName.find("\\01.mp4") != std::string::npos
            || _filePathName.find("/01.mp4") != std::string::npos)
            fileKnown = true;
        else if (_filePathName.find("\\02.mp4") != std::string::npos
            || _filePathName.find("/02.mp4") != std::string::npos)
            fileKnown = true;
        else if (_filePathName.find("\\03.mp4") != std::string::npos
            || _filePathName.find("/03.mp4") != std::string::npos)
            fileKnown = true;

        if (fileKnown)
        {
            ImGui::Text("Selected video file has saved settings.");
            ImGui::Text("Do You want to load saved settings? ");
            if (ImGui::Button("LOAD SETTINGS"))
            {
                SetSelectedPitchPointsCount(4);
                SetMatrixType(2);
                editingPitchPoints = false;
                _recalculateProjections = true;
                _newVideoFileSelectedMenu = false;
                videoSelectMenuConfirmed = true;
                savedSettings = true;
                return;
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Text("Please select team jersey colors");

        const char* names[] = { colors[0].name, colors[1].name, colors[2].name, colors[3].name, colors[4].name, colors[5].name };
        static const char* teamItem[] = { "Team one", "Team two" };
        if (teamColor[0] != -1)
            teamItem[0] = names[teamColor[0]];
        if (teamColor[1] != -1)
            teamItem[1] = names[teamColor[1]];
        if (ImGui::BeginCombo("##combo1", teamItem[0]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                bool is_selected = (teamItem[0] == names[n]);
                if (ImGui::Selectable(names[n], is_selected)) {
                    teamItem[0] = names[n];
                    teamColor[0] = n;
                    _newTeamColorSelected = true;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("##combo2", teamItem[1]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                bool is_selected = (teamItem[1] == names[n]);
                if (ImGui::Selectable(names[n], is_selected)) {
                    teamItem[1] = names[n];
                    teamColor[1] = n;
                    _newTeamColorSelected = true;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (teamColor[0] != -1 && teamColor[1] != -1)
        {


            if (pitchPointsInit)
            {
                clearPitchPoints = true;
                pitchPointsInit = false;
                editingPitchPoints = true;
            }
            ImGui::Text("Select corner points starting bottom left ");
            ImGui::Text("corner clockwise. Click CONFIRM after selection! ");

            if (selectedPitchPointsCount < 4)
            {
                _matView = 1;
                ImGui::Text("Select point %d ", selectedPitchPointsCount + 1);
            }
            else
            {
                ImGui::Text("Points can be now adjusted by dragging");
                ImGui::Text("Move points if view errow is too high");
                ImGui::Text("Current view error %.2f", viewError);
            }

            if (ImGui::Button("CONFIRM") )
            {
                if (selectedPitchPointsCount == 4)
                {
                    editingPitchPoints = false;
                    _recalculateProjections = true;
                    _newVideoFileSelectedMenu = false;
                    videoSelectMenuConfirmed = true;
                }
                else 
                {
                    clearPitchPoints = true;
                    editingPitchPoints = true;
                }
            }

            if (ImGui::Button("Clear points"))
            {
                    clearPitchPoints = true;
            }
        }


        ImGui::End();
    }

}

void Gui::OffsideLineMenu(double videoTime, OffsideLine& offsideLine)
{
    // flags for static windows
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings;
    ImGui::SetNextWindowPos(ImVec2(900, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 295));
    ImGui::Begin("Offside line menu", nullptr, flags);

    ImGui::Image((void*)(intptr_t)flag.textureId, ImVec2(64, 64));

    ImGui::Text("Virtual offside line");
    if (ImGui::Button((offsideLine.IsActive()) ? "Hide" : "Show"))
    {
        offsideLine.InvertActive();
        if (offsideLine.IsActive())
        {
            _offsideLineMove = true;
            play.boolean = false;
        }
    }

    ImGui::Text("Move offide position by cursor");
    if (ImGui::Button((_offsideLineMove) ? "Click on location to save" : "Edit"))
        _offsideLineMove = !_offsideLineMove;

    bool autoOffside = false;
    ImGui::Text("Automatic offside line position");
    if (ImGui::Button((offsideLine.GetAutoOffside()) ? "Turn Off" : "Turn On"))
        offsideLine.SetAutoOffside(!offsideLine.GetAutoOffside());

    ImGui::End();
}

void Gui::DebugWindowDraw()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings;
    ImGui::SetNextWindowPos(ImVec2(_windowSize.x - 344, 0));
    ImGui::SetNextWindowSize(ImVec2(344, 295));
    ImGui::Begin("Settings",nullptr, flags);


    ImGui::Text("Light position");
    for (int i = 0; i < sliderSize; i++)
    {
        int maxVal = 100;
        int minVal = -100;
        std::string SliderName = std::string("Slider") + std::to_string(i);
        ImGui::SliderInt(SliderName.c_str(), &_slider.at(i), minVal, maxVal);
    }


    ImGui::Text("Video Type");
    ImGui::RadioButton("Depth map", &_imgView, 0); ImGui::SameLine();
    ImGui::RadioButton("Player Detection", &_imgView, 1); 
    ImGui::RadioButton("Final Scene", &_imgView, 2);ImGui::SameLine();
    ImGui::RadioButton("Virtual Scene", &_imgView, 3);
    ImGui::Dummy(ImVec2(0.0f, 10.0f));


    ImGui::Text("Scene view matrix");
    if (ImGui::RadioButton("Manual View Matrix", &_matView, 0) )
    {
        _recalculateProjections = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Automatic View Matrix", &_matView, 1))
    {
        _recalculateProjections = true;
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));


    ImGui::Text("Auto view matrix calibration type");
    if (ImGui::RadioButton("SolvePnP", &_calibration, 0))
    {
        _recalculateProjections = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Calib", &_calibration, 1))
    {
        _recalculateProjections = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Calib Adv", &_calibration, 2))
    {
        _recalculateProjections = true;
    }

    //ImGui::Text("Gui calc fps %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
}


/**
* @brief Converts OpenCv Mat to OpenGL texture
*/
void Gui::InitializeTextureIcon(std::string location, Icon &icon)
{
    cv::Mat mat = cv::imread(location);
    if (mat.empty())
    {
        throw("Could not load the icon: %s \n", location);
    }

    // Set input texture format
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1)
    {
        inputColourFormat = GL_RED;
    }

    glEnable(GL_TEXTURE_2D);
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D,     // Texture type
        0,                 // Pyramid level
        GL_RGB,            // Output color format
        mat.rows,          // Image width
        mat.cols,          // Image height
        0,                 // Width of border 0/1
        inputColourFormat, // Input format
        GL_UNSIGNED_BYTE,  // Image data type
        mat.ptr()          // Image data 
    );

    icon.textureId = textureId;
    icon.width = mat.rows;
    icon.height = mat.cols;


    glDisable(GL_TEXTURE_2D);
}

void Gui::OpenVideoFunc(GraphicObject &soccerPitch)
{

    _newVideoFileSelected = true;
    _newVideoFileSelectedMenu = true;

    // stop video play
    play.boolean = false;

    // reset match settings
    teamColor[0] = -1;  teamColor[1] = -1;
    teamSelectCombo[0] = -1;  teamSelectCombo[1] = -1;
    pitchPointsInit = true;
    _showPlane = false;

    for (int i = 0; i < soccerPitch.instances.size(); i++)
    {
        soccerPitch.instances.at(i).active = false;
    }
}