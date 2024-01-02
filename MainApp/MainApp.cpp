#pragma once

#include "MainApp.h"

#include <Graphics/MeshFactory.h>


MainApp::MainApp(QWidget *parent)
    : QWidget(parent)
{
    mMesh = nullptr;

    ui.setupUi(this);
    setGeometry(0, 0, 512, 512);

    mHandle = (HWND)winId();
    CreateEngineAndProgramFactory();

    if (!SetEnvironment() || !CreateScene())
    {
        return;
    }

    InitializeCamera(60.0f, width() / height(), 0.1f, 100.0f, 0.01f, 0.001f,
        { 0.0f, 0.0f, -2.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
    mPVWMatrices.Update();
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
    if (0)
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

    //mTimer.Measure();

    if (mCameraRig.Move())
    {
        mPVWMatrices.Update();
    }

    mEngine->ClearBuffers();
    mEngine->Draw(mMesh);
    //mEngine->Draw(8, width() - 8, { 0.0f, 0.0f, 0.0f, 1.0 }, mTimer.GetFPS());
    mEngine->DisplayColorBuffer(0);

    //mTimer.UpdateFrameCount();
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

bool MainApp::SetEnvironment()
{
    std::string path = GetGTEPath();
    if (path == "")
    {
        return false;
    }

    mEnvironment.Insert(path + "/Samples/Graphics/WireMesh/Shaders/");

    std::vector<std::string> inputs =
    {
        mEngine->GetShaderName("WireMesh.vs"),
        mEngine->GetShaderName("WireMesh.ps"),
        mEngine->GetShaderName("WireMesh.gs")
    };

    for (auto const& input : inputs)
    {
        if (mEnvironment.GetPath(input) == "")
        {
            LogError("Cannot find file " + input);
            return false;
        }
    }

    return true;
}

bool MainApp::CreateScene()
{
    std::string vsPath = mEnvironment.GetPath(mEngine->GetShaderName("WireMesh.vs"));
    std::string psPath = mEnvironment.GetPath(mEngine->GetShaderName("WireMesh.ps"));
    std::string gsPath = mEnvironment.GetPath(mEngine->GetShaderName("WireMesh.gs"));
    auto program = mProgramFactory->CreateFromFiles(vsPath, psPath, gsPath);
    if (!program)
    {
        return false;
    }

    auto parameters = std::make_shared<ConstantBuffer>(3 * sizeof(Vector4<float>), false);
    auto* data = parameters->Get<Vector4<float>>();
    data[0] = { 0.0f, 0.0f, 1.0f, 1.0f };  // mesh color
    data[1] = { 0.0f, 0.0f, 0.0f, 1.0f };  // edge color
    data[2] = { static_cast<float>(width()), static_cast<float>(height()), 0.0f, 0.0f };
    program->GetVertexShader()->Set("WireParameters", parameters);
    program->GetPixelShader()->Set("WireParameters", parameters);
    program->GetGeometryShader()->Set("WireParameters", parameters);

    auto cbuffer = std::make_shared<ConstantBuffer>(sizeof(Matrix4x4<float>), true);
    program->GetVertexShader()->Set("PVWMatrix", cbuffer);

    auto effect = std::make_shared<VisualEffect>(program);

    VertexFormat vformat;
    vformat.Bind(VASemantic::POSITION, DF_R32G32B32_FLOAT, 0);
    MeshFactory mf;
    mf.SetVertexFormat(vformat);
    mMesh = mf.CreateSphere(16, 16, 1.0f);
    mMesh->SetEffect(effect);

    mPVWMatrices.Subscribe(mMesh->worldTransform, cbuffer);

    mTrackBall.Attach(mMesh);
    mTrackBall.Update();
    return true;
}
