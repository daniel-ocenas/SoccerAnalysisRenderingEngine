#include "Controller.hpp"

/**
* @brief Application constructor
*/
Controller::Controller(char** argv)
{
    std::string dir(argv[0]);
    path = dir.substr(0, dir.find_last_of("/\\"));

    glfwSetErrorCallback(ErrorCallback);

    InitGLFW();
    InitGLEW();
    InitOpenGL(viewHandle->width, viewHandle->height);
    viewHandle->GetGui()->Init(viewHandle->GetWindow(), path);
    viewHandle->GetGui()->SetWindowSize((float)viewHandle->width, (float)viewHandle->height);


    // Create and compile shaders
    _objectShader = new Shader();
    _objectShader->compileShaders(path+ "\\shaders\\ShadowMapping.vert", path + "\\shaders\\ShadowMapping.frag");
    _depthViewShader = new Shader();
    _depthViewShader->compileShaders(path + "\\shaders\\ShadowMappingDebug.vert", path + "\\shaders\\ShadowMappingDebug.frag");
    _depthShader = new Shader();    
    _depthShader->compileShaders(path + "\\shaders\\ShadowMappingDepth.vert", path + "\\shaders\\ShadowMappingDepth.frag");
    InitDepthShader();

    // Create objects
    _circle = new GraphicObject();
    _lightCube = new GraphicObject();
    _soccerPitch = new GraphicObject();
    _arrow = new Arrow(path);
    _offsideLine = new OffsideLine(path);


    ModelLoader loader;
    //loader.loadObjFile(_circle->vao, _circle->mesh, circleVertices, circleUVs, circleNormals, circleSize);
    //loader.loadObjFile(_soccerPitch->vao, _soccerPitch->mesh, planeVertices, planeUVs, planeNormals, planeSize);
    //loader.loadObjFile(_lightCube->vao, _lightCube->mesh, cubeVertices, cubeUVs, cubeNormals, cubeSize);
    loader.loadObjFile(path + "\\graphics\\objects3D\\circle3D.obj", _circle->vao, _circle->mesh);
    loader.loadObjFile(path + "\\graphics\\objects3D\\plane.obj", _soccerPitch->vao, _soccerPitch->mesh);
    loader.loadObjFile(path + "\\graphics\\objects3D\\cube.obj", _lightCube->vao, _lightCube->mesh);

    // preopen video
    //viewHandle->GetGui()->SetFilePathName(path + "\\graphics\\videos\\01.mp4");
    //viewHandle->GetGui()->SetNewVideoFileSelected(true);
    //viewHandle->GetGui()->SetVideoSelectMenuConfirmed(true);
    //viewHandle->GetGui()->SetSelectedPitchPointsCount(4);
    //viewHandle->GetGui()->SetMatrixType(2);
    //viewHandle->GetGui()->SetRecalculateProjections(true);
    //viewHandle->GetGui()->SetSavedSettings(true);

    viewHandle->GetPerspectiveCamera()->setDistance(initViewParams[0]);
    viewHandle->GetPerspectiveCamera()->setXRotation(initViewParams[1]);

    viewHandle->InitVideoLoader(path);

    // Depth map visualization
    float quadVertices[] = {
        //positions          texture Coords
       -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,
       -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,   1.0f, 1.0f,
        1.0f,  1.0f, 0.0f,   1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

}

/**
* @brief Application destructor
*/
Controller::~Controller()
{
    glDeleteFramebuffersEXT(1, &depthMapFBO);
    glDeleteTextures(1, &depthMap);

    delete _circle;
    delete _arrow;
    delete _lightCube;
    delete _soccerPitch;
    delete _offsideLine;

    delete _objectShader;
    delete _depthShader;
    delete _depthViewShader;

    delete viewHandle;

    if (match != nullptr)
        delete match;


    glfwTerminate();
}


/**
* @brief GLFW Initialization
*/
void Controller::InitGLFW()
{
    if (!glfwInit())
        throw("GLFW initiation error");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Rendering Engine", NULL, NULL);
    viewHandle = new View(window);

    viewHandle->setCursor(glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
    if (!window) {
        glfwTerminate();
        throw("GLFW Window creation error");
    }

    int xpos, ypos;
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    glfwGetMonitorWorkarea(primary, &xpos, &ypos, &viewHandle->monitorWidth, &viewHandle->monitorHeight);

    glfwSetWindowPos(window, 0, 28);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //glfwSetWindowSizeCallback(window, resizeWindow);
    glfwSetFramebufferSizeCallback(window, ResizeWindow);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonnCallback);
}


/**
* @brief GLEW initialization
*/
void Controller::InitGLEW()
{
    //  Initialise glew (must occur AFTER window creation or glew will error)
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        throw("GLEW initialisation error: %s", glewGetErrorString(err));
    }

}

/**
* @brief OpenGL initialization
*/
void Controller::InitOpenGL(int w, int h)
{
    // use a screen size of WIDTH x HEIGHT
    glViewport(0, 0, w, h);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


/**
* @brief Resize Opengl Window Callback for GLFW
*/
void Controller::ResizeWindow(GLFWwindow* window, int width, int height)
{
    View* win = reinterpret_cast<View*>(glfwGetWindowUserPointer(window));
    win->width = width;
    win->height = height - VIEWPORT_YOFFSET;
    glViewport(0, 0, win->width, win->height);

    win->GetGui()->SetWindowSize((float)win->width, (float)win->height);

    // change project matrix based on new dimensions
    win->GetPerspectiveCamera()->setAspect((float)win->width / win->height);

    if (win->GetGui()->GetVideoFileUsed())
    {
        win->GetVideoHandler()->ResizeVideo(win->width = width, win->height);
    }
}


/**
* @brief GLFW mouse button click callback
*/
void Controller::MouseButtonnCallback(GLFWwindow* window, int button, int action, int mods)
{
    View* win = reinterpret_cast<View*>(glfwGetWindowUserPointer(window));

    double xpos, ypos;
    glfwGetCursorPos(win->GetWindow(), &xpos, &ypos);
    ypos -= VIEWPORT_YOFFSET;
    //std::cout << xpos << " " << ypos << std::endl;

    if (win->GetGui()->GetOffsideLineMove())
    {
        win->GetGui()->SetOffsideLineMove(false);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {

        if (win->GetGui()->GetMouseSelect() != 0)
        {
            win->GetGui()->SetArrowPointSelection(true);
        }

        if (win->GetGui()->GetEditingPitchPoints())
        {

            int pitchScreenPointsSize = (int)win->GetVideoHandler()->GetPitchScreenPoints().size();
            if (pitchScreenPointsSize == 4)
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (
                        (action == GLFW_PRESS) &&
                        ((abs(win->GetVideoHandler()->GetPitchScreenPoints()[i].x - xpos) < 10)) &&
                        (abs(win->GetVideoHandler()->GetPitchScreenPoints()[i].y - ypos) < 10)
                        )
                    {
                        win->GetGui()->SetSelectedCornerIndex(i);
                        win->GetGui()->SetDragging(true);
                    }
                }
            }
            else if (pitchScreenPointsSize <= 4)
            {
                if (pitchScreenPointsSize == 3)
                {
                    win->GetGui()->SetRecalculateProjections(true);
                }
                win->GetVideoHandler()->AddPitchScreenPoint((int)xpos, (int)ypos);
                win->GetGui()->SetSelectedPitchPointsCount(pitchScreenPointsSize + 1);
            }
        }

    }


    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        if (win->GetGui()->GetEditingPitchPoints() && win->GetGui()->GetDragging())
        {
            win->GetVideoHandler()->EditPitchScreenPoint(win->GetGui()->GetSelectedCornerIndex(), (int)xpos, (int)ypos);
            win->GetGui()->SetDragging(false);
            win->GetGui()->SetRecalculateProjections(true);
        }
    }
}


/**
* @brief GLFW error callback
*/
void Controller::ErrorCallback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

/**
* @brief Keyboard input handling
*/
void Controller::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    View* win = reinterpret_cast<View*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(win->GetWindow(), GLFW_TRUE);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        win->GetGui()->SetPlayInvert();
    }

    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        if (win->moveSensitivity < 1.f)
        {
            win->moveSensitivity += 0.01f;
        }
    }
    if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        if (win->moveSensitivity > 0.01f)
        {
            win->moveSensitivity -= 0.01f;
        }
    }

    // move camera up
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addYPosition(-win->moveSensitivity);
        win->GetPerspectiveCamera()->addYposView(-win->moveSensitivity);
    }

    // move camera down
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addYPosition(win->moveSensitivity);
        win->GetPerspectiveCamera()->addYposView(win->moveSensitivity);
    }

    // move camera right
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addXPosition(win->moveSensitivity);
        win->GetPerspectiveCamera()->addXposView(win->moveSensitivity);
    }

    // move camera left
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addXPosition(-win->moveSensitivity);
        win->GetPerspectiveCamera()->addXposView(-win->moveSensitivity);
    }

    // increase camera disctance
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addDistance(win->moveSensitivity);
        win->GetPerspectiveCamera()->addViewDistance(win->moveSensitivity);
    }

    // decrease camera distance
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addDistance(-win->moveSensitivity);
        win->GetPerspectiveCamera()->addViewDistance(-win->moveSensitivity);
    }

    // Add rotations
    if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addXRotation(win->moveSensitivity);
        win->GetPerspectiveCamera()->rotateXView(win->moveSensitivity);
    }
    if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addXRotation(-win->moveSensitivity);
        win->GetPerspectiveCamera()->rotateXView(-win->moveSensitivity);
    }
    if (key == GLFW_KEY_C && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addYRotation(1.f);
        win->GetPerspectiveCamera()->rotateYView(1.f);
    }
    if (key == GLFW_KEY_V && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        win->GetPerspectiveCamera()->addYRotation(-1.f);
        win->GetPerspectiveCamera()->rotateYView(-1.f);
    }
}



/**
* @brief Inicialize depth uniform location IDs
*/
void Controller::InitDepthShader()
{
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1920, 1080, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw("Depth frame buffer error.");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
* @brief Application main loop
*/
void Controller::start()
{
    double video_start_time = glfwGetTime();
    GenerateGraphicObjects();

    // main loop
    while (!glfwWindowShouldClose(viewHandle->GetWindow()))
    {
        // load current time and scene
        _frameStartTime = glfwGetTime();

        // Clear color and depth buffers
        glViewport(0, 0, viewHandle->width, viewHandle->height);
        glEnable(GL_DEPTH_CLAMP);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        viewHandle->GetGui()->DebugWindowDraw();
        viewHandle->GetGui()->ArrowMenu(viewHandle->GetVideoHandler()->GetTime(), *(_arrow));
        viewHandle->GetGui()->MainMenu(viewHandle->GetVideoHandler()->GetTime(), *(_soccerPitch), currentViewError / 4.0);
        viewHandle->GetGui()->OffsideLineMenu(viewHandle->GetVideoHandler()->GetTime(), *(_offsideLine));
        viewHandle->GetGui()->MatchMenu(*(_circle), *(_arrow));
        viewHandle->GetGui()->NewVideoMenu(currentViewError / 4.0);
        ImGui::End();

        // calculate view matrix and homography settings 
        if (viewHandle->GetGui()->GetVideoFileUsed()&&
            (viewHandle->GetGui()->GetRecalculateProjections() || viewHandle->GetGui()->GetSavedSettings())  ) 
        {
            if (viewHandle->GetGui()->GetSavedSettings())
            {
                ApplyDemoSettings();
                viewHandle->GetVideoHandler()->LoadDemoVideoPoints(viewHandle->GetGui()->GetFilePathName());
                viewHandle->GetGui()->SetSavedSettings(false);
            }
            ResizeField();
            viewHandle->GetVideoHandler()->SetFieldParametes((float)viewHandle->GetGui()->GetFieldWidth(), (float)viewHandle->GetGui()->GetFieldLength());
            viewHandle->GetVideoHandler()->CalculateHomographyMatrix();
            viewHandle->GetPerspectiveCamera()->setVideoViewMatrix(viewHandle->GetVideoHandler()->CalculateVideoViewMatrix());

            if (viewHandle->GetGui()->GetCalibrationType())
            {
                RefineViewMatrix();
            }

            viewHandle->GetGui()->SetRecalculateProjections(false);
        }


        // load matrices  
        glm::mat4 proj = viewHandle->GetPerspectiveCamera()->getProjMatrix();
        glm::mat4 view = glm::mat4(1.f);
        if (viewHandle->GetGui()->GetVideoSelectMenuConfirmed())
            view = VideoInitializationAnimation();
        else if (viewHandle->GetGui()->GetMatrixType() == 0)
            view = viewHandle->GetPerspectiveCamera()->getViewMatrix();
        else
            view = viewHandle->GetPerspectiveCamera()->getVideoViewMatrix();

        glm::vec3 camera = glm::vec3(glm::inverse(view) * glm::vec4(0.f, 0.f, 0.f, 1.f));


        //test needed each frame, adjust
        viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, currentViewError);

        _light.x = (float) viewHandle->GetGui()->GetSliderVal(0);
        _light.y = (float) viewHandle->GetGui()->GetSliderVal(1);
        _light.z = (float) viewHandle->GetGui()->GetSliderVal(2);


        // depth shader setup
        glm::mat4 lightProj = glm::ortho(-100.f, 100.f, -40.f, 40.f, 0.1f, 168.f);
        glm::mat4 lightView = glm::lookAt(glm::vec3(_light.x,-_light.y, _light.z), glm::vec3(0.f,0.f, 20.f), glm::vec3(0.0, 1.f, 0.0));
        glm::mat4 lightSpaceMat = lightProj * lightView;

        // Loads video frame
        LoadVideoFrame();

        // Prepares model mats for scene rendering
        SetGraphicObjects();

        //Depth PreRendering disabled for not functioning
        // set back cull face for shadow render
        glCullFace(GL_BACK);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.f, -1.f);
        _depthShader->use();
        glViewport(0, 0, viewHandle->width, viewHandle->height * 3);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        _depthShader->setMat4("lightSpaceMat", lightSpaceMat);
        RenderScene(_depthShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);


        // set front cull face for object drawing
        glCullFace(GL_FRONT);
        glDisable(GL_POLYGON_OFFSET_FILL);
        if (viewHandle->GetGui()->GetViewType() == ORIGINALVIDEO)
        {
            // Shadow Texture debug
            glViewport(0, 0, viewHandle->width / 3, viewHandle->height / 2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _depthViewShader->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            RenderQuad();
            glUseProgram(0);

        }

        glViewport(0, 0, viewHandle->width, viewHandle->height);

        // draws video background
        if (viewHandle->GetGui()->GetViewType() == ORIGINALVIDEO || viewHandle->GetGui()->GetViewType() == MASKINGVIDEO)
            viewHandle->GetVideoHandler()->DrawBackground();

        // draws shadows on background
        //if (viewHandle->GetGui()->GetViewType() == MASKINGVIDEO)
        //    viewHandle->GetVideoHandler()->DrawShadowsOnBackground(depthMap);

        // render scene unless showin player mask
        if (viewHandle->GetGui()->GetViewType() != THRESHOLDVIDEO)
        {

            // Scene Rendering
            _objectShader->use();

            // Pass uniform values into shaders
            _objectShader->setMat4("lightSpaceMat", glm::mat4(lightSpaceMat));
            _objectShader->setVec3("lightPos", glm::vec3(_light));
            _objectShader->setMat4("viewMat", view);
            _objectShader->setMat4("projMat", proj);
            _objectShader->setVec3("cameraPos", camera);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);

            //Draw white cube at position of light
            //_objectShader->SetMaterialUniforms(_lightCube->instances.at(0).material);
            //_objectShader->SetModelMatUniform(
            //    glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(_light)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.f, 2.f, 2.f))
            //));
            //_lightCube->Draw();

            RenderScene(_objectShader);

            // Stop using shader, Scene Rendering END
            glUseProgram(0);
        }

        // Perform masking or draw player mask
        if ((viewHandle->GetGui()->GetViewType() == MASKINGVIDEO || viewHandle->GetGui()->GetViewType() == THRESHOLDVIDEO)
             &&  !viewHandle->GetGui()->GetVideoSelectMenuConfirmed())
            viewHandle->GetVideoHandler()->DrawMaskFrame(viewHandle->GetGui()->GetViewType());

        // Render gui window
        viewHandle->GetGui()->Render();

        // Lock frame rate for video FPS
        LockFrameRate(viewHandle->GetVideoHandler()->GetFPS());

        // Swap window buffers
        glfwSwapBuffers(viewHandle->GetWindow());

        // Manage window events
        glfwPollEvents();
    }

}


/**
* @brief Function renders graphic objects
*/
void Controller::RenderScene(Shader* shader)
{


    // render player highlighting
    for (int i = 0; i < _circle->instances.size(); i++)
    {
        if (_circle->instances.at(i).active)
        {
            shader->SetMaterialUniforms(_circle->instances.at(i).material);
            shader->SetModelMatUniform(_circle->instances.at(i).modelMat);
            _circle->Draw();
        }
    }

    // render arrows 
    for (int i = 0;i < _arrow->instancesCnt; i++)
    {
        if (_arrow->IsActive(i))
        {
            _arrow->Draw(shader, i, (float) glfwGetTime());
        }
    }

    // render offside line arrows
    if (_offsideLine->IsActive())
    {
        shader->SetMaterialUniforms(_offsideLine->GetMaterial());
        shader->SetModelMatUniform(_offsideLine->GetModelMat());
        _offsideLine->Draw();
    }
    // render pitch 
    for (int i = 0;i < _soccerPitch->instances.size(); i++)
    {
        if (_soccerPitch->instances.at(i).active)
        {
            shader->SetMaterialUniforms(_soccerPitch->instances.at(i).material);
            shader->SetModelMatUniform(_soccerPitch->instances.at(i).modelMat);
            _soccerPitch->Draw();
        }
    }
}


/**
* @brief Initial objects initialization
*/
void Controller::GenerateGraphicObjects()
{
    for (int i = 0; i < 2;i++)
    {
        for (int j = 0; j < 10;j++)
        {
            _circle->CreateObjectInstance(glm::mat4(1.f), (i == 0) ? BLUE : WHITE);
            _arrow->CreateArrowInstance(glm::mat4(0.f), (i == 0) ? BLUE : WHITE, (i == 0)?0:1);
        }
    }

    _lightCube->CreateObjectInstance(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(_light)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.f, 2.f, 2.f))), WHITE);

    //middleline
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), PITCH);
    //16th lines
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);
    _soccerPitch->CreateObjectInstance(glm::mat4(0.f), WHITE);

    ResizeField();

}

/**
* @brief Functions sets up player graphic objects model matrices for a simplier Draw function
*/
void Controller::MovePlayerHightlits(std::vector<std::vector<glm::vec2>> playerPositions)
{
    float rotationInTime = (float)120.f * fmodf((float)glfwGetTime(), 3.f);
    float moveInTime = (float)sin(fmodf(4.f * (float)glfwGetTime(), 2.f * 3.14159265f)) / 3.f + 4.f;

    std::vector<glm::vec2> playerPositionsVec;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < match->teams[i].players.size(); j++)
        {
            if (match->teams[i].players[j].positions.size() > 0)
            {
                playerPositionsVec.push_back(glm::vec2(
                    match->teams[i].players[j].positions[0].x,
                    match->teams[i].players[j].positions[0].y
                ));
            }
        }
    }

    for (int i = 0;i < _circle->instances.size(); i++)
    {
        _circle->SetObjectMat(glm::mat4(0.f), i);
        // used if having always correct ammount of detected players
        if (i < _circle->instances.size() && i < playerPositionsVec.size())
        {
            glm::mat4 tmpMat = glm::mat4(
                glm::translate(glm::mat4(1.0f), glm::vec3(playerPositionsVec[i].x, .03f, playerPositionsVec[i].y)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(rotationInTime), glm::vec3(0.0f, 1.0f, 0.0f))
            );
            _circle->SetObjectMat(tmpMat, i);
        }
    }


    for (int i = 0;i < _arrow->higlightingArrowsOffset; i++)
    {
        _arrow->SetArrowMats(glm::mat4(0.f), glm::mat4(0.f), i);
        if (i < _arrow->instancesCnt && i < (playerPositionsVec.size()))
        {

            glm::mat4 tubeMat = glm::translate(glm::mat4(1.0f), glm::vec3(playerPositionsVec[i].x, moveInTime, playerPositionsVec[i].y))
                * glm::rotate(glm::mat4(1.0f), glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::scale(glm::mat4(1.f), glm::vec3(4.f, 4.f, 1.5f));
            glm::mat4 cylinderMat = glm::translate(glm::mat4(tubeMat), glm::vec3(0.f, 0.f, 1.f));
            _arrow->SetArrowMats(cylinderMat, tubeMat, i);
        }
    }
}


/**
* @brief Function prepares graphic objects for rendering
*/
void Controller::SetGraphicObjects()
{

    double xpos, ypos;
    glfwGetCursorPos(viewHandle->GetWindow(), &xpos, &ypos);
    ypos -= VIEWPORT_YOFFSET;
    cv::Point2f warpedPoint = viewHandle->GetVideoHandler()->GetScreenToScenePoint((float)xpos, (float)ypos);
    int fieldHalfWidth = viewHandle->GetGui()->GetFieldWidth() / 2;
    int fieldHalfLength = viewHandle->GetGui()->GetFieldLength() / 2;
    if (warpedPoint.x > fieldHalfLength)
        warpedPoint.x = (float) fieldHalfLength;
    if (warpedPoint.x < -fieldHalfLength)
        warpedPoint.x = (float) -fieldHalfLength;
    if (warpedPoint.y > fieldHalfWidth)
        warpedPoint.y = (float) fieldHalfWidth;
    if (warpedPoint.y < -fieldHalfWidth)
        warpedPoint.y = (float) -fieldHalfWidth;

    if (viewHandle->GetGui()->GetOffsideLineMove())
        _offsideLine->SetModelmat(warpedPoint.x, (float) viewHandle->GetGui()->GetFieldWidth());


    if (_offsideLine->GetAutoOffside())
    {
        std::vector<std::vector<glm::vec2>> playerPositionsVec;
        playerPositionsVec.resize(20);
        for (int i = 0; i < match->teams.size(); i++)
        {
            for (int j = 0; j < match->teams[i].players.size(); j++)
            {
                if (match->teams[i].players[j].positions.size() > 0 && std::isfinite(match->teams[i].players[j].positions[0].x) && std::isfinite(match->teams[i].players[j].positions[0].y))
                {
                    playerPositionsVec[i].push_back(glm::vec2(
                        match->teams[i].players[j].positions[0].x,
                        match->teams[i].players[j].positions[0].y
                    ));
                }
            }
        }
        _offsideLine->AutoOffside(playerPositionsVec, (float) viewHandle->GetGui()->GetFieldWidth());
    }



    if (glfwGetMouseButton(viewHandle->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && viewHandle->GetGui()->GetArrowPointSelection())
    {
        viewHandle->GetGui()->SetArrowPointSelection(false);
            
        if (viewHandle->GetGui()->GetArrowCreation() && viewHandle->GetGui()->GetMouseSelect() == 1)
        {
            if (!std::isnan(warpedPoint.x) && !std::isnan(warpedPoint.y))
            {
                viewHandle->GetGui()->SetPoint1(warpedPoint.x, warpedPoint.y);

                _arrow->CreateArrowInstance(glm::vec2(warpedPoint.x, warpedPoint.y), glm::vec2(warpedPoint.x, warpedPoint.y), viewHandle->GetGui()->GetArrowNewColor());
                viewHandle->GetGui()->SetMouseSelect(2);
            }
        }
        else if (viewHandle->GetGui()->GetArrowCreation() && viewHandle->GetGui()->GetMouseSelect() == 2)
        {
            viewHandle->GetGui()->SetPoint2(warpedPoint.x, warpedPoint.y);
            _arrow->SetDisplayParameters(viewHandle->GetGui()->p1, glm::vec2(warpedPoint.x, warpedPoint.y), _arrow->instancesCnt - 1);
            _arrow->SetPoints(viewHandle->GetGui()->p1, glm::vec2(warpedPoint.x, warpedPoint.y), _arrow->instancesCnt - 1);
            viewHandle->GetGui()->SetMouseSelect(0);
            viewHandle->GetGui()->SetArrowCreation(false);
        }
    }
    else if (viewHandle->GetGui()->GetArrowCreation() && viewHandle->GetGui()->GetMouseSelect() == 2)
    {
        _arrow->SetDisplayParameters(viewHandle->GetGui()->p1, glm::vec2(warpedPoint.x, warpedPoint.y), _arrow->instancesCnt - 1);
        _arrow->SetPoints(viewHandle->GetGui()->p1, glm::vec2(warpedPoint.x, warpedPoint.y), _arrow->instancesCnt - 1);
    }



    if (viewHandle->GetGui()->GetNewTeamColorSelected())
    {
        int jerseyColor = viewHandle->GetGui()->GetTeam1Color();
        if (jerseyColor != -1)
        {
            match->teams[0].SetTeamColor(jerseyColor);
            for (int i = 0;i < _circle->instances.size()/2; i++)
            {
                _circle->SetMaterial(jerseyColor, i);
                _arrow->SetMaterial(jerseyColor, i);
            }
        }
        jerseyColor = viewHandle->GetGui()->GetTeam2Color();
        if (jerseyColor != -1)
        {
            match->teams[1].SetTeamColor(jerseyColor);
            for (int i = int(_circle->instances.size()/2); i < _circle->instances.size(); i++)
            {
                _circle->SetMaterial(jerseyColor, i);
                _arrow->SetMaterial(jerseyColor, i);
            }
        }

        viewHandle->GetGui()->SetNewTeamColorSelected(false);
    }




    // image analyzer finds player positions and creates mask
    if (match != nullptr)
    {
        // load player positions
        viewHandle->GetVideoHandler()->FindPlayersPositions(*(match), *(viewHandle->GetGui()));
        // perform matching
        match->MatchPositionsToPlayers((int)viewHandle->GetVideoHandler()->GetFrameCount());
        // sets up model matrices for player objects
        MovePlayerHightlits(match->playerPositions);
    }
}


/**
* @brief Functions calibrates view matrix based on SolvePnP result
*/
void Controller::RefineViewMatrix()
{
    float sensitivity[3] = { 1.f,1.f,.1f };
    double minError = DBL_MAX;
    double newError = DBL_MAX;
    _distance = 0.0f, _rotX = 0.0f, _posY = 0.0f;
    std::cout << "Distance: " << _distance << std::endl;
    std::cout << "rotX: " << _rotX << std::endl;
    std::cout << "posY: " << _posY << std::endl;
    glm::mat4 proj = viewHandle->GetPerspectiveCamera()->getProjMatrix();
    glm::mat4 view = viewHandle->GetPerspectiveCamera()->getVideoViewMatrix();
    glm::mat4 viewConst = view;


    //eliminate unnecessary rotation
    view[0][0] = 1.f;
    view[0][1] = 0.f;
    view[0][2] = 0.f;
    view[0][3] = 0.f;
    view[1][0] = 0.f;
    view[2][0] = 0.f;
    view[3][1] *= -1.f;
    view[3][3] = 1.f;

    // test error and load points
    viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, newError);

    // image coordinates of pitch corners from manual input
    std::vector<cv::Point2f> imagePoints = viewHandle->GetVideoHandler()->GetPitchScreenPoints();

    // TestViewMatrix must init projectedPoints based on view matrix
    std::vector<cv::Point2f> imagePointsProjectedInit = viewHandle->GetVideoHandler()->GetProjectedPoints();


    _rotX = GetRotationToParallel(imagePoints, imagePointsProjectedInit);

    // Propoesed algorithm for view matrix improvement, can be enabled/disabled by gui
    if (viewHandle->GetGui()->GetCalibrationType() == 2)
    {
        while (1)
        {
            CalibrationBestStep(proj, view, _distance, _rotX, _posY, sensitivity, newError);

            if (newError < minError)
                minError = newError;

            if (sensitivity[0] < 0.000001f || sensitivity[1] < 0.000001f || sensitivity[2] < 0.0000001f)
                break;
        }
    }

    rotateXMat(viewConst, _rotX);
    addYposMat(viewConst, -_posY);
    addDistanceMat(viewConst, _distance);
    viewHandle->GetPerspectiveCamera()->setVideoViewMatrix(viewConst);


    // setup for animation not part of the algorithm
    _rotX = glm::degrees(atan2(sqrt(viewConst[2][1] * viewConst[2][1] + viewConst[2][0] * viewConst[2][0]), viewConst[2][2]));
    _posY = -viewConst[3][1];
    _distance = viewConst[3][2];


    viewConst = glm::translate(glm::mat4(1.f), glm::vec3(0.f, _posY, _distance))
        * glm::rotate(glm::mat4(1.f), glm::radians(_rotX), glm::vec3(1.f, 0.f, 0.f));

    viewHandle->GetPerspectiveCamera()->setVideoViewMatrix(viewConst);


    float fps = (float)viewHandle->GetVideoHandler()->GetFPS() * 2.f;
    videoAnimParams = { -initViewParams[0], initViewParams[1], initViewParams[2],
         (_distance - initViewParams[0]) / fps, (initViewParams[1] - _rotX) / fps, (initViewParams[2] - _posY) / fps
    };


}

/**
* @brief Functions best actualization step
*/
void Controller::CalibrationBestStep(glm::mat4 proj, glm::mat4 view,
    float& distance, float& rotX, float& posY, float* sensitivity, double& currentError)
{
    glm::mat4 viewConst = view;
    double errorTmp = 1000.f;
    std::vector<double> errors = { 1000, 1000, 1000 };
    // distance
    float tmpdistance = distance + sensitivity[0];
    rotateXMat(view, rotX);
    addYposMat(view, posY);
    addDistanceMat(view, tmpdistance);
    viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, errorTmp);

    view = viewConst;
    tmpdistance = distance - sensitivity[0];
    rotateXMat(view, rotX);
    addYposMat(view, posY);
    addDistanceMat(view, tmpdistance);
    viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, errors[0]);
    if (errorTmp < errors[0])
    {
        errors[0] = errorTmp;
        tmpdistance = distance + sensitivity[0];
    }

    // posY
    view = viewConst;
    float tmpPosY = posY + sensitivity[1];
    rotateXMat(view, rotX);
    addYposMat(view, tmpPosY);
    addDistanceMat(view, distance);
    viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, errorTmp);

    view = viewConst;
    tmpPosY = posY - sensitivity[1];
    rotateXMat(view, rotX);
    addYposMat(view, tmpPosY);
    addDistanceMat(view, distance);
    viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, errors[1]);
    if (errorTmp < errors[1])
    {
        errors[1] = errorTmp;
        tmpPosY = posY + sensitivity[1];
        view = viewConst;
        rotateXMat(view, rotX);
        addYposMat(view, tmpPosY);
        addDistanceMat(view, distance);
        viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, errors[1]);
    }

    std::vector<double>::iterator idxIterator = std::min_element(std::begin(errors), std::end(errors));
    int idx = (int)std::distance(std::begin(errors), idxIterator);

    if (errors[idx] < currentError)
    {
        if (idx == 0)
        {
            distance = tmpdistance;
        }
        else if (idx == 1)
        {
            posY = tmpPosY;
        }
        currentError = errors[idx];

        // rotation
        view = viewConst;
        double err1 = currentError, err2 = 1000.f;
        addYposMat(view, posY);
        addDistanceMat(view, distance);
        rotateXMat(view, rotX);
        viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, err1);

        std::vector<cv::Point2f> imagePoints = viewHandle->GetVideoHandler()->GetPitchScreenPoints();
        std::vector<cv::Point2f> imagePointsProjected = viewHandle->GetVideoHandler()->GetProjectedPoints();
        float rotationTheta = GetRotationToParallel(imagePoints, imagePointsProjected);

        (GetChangeType(ROT_X, proj, view, rotationTheta, err1, err2)) ? rotX += rotationTheta : rotX -= rotationTheta;

    }
    else
    {
        sensitivity[idx] /= 10.0f;
    }

}


bool Controller::GetChangeType(int type, glm::mat4 proj, glm::mat4 view, float sensitivity, double totalError, double totalErrorNew)
{
    glm::mat4 tmp = view;
    double tmpError = 1000;

    viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, totalError);


    // test positive change
    if (type == DISTANCE)
        addDistanceMat(view, sensitivity);
    else if (type == ROT_X)
        rotateXMat(view, sensitivity);
    else if (type == POS_Y)
        addYposMat(view, sensitivity);

    viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, totalErrorNew);

    // test negative change
    if (type == DISTANCE)
        addDistanceMat(tmp, -sensitivity);
    else if (type == ROT_X)
        rotateXMat(tmp, -sensitivity);
    else if (type == POS_Y)
        addYposMat(tmp, -sensitivity);

    viewHandle->GetVideoHandler()->TestViewMatrix(proj * tmp, tmpError);

    if (totalErrorNew < totalError)
    {
        return true;
    }
    else if (totalErrorNew < tmpError)
    {
        return true;
    }

    return false;
}



/**
* @brief Functions calculates angle between 2 lines
*/
float Controller::GetRotationToParallel(std::vector<cv::Point2f> line1, std::vector<cv::Point2f> line2)
{
    // only size 2 and 4 for both vectors accepted
    if (!(line1.size() == 2 || line1.size() == 4) && line1.size() == line2.size())
    {
        throw("Rotation to make lines parallel has invalid input!");
    }

    cv::Point2f dAB1 = cv::Point2f(line1[1].x - line1[0].x, line1[1].y - line1[0].y);
    cv::Point2f dCD1 = cv::Point2f(line2[1].x - line2[0].x, line2[1].y - line2[0].y);
    float theta = glm::degrees(atan2(dAB1.x * dCD1.y - dAB1.y * dCD1.x, dAB1.x * dCD1.x + dAB1.y * dCD1.y));
    return theta;
}


/**
* @brief Function loads video frame or initializes a new video
*/
void Controller::LoadVideoFrame(bool tests)
{
    // Draw video texture as a background frame and generate player mask
    if (viewHandle->GetGui()->GetVideoFileUsed() && !viewHandle->GetGui()->GetNewVideoFileSelected())
    {
        // all fragments should pass the stencil test
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        if (!viewHandle->GetVideoHandler()->InitVideoFrame(*(viewHandle->GetGui()), tests))
        {
            // stop exectuion of video loop if video ends
            // later edit logical values, this ensures proper app flow
            viewHandle->GetGui()->SetVideoFileUsed(true);
            viewHandle->GetGui()->SetVideoFileEnded(true);
            viewHandle->GetGui()->SetNewVideoFileSelected(true);
        }
    }
    else if (viewHandle->GetGui()->GetFilePathName().length() > 0)
    {
        if (match != nullptr)
            delete match;
        match = new Match();

        // init new video
        viewHandle->GetGui()->SetNewVideoFileSelected(false);

        if (viewHandle->GetGui()->GetVideoFileEnded())
        {
            viewHandle->GetGui()->SetVideoFileEnded(false);
        }
        else {
            viewHandle->GetGui()->SetVideoFileUsed(true);
        }

        viewHandle->GetVideoHandler()->InitNewVideo(viewHandle->GetGui()->GetFilePathName());

        // resize window based on dimension ratio of the video
        int videoWidth = viewHandle->GetVideoHandler()->GetVideoWidth();
        int videoHeight = viewHandle->GetVideoHandler()->GetVideoHeight();
        float videoDim = (float)videoWidth / videoHeight;
        float windowDim = (float)viewHandle->width / viewHandle->height;

        // resize if aspect ratios are not the same
        if (fabs(videoDim - windowDim) > 0.001)
            viewHandle->height = static_cast<int>(viewHandle->width / videoDim);

        glfwSetWindowSize(viewHandle->GetWindow(), viewHandle->width, viewHandle->height + VIEWPORT_YOFFSET);
        viewHandle->GetGui()->SetWindowSize((float)viewHandle->width, (float)viewHandle->height);
        viewHandle->GetVideoHandler()->InitVideoFrame(*(viewHandle->GetGui()));
    }
}


/**
* @brief Function resizes pitch based on input values
*/
void Controller::ResizeField()
{
    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f))
        * glm::scale(glm::mat4(1.0f), glm::vec3((float)viewHandle->GetGui()->GetFieldLength() / 2.f, 1.f, (float)viewHandle->GetGui()->GetFieldWidth() / 2.f))), 0);

    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(-(float)viewHandle->GetGui()->GetFieldLength() / 2.f + 16.5f, .01f, 0.f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 1.f, 20.16f))), 1);
    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3((float)viewHandle->GetGui()->GetFieldLength() / 2.f - 16.5f, .01f, 0.f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 1.f, 20.16f))), 2);

    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(-(float)viewHandle->GetGui()->GetFieldLength() / 2.f + 8.25f, .01f, 20.16f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(8.25, 1.f, 0.1f))), 3);
    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(-(float)viewHandle->GetGui()->GetFieldLength() / 2.f + 8.25f, .01f, -20.16f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(8.25, 1.f, 0.1f))), 4);
    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3((float)viewHandle->GetGui()->GetFieldLength() / 2.f - 8.25f, .01f, 20.16f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(8.25, 1.f, 0.1f))), 5);
    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3((float)viewHandle->GetGui()->GetFieldLength() / 2.f - 8.25f, .01f, -20.16f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(8.25, 1.f, 0.1f))), 6);
    _soccerPitch->SetObjectMat(glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.01f, 0.f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 1.f, (float)viewHandle->GetGui()->GetFieldWidth() / 2.f))), 8);
}

void Controller::RenderQuad()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


/**
* @brief Pitch poloautomatic animation
*/
glm::mat4 Controller::VideoInitializationAnimation()
{
    double time = glfwGetTime();
    viewHandle->GetGui()->SetMatrixType(2);
    for (int i = 0; i < _soccerPitch->instances.size(); i++)
    {
        _soccerPitch->instances.at(i).active = true;
    }

    if (!videoAnimParams.running)
    {
        videoAnimParams.running = true;
        afterAnimationDelay = 2;
        videoAnimParams.start = glfwGetTime();
    }
    else if (videoAnimParams.distance < _distance) {
        videoAnimParams.distance -= videoAnimParams.dd;
        if (videoAnimParams.distance > _distance)
            videoAnimParams.distance = _distance;
        if (videoAnimParams.rotX < _rotX)
            videoAnimParams.end = glfwGetTime();
    }
    else if (videoAnimParams.rotX > _rotX)
    {
        videoAnimParams.rotX -= videoAnimParams.dx;
        if (videoAnimParams.rotX < _rotX)
            videoAnimParams.rotX = _rotX;
        if(videoAnimParams.posY > _posY)
            videoAnimParams.end = glfwGetTime();
    }
    else if (videoAnimParams.posY < _posY)
    {
        videoAnimParams.posY -= videoAnimParams.dy;
        if (videoAnimParams.posY > _posY)
            videoAnimParams.posY = _posY;

        videoAnimParams.end = glfwGetTime();
    }
    else if (videoAnimParams.end + afterAnimationDelay > time)
    {
        videoAnimParams.running = false;
        viewHandle->GetGui()->SetVideoSelectMenuConfirmed(false);
        viewHandle->GetGui()->SetMatrixType(1);
        viewHandle->GetGui()->SetPlay(true);
        viewHandle->GetGui()->SetPlaneShow(false);
    }

    std::cout << videoAnimParams.rotX << std::endl;
    std::cout << videoAnimParams.posY << std::endl;
    std::cout << videoAnimParams.distance << std::endl;

    return glm::translate(glm::mat4(1.f), glm::vec3(0.f, videoAnimParams.posY, videoAnimParams.distance))
        * glm::rotate(glm::mat4(1.f), glm::radians(videoAnimParams.rotX), glm::vec3(1.f, 0.f, 0.f));

}

/**
* @brief Function Loads manually created saved settings
*/
void Controller::ApplyDemoSettings()
{
    if (viewHandle->GetGui()->GetSavedSettings())
    {
        std::string newFilePathName = viewHandle->GetGui()->GetFilePathName();
        if (newFilePathName.find("\\01.mp4") != std::string::npos
            || newFilePathName.find("/01.mp4") != std::string::npos)
        {
            viewHandle->GetPerspectiveCamera()->SetExternalParams(58.42f, 30.42f, 0.f, glm::vec3(0.f, 7.72f, 0.f));
            viewHandle->GetGui()->SetFieldLength(100);
            viewHandle->GetGui()->SetFieldWidth(64);
            match->teams[0].SetTeamColor(COLORS::Blue);
            match->teams[1].SetTeamColor(COLORS::White);
        }
        else if (newFilePathName.find("\\02.mp4") != std::string::npos
            || newFilePathName.find("/02.mp4") != std::string::npos)
        {
            viewHandle->GetPerspectiveCamera()->SetExternalParams(65.86f, 35.97f, 0.f, glm::vec3(0.f, 7.8f, 0.f));
            viewHandle->GetGui()->SetFieldLength(134);
            viewHandle->GetGui()->SetFieldWidth(64);
            match->teams[0].SetTeamColor(COLORS::Blue);
            match->teams[1].SetTeamColor(COLORS::White);
        }
        else if (newFilePathName.find("\\03.mp4") != std::string::npos
            || newFilePathName.find("/03.mp4") != std::string::npos)
        {
            viewHandle->GetPerspectiveCamera()->SetExternalParams(67.94f, 27.3f, 0.f, glm::vec3(0.f, 9.77f, 0.f));
            viewHandle->GetGui()->SetFieldLength(133);
            viewHandle->GetGui()->SetFieldWidth(80);
            match->teams[0].SetTeamColor(COLORS::Red);
            match->teams[1].SetTeamColor(COLORS::White);
        }
    }
}

/**
* @brief Slows video play speed based on video fps
*        in case of video frames being proccessed too fast
*/
void Controller::LockFrameRate(double frame_rate, bool tests)
{
    // Note: frame_start_time is called first thing in the main loop
    _frameEndTime = glfwGetTime();  // in seconds
    _frameDrawTime = _frameEndTime - _frameStartTime;

    static double allowedFrameTime = 1.0 / frame_rate;
    double sleep_time = 0.0;

    if (!tests && _frameDrawTime < allowedFrameTime) {
        sleep_time = allowedFrameTime - _frameDrawTime;
        // multiplatform sleep command
        #ifdef LINUX
                usleep(1000 * sleepMs);
        #endif
        #ifdef WINDOWS
                Sleep((DWORD)(1000 * sleep_time));
        #endif
    }

    if (tests)
    {
        // Debug stuff
        double potentialFPS = 1.0 / _frameDrawTime;
        double actualFPS = 1.0 / (glfwGetTime() - _frameStartTime);
        std::cout << std::fixed << std::setprecision(2)
            << "Draw: " << _frameDrawTime * 1000 << "ms"
            << "  Sleep: " << sleep_time
            << "  Pot.FPS: " << potentialFPS
            << "  actualFPS: " << actualFPS
            << "  Frame end time: " << _frameEndTime << std::endl;
    }
}



/**
* @brief Application performance tests runs main loop with performance analysis
*/
void Controller::runPerformanceTest(char** argv)
{
    double video_start_time = glfwGetTime();

    std::cout << "Results will be shown at the end of the video." << std::endl;
    std::cout << "Performance tests started..." << std::endl;

    // setup performance tests with some graphics
    GenerateGraphicObjects();
    _offsideLine->SetActive(true);

    _arrow->CreateArrowInstance(glm::vec2(35,16), glm::vec2(20,-32), COLORS::Blue);
    _arrow->CreateArrowInstance(glm::vec2(16, 5), glm::vec2(0,30), COLORS::White);
    _arrow->CreateArrowInstance(glm::vec2(-16, -20), glm::vec2(30,30), COLORS::Red);
    _arrow->CreateArrowInstance(glm::vec2(25, 0), glm::vec2(-30,-30), COLORS::Orange);
    _arrow->CreateArrowInstance(glm::vec2(-15, -8), glm::vec2(-30,0), COLORS::Green);
    _arrow->CreateArrowInstance(glm::vec2(-30,25), glm::vec2(40,35), COLORS::Yellow);
    _arrow->CreateArrowInstance(glm::vec2(10,10), glm::vec2(20,20), COLORS::Gold);
    _arrow->CreateArrowInstance(glm::vec2(-20,-20), glm::vec2(-20,20), COLORS::Pink);


    std::string dir(argv[0]);
    std::string path = dir.substr(0, dir.find_last_of("/\\"));
    viewHandle->GetGui()->SetFilePathName(path + "\\graphics\\videos\\01.mp4");
    viewHandle->GetGui()->SetNewVideoFileSelected(true);
    viewHandle->GetGui()->SetVideoSelectMenuConfirmed(true);
    viewHandle->GetGui()->SetSelectedPitchPointsCount(4);
    viewHandle->GetGui()->SetMatrixType(2);

    viewHandle->GetPerspectiveCamera()->setDistance(initViewParams[0]);
    viewHandle->GetPerspectiveCamera()->setXRotation(initViewParams[1]);
    PerformanceUnit PUguidraw, PUcalibTime, PUVideoLoad, PUgraphicsPrepare, PUdepthRender,
        PUdepthView, PUdrawBackground, PUrenderObjects, PUrenderMask, PUwaitTime, PUframe;

    // main loop
    while (!glfwWindowShouldClose(viewHandle->GetWindow()))
    {
        double timeGap = 0;
        double timeDiff = 0;

        // load current time and scene
        _frameStartTime = glfwGetTime();

        // Clear color and depth buffers
        glViewport(0, 0, viewHandle->width, viewHandle->height);
        glEnable(GL_DEPTH_CLAMP);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // render gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        viewHandle->GetGui()->DebugWindowDraw();
        viewHandle->GetGui()->ArrowMenu(viewHandle->GetVideoHandler()->GetTime(), *(_arrow));
        viewHandle->GetGui()->MainMenu(viewHandle->GetVideoHandler()->GetTime(), *(_soccerPitch), currentViewError / 4.0);
        viewHandle->GetGui()->OffsideLineMenu(viewHandle->GetVideoHandler()->GetTime(), *(_offsideLine));
        viewHandle->GetGui()->MatchMenu(*(_circle), *(_arrow));
        viewHandle->GetGui()->NewVideoMenu(currentViewError / 4.0);
        ImGui::End();

        timeDiff = glfwGetTime() - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUguidraw, timeDiff);

        if (viewHandle->GetGui()->GetRecalculateProjections())
        {
            if (viewHandle->GetGui()->GetSavedSettings())
            {
                ApplyDemoSettings();
                viewHandle->GetVideoHandler()->LoadDemoVideoPoints(viewHandle->GetGui()->GetFilePathName());
                viewHandle->GetGui()->SetSavedSettings(false);
            }
            ResizeField();
            viewHandle->GetVideoHandler()->SetFieldParametes((float)viewHandle->GetGui()->GetFieldWidth(), (float)viewHandle->GetGui()->GetFieldLength());
            viewHandle->GetVideoHandler()->CalculateHomographyMatrix();
            viewHandle->GetPerspectiveCamera()->setVideoViewMatrix(viewHandle->GetVideoHandler()->CalculateVideoViewMatrix());

            if (viewHandle->GetGui()->GetCalibrationType())
            {
                RefineViewMatrix();
            }

            viewHandle->GetGui()->SetRecalculateProjections(false);

            timeDiff = glfwGetTime() - timeGap - _frameStartTime;
            timeGap += timeDiff;

            PerformanceUnitAddSample(PUcalibTime, timeDiff);
        }


        // load matrices  
        glm::mat4 proj = viewHandle->GetPerspectiveCamera()->getProjMatrix();
        glm::mat4 view = glm::mat4(1.f);
        if (viewHandle->GetGui()->GetVideoSelectMenuConfirmed())
            view = VideoInitializationAnimation();
        else if (viewHandle->GetGui()->GetMatrixType() == 0)
            view = viewHandle->GetPerspectiveCamera()->getViewMatrix();
        else
            view = viewHandle->GetPerspectiveCamera()->getVideoViewMatrix();

        glm::vec3 camera = glm::vec3(glm::inverse(view) * glm::vec4(0.f, 0.f, 0.f, 1.f));


        // shows error in each frame
        viewHandle->GetVideoHandler()->TestViewMatrix(proj * view, currentViewError);

        // depth shader setup
        glm::mat4 lightProj = glm::ortho(-100.f, 100.f, -40.f, 40.f, 0.1f, 168.f);
        glm::mat4 lightView = glm::lookAt(glm::vec3(_light.x, -_light.y, _light.z), glm::vec3(0.f, 0.f, 20.f), glm::vec3(0.0, 1.f, 0.0));
        glm::mat4 lightSpaceMat = lightProj * lightView;


        // Loads video frame
        LoadVideoFrame(true);

        if (viewHandle->GetGui()->GetVideoFileEnded())
            break;


        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUVideoLoad, timeDiff);


        // Prepares model mats for scene rendering
        SetGraphicObjects();
        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUgraphicsPrepare, timeDiff);


        //Depth PreRendering disabled for not functioning
        glCullFace(GL_BACK);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.f, -1.f);
        _depthShader->use();
        glViewport(0, 0, viewHandle->width, viewHandle->height * 3);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        _depthShader->setMat4("lightSpaceMat", lightSpaceMat);
        RenderScene(_depthShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);


        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUdepthRender, timeDiff);


        glCullFace(GL_FRONT);
        glDisable(GL_POLYGON_OFFSET_FILL);

        // Shadow Texture debug
        glViewport(0, 0, viewHandle->width / 3, viewHandle->height / 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _depthViewShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        RenderQuad();
        glUseProgram(0);

        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUdepthView, timeDiff);

        // draws background video frame
        glViewport(0, 0, viewHandle->width, viewHandle->height);

        if (viewHandle->GetGui()->GetViewType() != THRESHOLDVIDEO)
            viewHandle->GetVideoHandler()->DrawBackground();


        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUdrawBackground, timeDiff);

        // Scene Rendering
        _objectShader->use();

        // Pass uniform values into shaders
        _objectShader->setMat4("lightSpaceMat", glm::mat4(lightSpaceMat));
        _objectShader->setVec3("lightPos", glm::vec3(_light));
        _objectShader->setMat4("viewMat", view);
        _objectShader->setMat4("projMat", proj);
        _objectShader->setVec3("cameraPos", camera);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
       
        RenderScene(_objectShader);

        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUrenderObjects, timeDiff);

        // Stop using shader, Scene Rendering END
        glUseProgram(0);

        // Perform masking
        viewHandle->GetVideoHandler()->DrawMaskFrame(viewHandle->GetGui()->GetViewType());


        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUrenderMask, timeDiff);

        // Render gui window
        viewHandle->GetGui()->Render();

        // Lock frame rate for video FPS
        LockFrameRate(viewHandle->GetVideoHandler()->GetFPS());



        timeDiff = glfwGetTime() - timeGap - _frameStartTime;
        timeGap += timeDiff;
        PerformanceUnitAddSample(PUwaitTime, timeDiff);

        PerformanceUnitAddSample(PUframe, glfwGetTime() - _frameStartTime);
        // Swap window buffers
        glfwSwapBuffers(viewHandle->GetWindow());

        // Manage window events
        glfwPollEvents();
    }

    std::cout << "Total video time: " << glfwGetTime() - video_start_time << " seconds, frames: "<< PUframe.cnt << std::endl;


    std::cout << "Gui draw time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUguidraw) <<std::endl;
    std::cout << "Calibration time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUcalibTime) <<std::endl;
    std::cout << "Video load time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUVideoLoad) <<std::endl;
    std::cout << "Graphics prepare time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUgraphicsPrepare ) <<std::endl;
    std::cout << "DepthRender time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUdepthRender) <<std::endl;
    std::cout << "Depth view render time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUdepthView) <<std::endl;
    std::cout << "Draw background time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUdrawBackground) <<std::endl;
    std::cout << "Scene Render time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUrenderObjects) <<std::endl;
    std::cout << "Mask Render time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUrenderMask) <<std::endl;
    std::cout << "Sleep time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUwaitTime) << std::endl;
    std::cout << "Total frame time (ms) " << std::fixed << std::setprecision(4) << GetAverageUnitTime(PUframe) << std::endl;

    std::cout << "Pres Enter to exit." << std::endl;
    std::cin.get();
}

void Controller::PerformanceUnitAddSample(PerformanceUnit &unit, double time)
{ 
    unit.time.push_back(time*1000); 
    unit.cnt++; 
}

double Controller::GetAverageUnitTime(PerformanceUnit unit) {
    double sum = 0.0;
    std::for_each(unit.time.begin(), unit.time.end(), [&](double n) {
        sum += n;
    });
    // return in ms
    return sum / unit.cnt;
}