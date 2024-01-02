#pragma once

#include "Graphics/GL45/WGL/WGLEngine.h"
#include "Graphics/GL45/GLSLProgramFactory.h"
#include <Applications/GTApplicationsPCH.h>
#include <Applications/Environment.h>
#include <cstdlib>
#include <fstream>
#include <Graphics/GraphicsEngine.h>
#include <Applications/Application.h>
#include <Applications/OnIdleTimer.h>
#include <Applications/CameraRig.h>
#include <Applications/TrackBall.h>
#include <Graphics/PVWUpdater.h>

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

public:
    // Get the value of the GTE_PATH environment variable. Derived
    // classes may use this variable to ensure the existence of input
    // data sets that are required by an application. If the function
    // returns "", the GTE_PATH variable has not been set.
    inline std::string GetGTEPath() const
    {
        return mEnvironment.GetGTEPath();
    }

protected:
    virtual void paintEvent(QPaintEvent* event) override;


private:
    Ui::MainAppClass ui;



protected:
    // Support for access to environment variables and paths.
    Environment mEnvironment;

    // The graphics engine and program factory are stored as base
    // class pointers to allow Application to be independent of the
    // corresponding graphics API subsystems.
    std::shared_ptr<BaseEngine> mBaseEngine;
    std::shared_ptr<ProgramFactory> mProgramFactory;

private:

    // Standard window information.
    HWND mHandle;

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
};
