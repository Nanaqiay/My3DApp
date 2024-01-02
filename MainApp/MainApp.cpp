#pragma once

#include "MainApp.h"

#include <Graphics/MeshFactory.h>


MainApp::MainApp(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowState(Qt::WindowFullScreen);

    mHandle = (HWND)winId();


    CreateEngineAndProgramFactory();
}

MainApp::~MainApp()
{}

void MainApp::CreateEngineAndProgramFactory()
{
    auto engine = std::make_shared<gte::WGLEngine>(mHandle, true, false);
    if (!engine->MeetsRequirements())
    {
        LogError("OpenGL 4.5 or later is required.");
    }

    if (engine->GetDevice())
    {
        mEngine = engine;
        mBaseEngine = engine;

        mEngine = std::static_pointer_cast<GraphicsEngine>(mBaseEngine);

        mProgramFactory = std::make_shared<GLSLProgramFactory>();
        engine->DisplayColorBuffer(0);
    }
    else
    {
        LogError("Cannot create graphics engine.");
    }

    
    mUpdater = [this](std::shared_ptr<Buffer> const& buffer) { mEngine->Update(buffer); };
    mCamera = std::make_shared<Camera>(true, mEngine->HasDepthRange01());
    mCameraRig = CameraRig(mCamera, 0.0f, 0.0f);
    mPVWMatrices = PVWUpdater(mCamera, mUpdater);
    mTrackBall = TrackBall(500, 500, mCamera);


}

void MainApp::paintEvent(QPaintEvent* event)
{
    mTimer.Measure();

    mEngine->ClearBuffers();
    //mEngine->Draw(width() / 2, height() / 2, { 255.0f, 0.0f, 0.0f, 1.0f }, mTimer.GetFPS());
    mEngine->Draw(width() / 2, height() / 2, { 255.0f, 0.0f, 0.0f, 1.0f }, "TestCenter");
    mEngine->Draw(0, height() / 2, { 255.0f, 0.0f, 0.0f, 1.0f }, "Test1");
    mEngine->Draw(8, height() - 8, { 255.0f, 0.0f, 0.0f, 1.0f }, "Test2");

    mEngine->DisplayColorBuffer(0);

    mTimer.UpdateFrameCount();
}

void MainApp::InitializeCamera(float upFovDegrees, float aspectRatio, float dmin, float dmax,
    float translationSpeed, float rotationSpeed, std::array<float, 3> const& pos,
    std::array<float, 3> const& dir, std::array<float, 3> const& up)
{
    mCamera->SetFrustum(upFovDegrees, aspectRatio, dmin, dmax);
    Vector4<float> camPosition{ pos[0], pos[1], pos[2], 1.0f };
    Vector4<float> camDVector{ dir[0], dir[1], dir[2], 0.0f };
    Vector4<float> camUVector{ up[0], up[1], up[2], 0.0f };
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    mCameraRig.ComputeWorldAxes();
    mCameraRig.SetTranslationSpeed(translationSpeed);
    mCameraRig.SetRotationSpeed(rotationSpeed);
}