#pragma once

#include <cstdlib>
#include <Graphics/GraphicsEngine.h>
#include <Graphics/GL45/WGL/WGLEngine.h>
#include <Graphics/GL45/GLSLProgramFactory.h>
#include <Graphics/PVWUpdater.h>

#include <Applications/OnIdleTimer.h>
#include <Applications/CameraRig.h>
#include <Applications/TrackBall.h>
#include <Applications/Environment.h>

#include <QtWidgets/QWidget>
#include "ui_MainApp.h"

using namespace gte;

class MainApp : public QWidget
{
    Q_OBJECT

public:
    MainApp(QWidget *parent = nullptr);
    ~MainApp();

    // Create the camera and camera rig.
    void InitializeCamera(float upFovDegrees, float aspectRatio, float dmin, float dmax,
        float translationSpeed, float rotationSpeed, std::array<float, 3> const& pos,
        std::array<float, 3> const& dir, std::array<float, 3> const& up);

    void CreateEngineAndProgramFactory();

    inline std::string GetGTEPath() const { return mEnvironment.GetGTEPath(); }

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    QPaintEngine* paintEngine()const { return 0; }

    bool SetEnvironment();
    bool CreateScene();

private:
    Ui::MainAppClass ui;

    // Standard window information.
    HWND mHandle;

    // Support for access to environment variables and paths.
    Environment mEnvironment;

    // The graphics engine and program factory are stored as base
    // class pointers to allow Application to be independent of the
    // corresponding graphics API subsystems.
    std::shared_ptr<BaseEngine> mBaseEngine;
    std::shared_ptr<ProgramFactory> mProgramFactory;

    // TODO: This is assigned mBaseEngine, which allows development of
    // the DX12 engine independently of DX11 and WGL. The DX12 engine
    // is a work in progress.
    std::shared_ptr<GraphicsEngine> mEngine;

    BufferUpdater mUpdater;
    std::shared_ptr<Camera> mCamera;
    CameraRig mCameraRig;
    PVWUpdater mPVWMatrices;
    TrackBall mTrackBall;

    OnIdleTimer mTimer;

    std::shared_ptr<Visual> mMesh;
};
