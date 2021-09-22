#include "Videoloader.hpp"

/**
* @brief VideoLoader Constructor
* 
*/
VideoLoader::VideoLoader(std::string path)
{
    _shader = new Shader();
    _shader->compileShaders(path + "\\shaders\\BackgroundTextures.vert", path + "\\shaders\\BackgroundTextures.frag");

    _shaderBlend = new Shader();
    _shaderBlend->compileShaders(path + "\\shaders\\BackgroundTexturesBlend.vert", path + "\\shaders\\BackgroundTexturesBlend.frag");

    _shaderShadow = new Shader();
    _shaderShadow->compileShaders(path + "\\shaders\\BackgroundTexturesShadow.vert", path + "\\shaders\\BackgroundTexturesShadow.frag");


    _kernelElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    mog2BackgSubs = cv::createBackgroundSubtractorMOG2(1000, 16, true);

    pitchScreenPoints.push_back(cv::Point(0,0));
    pitchScreenPoints.push_back(cv::Point(0,0));
    pitchScreenPoints.push_back(cv::Point(0,0));
    pitchScreenPoints.push_back(cv::Point(0,0));
   
    modelCorners.push_back(cv::Point2f(-50,  32 ));
    modelCorners.push_back(cv::Point2f(-50, -32 ));
    modelCorners.push_back(cv::Point2f( 50, -32 ));
    modelCorners.push_back(cv::Point2f( 50,  32 ));
}

/**
* @brief VideoLoader Destructor
*/
VideoLoader::~VideoLoader()
{
    ClearTextures();
    ClearTextureBuffers();
    delete _shader;
    delete _shaderBlend;
    delete _shaderShadow;
    delete _capture;
}


/**
 * Function inits new video, after video initiation function CreateTextureBuffers must be called separately
 */
void VideoLoader::InitNewVideo(std::string videoPath)
{
    if (_capture != nullptr)
    {
        delete _capture;
        ClearTextures();
        ClearTextureBuffers();
    }

    _capture = new cv::VideoCapture(videoPath);
    if (!_capture->isOpened()) 
        throw("Cannot open video: %s\n", videoPath);


    CreateTextureBuffers();

    _frameCount = 0;
    _firstFrameLoaded = false;
    _videoLastFrame = _capture->get(cv::CAP_PROP_FRAME_COUNT);
    _videoFPS = _capture->get(cv::CAP_PROP_FPS);
    if (_videoFPS < 0 && _videoFPS > 1000)
        _videoFPS = 25.0;



}

/**
* @brief Return video width
*/
int VideoLoader::GetVideoWidth()
{
    return (int)_capture->get(cv::CAP_PROP_FRAME_WIDTH);
}

/**
* @brief Return video height
*/
int VideoLoader::GetVideoHeight()
{
    return (int)_capture->get(cv::CAP_PROP_FRAME_HEIGHT);
}

/**
* @brief Return video fps value
*/
double VideoLoader::GetFPS()
{
    return _videoFPS;
}

void VideoLoader::LoadDemoVideoPoints(std::string videoPath)
{
    if (videoPath.find("\\01.mp4") != std::string::npos
        || videoPath.find("/01.mp4") != std::string::npos)
    {
        //hardcoded pixels for pitch screen points video 01.mp4
        pitchScreenPoints.clear();
        pitchScreenPoints.push_back(cv::Point((int)72, (int)368));
        pitchScreenPoints.push_back(cv::Point((int)666, (int)65));
        pitchScreenPoints.push_back(cv::Point((int)1256, (int)65));
        pitchScreenPoints.push_back(cv::Point((int)1850, (int)368));
    }
    else if (videoPath.find("\\02.mp4") != std::string::npos
        || videoPath.find("/02.mp4") != std::string::npos)
    {
        //hardcoded pixels for pitch screen points video 02.mp4
        pitchScreenPoints.clear();
        pitchScreenPoints.push_back(cv::Point((int)23, (int)380));
        pitchScreenPoints.push_back(cv::Point((int)583, (int)61));
        pitchScreenPoints.push_back(cv::Point((int)1335, (int)61));
        pitchScreenPoints.push_back(cv::Point((int)1897, (int)380));
    }
    else if (videoPath.find("\\03.mp4") != std::string::npos
        || videoPath.find("/03.mp4") != std::string::npos)
    {
        //hardcoded pixels for pitch screen points video 02.mp4
        pitchScreenPoints.clear();
        pitchScreenPoints.push_back(cv::Point((int)12, (int)302));
        pitchScreenPoints.push_back(cv::Point((int)705, (int)56));
        pitchScreenPoints.push_back(cv::Point((int)1236, (int)56));
        pitchScreenPoints.push_back(cv::Point((int)1911, (int)301));
    }
}


/**
* @brief Functions can resize video structure
*/
void VideoLoader::ResizeVideo(int windowWidth, int windowHeight)
{
    _windowWidth = windowWidth;
    _windowHeight = windowHeight;

    ClearTextures();
}

/**
* @brief Texture initialization
*/
void VideoLoader::CreateTextureBuffers()
{

    //increasing vertex position  value may be used for zooming to texture,
    //vertices recalculation system would be needed
    //if zooming to texture successfull, need to create zoom factor for object drawing and disable corner input 
    float vertices[] = {
        // positions    // texture coords
         1.f,  1.f,     1.f, 0.f, // top right
         1.f,  -1.f,    1.f, 1.f, // bottom right
         -1.f, -1.f,    0.f, 1.f, // bottom left
         -1.f, 1.f,     0.f, 0.f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

/**
* @brief Clears Texture Buffers
*/
void VideoLoader::ClearTextureBuffers()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

/**
* @brief Clears Textures
*/
void VideoLoader::ClearTextures()
{
    DeleteTexture(_thresholdTexture, _thresholdTextureInitialized);
    DeleteTexture(_backgroundTexture, _backgroundTextureInitialized);
}


/**
* @brief Deletes textures if it wont be used anymore
*/
void VideoLoader::DeleteTexture(GLuint textureId, bool &initialized)
{
    if (initialized)
    {
        glDeleteTextures(1, &textureId);
        initialized = false;
    }
}



/**
* @brief Converts OpenCv Mat to OpenGL texture
*/
void VideoLoader::MatToTexture(const cv::Mat& mat, GLuint& textureId, bool &initialized)
{
    try 
    {

        // Set input texture format
        GLenum inputColourFormat = GL_BGR;
        if (mat.channels() == 1)
        {
            inputColourFormat = GL_RED;
        }

        if (!initialized)
        {
            glEnable(GL_TEXTURE_2D);
            // Create Texture ID in Opengl
            GLuint newTextureId = 0;
            glGenTextures(1, &newTextureId);

            // Bind to texture handle
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, newTextureId);

            // Set texture interpolation methods for minification and magnification
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Set texture clamping method
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


            // Create the texture
            glTexImage2D(
                GL_TEXTURE_2D,     // Texture type
                0,                 // Pyramid level
                GL_RGB,            // Output color format
                _windowWidth,      // Image width
                _windowHeight,     // Image height
                0,                 // Width of border 0/1
                inputColourFormat, // Input format
                GL_UNSIGNED_BYTE,  // Image data type
                mat.ptr()          // Image data 
            );  
            textureId = newTextureId;
            initialized = true;
            glDisable(GL_TEXTURE_2D);
        }
        else
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                _windowWidth,
                _windowHeight,
                inputColourFormat,
                GL_UNSIGNED_BYTE,
                mat.ptr()
            );
            glDisable(GL_TEXTURE_2D);
        }

    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what();
    }

}


/**
* @brief Draws background frame
*/
bool VideoLoader::InitVideoFrame(Gui& gui, bool tests)
{
    double initvVidStartTime = glfwGetTime();
    double timeDiff = 0;
    double timeGap = 0;

    // if user enables video play, set video being continues
    // for last texture deleting and calculate new texture
    if (gui.GetVideoState() && _isPaused)
    {
        _isPaused = false;
        _isResumed = true;
    }
    // if video running calculate new texture
    else if (gui.GetVideoState() && !_isPaused)
    {
        _isResumed = false;
    }
    // if user stops video, load a new texture but disable deleting it
    else if (!gui.GetVideoState() && !_isPaused)
    {
        _isPaused = true;
        _pauseDetected = true;
    }
    // if video is stopped
    else if (!gui.GetVideoState() && _isPaused)
    {
        _pauseDetected = false;
    }
    (gui.GetEditingPitchPoints()) ? editingPitchPoints = true : editingPitchPoints = false;

    cv::Mat newFrame;
    // if video is running or pause was detected load a new frame
    if (!_isPaused || _pauseDetected || !_firstFrameLoaded)
    {
        // if video end and cannot load new frame free resources
        if (!_capture->read(newFrame))
        {
            return false;
        }
        _frameCount++;
    }
    else if (!editingPitchPoints && _backgroundTextureInitialized) {
        DrawTexture(_backgroundTexture, _backgroundTextureInitialized);
        return true;
    }


    if ( !( _isPaused && editingPitchPoints)  )
    {
        // resize video frame to window screen size, dimensions are kept the same
        frame = cv::Mat::zeros(_windowWidth, _windowHeight, CV_8UC3);
        cv::resize(newFrame, frame, cv::Size(_windowWidth, _windowHeight));
    }


    cv::Mat backgroundFrame = frame.clone();

    // Conected screen points and connected projected model points based on view estitmation
    //cv::line(backgroundFrame, pitchScreenPoints[0], pitchScreenPoints[1], cv::Scalar(0, 0, 255), 2);
    //cv::line(backgroundFrame, pitchScreenPoints[1], pitchScreenPoints[2], cv::Scalar(0, 0, 255), 2);
    //cv::line(backgroundFrame, pitchScreenPoints[2], pitchScreenPoints[3], cv::Scalar(0, 0, 255), 2);
    //cv::line(backgroundFrame, pitchScreenPoints[3], pitchScreenPoints[0], cv::Scalar(0, 0, 255), 2);

    //cv::line(backgroundFrame, projectedPoints[0], projectedPoints[1], cv::Scalar(255, 255, 255), 2);
    //cv::line(backgroundFrame, projectedPoints[1], projectedPoints[2], cv::Scalar(255, 255, 255), 2);
    //cv::line(backgroundFrame, projectedPoints[2], projectedPoints[3], cv::Scalar(255, 255, 255), 2);
    //cv::line(backgroundFrame, projectedPoints[3], projectedPoints[0], cv::Scalar(255, 255, 255), 2);


    // Stop generating new background if video is stopped
    if (!_isPaused || _pauseDetected || editingPitchPoints || !_firstFrameLoaded)
    {
        // player mask creation
        if (!_isPaused || _pauseDetected)
        {
            CreatePlayerMask(frame, _playerMask);
        }
        

        // clear pitch points
        if (gui.GetClearPitchPoints())
        {
            pitchScreenPoints.clear();
            gui.SetClearPitchPoints(false);
            gui.SetSelectedPitchPointsCount(0);
        }


        // display field points if they are being edited
        if (editingPitchPoints)
        {
            for (int i = 0; i < pitchScreenPoints.size(); i++)
            {
                cv::circle(backgroundFrame, cv::Point( (int)pitchScreenPoints[i].x, (int)pitchScreenPoints[i].y), 1, cv::Scalar(0, 255, 0), 1);
                cv::circle(backgroundFrame, cv::Point( (int)pitchScreenPoints[i].x, (int)pitchScreenPoints[i].y), 8, cv::Scalar(0, 255, 0), 2);
                if (i > 0)
                {
                    cv::line(backgroundFrame,
                        cv::Point( (int)pitchScreenPoints[(i - 1)].x, (int)pitchScreenPoints[(i - 1)].y),
                        cv::Point( (int)pitchScreenPoints[i].x, (int)pitchScreenPoints[i].y),
                        cv::Scalar(0, 0, 255),
                        1
                    );
                }
                if (i == 3)
                {
                    cv::line(backgroundFrame,
                        cv::Point( (int)pitchScreenPoints[i].x, (int)pitchScreenPoints[i].y),
                        cv::Point( (int)pitchScreenPoints[0].x, (int)pitchScreenPoints[0].y),
                        cv::Scalar(0, 0, 255),
                        1
                    );
                }
            }
        }

        MatToTexture(backgroundFrame, _backgroundTexture, _backgroundTextureInitialized);


        if(!_firstFrameLoaded)
            _firstFrameLoaded = true;
    }

    backgroundFrame.release();

    return true;
}


/**
* @brief Public function DrawBackround to perform backround draw when needed
*/
void VideoLoader::DrawBackground()
{
    // Set texture as background
    DrawTexture(_backgroundTexture, _backgroundTextureInitialized);
}

/**
* @brief Draws texture in a video dimensions
* @param textureId Gluint ID of a texture
*/
void VideoLoader::DrawTexture(GLint textureId, bool initialized)
{
    if (initialized)
    {
        glEnable(GL_TEXTURE_2D);
        glUseProgram(_shader->getShaderProg());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(0);
        glDisable(GL_TEXTURE_2D);
    }
}



/**
* @brief Draws mask frame into alpha buffer or shows into the screen for debbuing
*/
void VideoLoader::DrawMaskFrame(int viewType)
{
    if (viewType == THRESHOLDVIDEO)
    {
        glEnable(GL_TEXTURE_2D);
        DrawTexture(_thresholdTexture, _thresholdTextureInitialized);
        glDisable(GL_TEXTURE_2D);
    }
    else if (viewType == MASKINGVIDEO)
    {
        // For drawing overlay disable depth test
        glDisable(GL_DEPTH_TEST);

        // Finally, Draw foreground texture visible only in areas with high alpha.
        glUseProgram(_shaderBlend->getShaderProg());
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, _backgroundTexture);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, _thresholdTexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glEnable(GL_DEPTH_TEST);
        glUseProgram(0);

    }

    if (_maskCalculated)
    {
        //DeleteTexture(_thresholdTexture);
        _maskCalculated = false;
    }

    //DeleteTexture(_backgroundTexture);
}

/**
* @brief Draws shadows on background frame
*/
void VideoLoader::DrawShadowsOnBackground(GLint frameBufferId)
{

    // For drawing overlay disable depth test
    glDisable(GL_DEPTH_TEST);

    glUseProgram(_shaderShadow->getShaderProg());
    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, _backgroundTexture);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, frameBufferId);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        glEnable(GL_DEPTH_TEST);

    glUseProgram(0);
}



/**
* @brief Function creates bounding area around moving objects in soccer field area
*/
void VideoLoader::CreatePlayerMask(cv::Mat in, cv::Mat &out)
{
    cv::Mat fieldArea = cv::Mat::zeros(in.rows, in.cols, CV_8UC1);
    std::vector<cv::Point> pts;
    pts.push_back(cv::Point((int)pitchScreenPoints[0].x, (int)pitchScreenPoints[0].y));
    pts.push_back(cv::Point((int)pitchScreenPoints[1].x, (int)pitchScreenPoints[1].y));
    pts.push_back(cv::Point((int)pitchScreenPoints[2].x, (int)pitchScreenPoints[2].y));
    pts.push_back(cv::Point((int)pitchScreenPoints[3].x, (int)pitchScreenPoints[3].y));

    //create field mask
    cv::fillConvexPoly(fieldArea, pts, cv::Scalar(255, 255, 255));
    cv::Mat framemasked, playersDetected, thresh;
    cv::bitwise_and(in, in, framemasked, fieldArea);
    
    //_fieldArea
    MOG2(framemasked, playersDetected);
    cv::threshold(playersDetected, out, 254, 255, cv::THRESH_BINARY);
    //cv::dilate(thresh, out, _kernelElement);
}


/**
* @brief Functions calculates OpenGL view matrix based on SolvePnP problem
*/
glm::mat4 VideoLoader::CalculateVideoViewMatrix()
{
    bool pointsCorrect = true;
    int cnt = 0;
    
    // repeat calculation in case of getting nan/inf value
    do
    {
        cnt++;
        pointsCorrect = true; 
        if (cnt > 10)
            break;

        // reaarange poits for solvePnP input
        std::vector<cv::Point3f> modelPoints;
        std::vector<cv::Point2f> imagePoints;
        // x,y,z but must be entered as x,z,y because of opengl in result
        for (int i = 0; i < modelCorners.size(); i++)
        {
            modelPoints.push_back(cv::Point3f(modelCorners[i].x / scaleVal, 0.f, modelCorners[i].y / scaleVal));
        }
        for (int i = 0; i < pitchScreenPoints.size(); i++)
        {
            imagePoints.push_back(cv::Point2f(pitchScreenPoints[i].x, pitchScreenPoints[i].y));
        }

        // Create camera intrinsic matrix
        float intrinsicMatrixData[9] = {
        (float)_windowWidth,          0.f, (float)_windowWidth  / 2.f,
                0.f, (float)_windowHeight, (float)_windowHeight / 2.f,
                0.f,                  0.f,  1.f
        };

        // Create camera distortion matrix
        float distMatrixData[5] = {
            0, 0, 0,
            0, 0,
        };

        cv::Mat intrinsicMatrix = cv::Mat(3, 3, CV_32F, intrinsicMatrixData);
        cv::Mat distMatrix = cv::Mat(5, 1, CV_32F, distMatrixData);
        cv::Mat rotationMatrix, viewMatrix(4, 4, CV_64F);

        cv::Mat rvec = cv::Mat(3, 1, CV_64F);
        cv::Mat tvec = cv::Mat(3, 1, CV_64F);

        // calc rvec, tvec 
        cv::solvePnP(cv::Mat(modelPoints), cv::Mat(imagePoints), intrinsicMatrix, distMatrix, rvec, tvec, false, cv::SOLVEPNP_ITERATIVE);
        cv::Rodrigues(rvec, rotationMatrix);

        // gets camera position
        cv::Mat cam_pos = -rotationMatrix.t() * tvec;


        // R | t matrix
        // 0 | 1
        for (unsigned int row = 0; row < 3; ++row)
        {
            for (unsigned int col = 0; col < 3; ++col)
            {
                viewMatrix.at<double>(row, col) = rotationMatrix.at<double>(row, col);
            }
            viewMatrix.at<double>(row, 3) = tvec.at<double>(row, 0);
        }
        viewMatrix.at<double>(3, 3) = 1.0f;


        // Opengl and Opencv Y and Z axis are inverted
        cv::Mat cvToGl = cv::Mat::zeros(4, 4, CV_64F);
        cvToGl.at<double>(0, 0) = 1.0f;
        cvToGl.at<double>(1, 1) = -1.0f; // Invert the y axis
        cvToGl.at<double>(2, 2) = -1.0f; // invert the z axis
        cvToGl.at<double>(3, 3) = 1.0f;
        viewMatrix = cvToGl * viewMatrix;


        // opencv is row major and opengl is column major
        cv::Mat glViewMatrix = cv::Mat::zeros(4, 4, CV_64F);
        cv::transpose(viewMatrix, glViewMatrix);

        // fill glm matrix with opencv matrix data
        for (unsigned int row = 0; row < 4; ++row)
        {
            for (unsigned int col = 0; col < 4; ++col)
            {
                _videoViewMat[row][col] = (float)glViewMatrix.at<double>(row, col);
                if (!std::isfinite(_videoViewMat[row][col]))
                {
                    pointsCorrect = false;
                }
            }
        }

    } while (!pointsCorrect);


    return _videoViewMat;
}


/**
* @brief Function calculates total error of euclidian distance of inserted screen pitch corners
*        and corresponding scene pitch corner points based generated projection view matrix 
*/
void VideoLoader::TestViewMatrix(glm::mat4 projViewMatrix, double& totalError)
{
    totalError = 0.0;
    std::vector<cv::Point3f> modelPointsTmp;

    for (int i = 0; i < modelCorners.size(); i++)
    {
        // x,y,z but must be entered as x,z,y because of opengl in result
        modelPointsTmp.push_back(cv::Point3f(modelCorners[i].x, 0.f, modelCorners[i].y));
    }

    projectedPoints.clear();
    for (int i = 0; i < modelPointsTmp.size(); i++)
    {
        cv::Point2f point;
        CalcProjectedPointScreenPos(projViewMatrix, modelPointsTmp[i], point);
        projectedPoints.push_back(point);
        float errx = (pitchScreenPoints[i].x - projectedPoints[i].x);
        float erry = (pitchScreenPoints[i].y - projectedPoints[i].y);
        double errLenght = sqrt(pow(errx, 2) + pow(erry, 2));
        totalError += errLenght;

    }


}



/**
* @brief Function calculates position of scene point transformed to screen
*/
void VideoLoader::CalcProjectedPointScreenPos(glm::mat4 projViewMatrix, cv::Point3f inModelPoint, cv::Point2f& outScreenPoint)
{
    // create mat holders
    cv::Mat projViewCV = cv::Mat::zeros(4, 4, CV_64F);
    cv::Mat projPoint = cv::Mat::zeros(4, 1, CV_64F);
    cv::Mat projPointWarp = cv::Mat::zeros(3, 1, CV_64F);

    // convert point to homogenous coordinates
    cv::Mat modelPoint = cv::Mat::zeros(4, 1, CV_64F);
    modelPoint.at<double>(0, 0) = inModelPoint.x;
    modelPoint.at<double>(0, 1) = inModelPoint.y;
    modelPoint.at<double>(0, 2) = inModelPoint.z;
    modelPoint.at<double>(0, 3) = 1;

    for (unsigned int row = 0; row < 4; ++row)
    {
        for (unsigned int col = 0; col < 4; ++col)
        {
            // convert projView matrix to opencv
            projViewCV.at<double>(row, col) = projViewMatrix[row][col];
        }
    }
    // must be transposed because of transfer Opengl to Opencv
    projViewCV = projViewCV.t();


    // apply projection
    projPoint = projViewCV * modelPoint;
    // convert projected point from to NDC and screen space
    outScreenPoint.x = (float)round((projPoint.at<double>(0) / projPoint.at<double>(3) + 1) * 0.5 * _windowWidth);
    outScreenPoint.y = (float)round((1 - projPoint.at<double>(1) / projPoint.at<double>(3)) * 0.5 * _windowHeight);

}


/**
* @brief Function return homography transformation matrix in glm mat type
*/
glm::mat4 VideoLoader::GetTMatGLM()
{
    glm::mat4 glmTMat = glm::mat4(1.f);
    // fill glm matrix with opencv matrix data
    for (unsigned int row = 0; row < 3; ++row)
    {
        for (unsigned int col = 0; col < 3; ++col)
        {
            glmTMat[row][col] = (float)TMat(row, col);
        }
    }
    return glmTMat;
}



/**
* @brief Function finds scene positions of players based on detection mask
*/
void VideoLoader::FindPlayersPositions(Match& match, Gui& gui)
{
    try
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(_playerMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        std::vector<std::vector<cv::Point>> contours_poly(contours.size());

        cv::Mat mask = cv::Mat::zeros(_playerMask.size(), CV_8UC1);
        cv::Mat playerImg;
        int kernelSize = 5;
        cv::Mat kernelElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));


        match.playerPositions.clear();
        match.playerPositions.resize(3);

        for (size_t i = 0; i < contours.size(); i++) {
            double countoursArea = cv::contourArea(contours[i], false);
            if (countoursArea > 30)
            {
                cv::approxPolyDP(contours[i], contours_poly[i], 3, true);

                cv::Rect rectPos = cv::boundingRect(contours_poly[i]);
                int x = rectPos.x;
                int y = rectPos.y;
                int w = rectPos.width;
                int h = rectPos.height;


                cv::Point2f warpedPoint = GetScreenToScenePoint(x + w / 2.f, (float)y + h);

                playerImg = cv::Mat::zeros(w, h, CV_8UC3);

                frame(cv::Rect(x, y, w, h)).copyTo(playerImg);
                cv::Mat playerHSV0, playerHSV1, mask1, res1, mask2, res2;

                cv::cvtColor(playerImg, playerHSV0, colors[match.teams[0].GetTeamColor()].type);

                // team 0 jersey
                cv::inRange(playerHSV0, colors[match.teams[0].GetTeamColor()].lowTh, colors[match.teams[0].GetTeamColor()].highTh, mask1);

                cv::bitwise_and(playerImg, playerImg, res1, mask1);
                cv::dilate(res1, res1, kernelElement);
                cv::cvtColor(res1, res1, cv::COLOR_BGR2GRAY);
                int nzCnt1 = cv::countNonZero(res1);
                if (nzCnt1 >= 15) {
                    cv::drawContours(mask, contours_poly, (int)i, cv::Scalar(255, 255, 255), -1);
                    match.playerPositions[0].push_back(glm::vec4(warpedPoint.x, warpedPoint.y, w, h));
                    continue;
                }

                // team 1 jersey        
                cv::cvtColor(playerImg, playerHSV1, colors[match.teams[1].GetTeamColor()].type);
                cv::inRange(playerHSV1, colors[match.teams[1].GetTeamColor()].lowTh, colors[match.teams[1].GetTeamColor()].highTh, mask2);
                cv::bitwise_and(playerImg, playerImg, res2, mask2);
                cv::dilate(res2, res2, kernelElement);
                cv::cvtColor(res2, res2, cv::COLOR_BGR2GRAY);
                int nzCnt2 = cv::countNonZero(res2);
                if (nzCnt2 >= 15) {
                    cv::drawContours(mask, contours_poly, (int)i, cv::Scalar(255, 255, 255), -1);
                    match.playerPositions[1].push_back(glm::vec4(warpedPoint.x, warpedPoint.y, w, h));
                    continue;
                }

                cv::drawContours(mask, contours, (int)i, cv::Scalar(255, 255, 255), -1);
                match.playerPositions[2].push_back(glm::vec4(warpedPoint.x, warpedPoint.y, w, h));
            }
        }

        // Create mask texture is video is playing, this doesn't produce a visible result, it just
        // changes the alpha values in the framebuffer
        if (!_isPaused || _pauseDetected)
        {
            MatToTexture(mask, _thresholdTexture, _thresholdTextureInitialized);
            _maskCalculated = true;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what();
    }
}



void VideoLoader::MOG2(cv::Mat& img, cv::Mat& mog2foregroundMask)
{
    mog2BackgSubs->apply(img, mog2foregroundMask);
}



/**
* @brief Function calculates Homography transformation matrix based on given screen positions of soccer field
*/
void VideoLoader::CalculateHomographyMatrix()
{
    TMat = getPerspectiveTransform(pitchScreenPoints, modelCorners);
}


/**
* @brief Function calculates and returns position of screen pixel in the scene
*/
cv::Point2f VideoLoader::GetScreenToScenePoint(float xpos, float ypos)
{
    cv::Point3f warpedPoint = TMat * cv::Point3f(xpos, ypos, 1);
    warpedPoint = warpedPoint * (1.0 / warpedPoint.z);
    return cv::Point2f(warpedPoint.x, warpedPoint.y);
}


/**
* @brief Function transforms field parameters into model points
*/
void VideoLoader::SetFieldParametes(float width, float length)
{
    modelCorners.clear();
    float hw = width / 2.f;
    float hl = length / 2.f;
    modelCorners.push_back(cv::Point2f(-hl, hw));
    modelCorners.push_back(cv::Point2f(-hl, -hw));
    modelCorners.push_back(cv::Point2f(hl, -hw));
    modelCorners.push_back(cv::Point2f(hl, hw));

}

/**
* @brief Function add a new pitch point
*/
void VideoLoader::AddPitchScreenPoint(int x, int y)
{
    pitchScreenPoints.push_back(cv::Point2f((float)x, (float)y));
}

/**
* @brief Function edits a new pitch point
*/
void VideoLoader::EditPitchScreenPoint(int index, int x, int y)
{
    if (index < pitchScreenPoints.size())
    {
        pitchScreenPoints[index] = cv::Point2f((float)x, (float)y);
        //int correspondingID = 3 - index;
        //pitchScreenPoints[correspondingID].y = (float)y;
    }
    ////for proper display side lines should have the same length
    //if (index == 0 || index == 1)
    //{
    //    int dx = pitchScreenPoints[1].x - pitchScreenPoints[0].x;
    //    if (index == 0)
    //        pitchScreenPoints[3].x = pitchScreenPoints[2].x + dx;
    //    else if (index == 1)
    //        pitchScreenPoints[2].x = pitchScreenPoints[3].x - dx;
    //}
    //else if (index == 2 || index == 3)
    //{
    //    int dx = pitchScreenPoints[3].x - pitchScreenPoints[2].x;
    //    if (index == 2)
    //        pitchScreenPoints[1].x = pitchScreenPoints[0].x + dx;
    //    else if (index == 3)
    //        pitchScreenPoints[0].x = pitchScreenPoints[1].x - dx;
    //}

}