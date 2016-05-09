#include "widget3dcameracontroller.h"

#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DLogic/QFrameAction>

Widget3DCameraController::Widget3DCameraController(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , mCamera(Q_NULLPTR)
{
    mKeyboardDevice = new Qt3DInput::QKeyboardDevice(this);
    mMouseDevice = new Qt3DInput::QMouseDevice(this);
    mMouseDevice->setSensitivity(0.1);

    mLogicalDevice = new Qt3DInput::QLogicalDevice(this);

    mMoveAction = new Qt3DInput::QAction(this);
    Qt3DInput::QActionInput *input = new Qt3DInput::QActionInput(this);
    input->setSourceDevice(mMouseDevice);
    input->setButtons({ Qt::LeftButton });
    mMoveAction->addInput(input);
    mLogicalDevice->addAction(mMoveAction);

    mRotateAction = new Qt3DInput::QAction(this);
    input = new Qt3DInput::QActionInput(this);
    input->setSourceDevice(mMouseDevice);
    input->setButtons({ Qt::RightButton });
    mRotateAction->addInput(input);
    mLogicalDevice->addAction(mRotateAction);

    mZoomInAction = new Qt3DInput::QAction(this);
    input = new Qt3DInput::QActionInput(this);
    input->setSourceDevice(mKeyboardDevice);
    input->setButtons({ Qt::Key_Plus });
    mZoomInAction->addInput(input);
    mLogicalDevice->addAction(mZoomInAction);

    mZoomOutAction = new Qt3DInput::QAction(this);
    input = new Qt3DInput::QActionInput(this);
    input->setSourceDevice(mKeyboardDevice);
    input->setButtons({ Qt::Key_Minus });
    mZoomOutAction->addInput(input);
    mLogicalDevice->addAction(mZoomOutAction);

    mRotateXAxis = new Qt3DInput::QAxis(this);
    Qt3DInput::QAnalogAxisInput *axisInput = new Qt3DInput::QAnalogAxisInput(this);
    axisInput->setSourceDevice(mMouseDevice);
    axisInput->setAxis(Qt::XAxis);
    mRotateXAxis->addInput(axisInput);
    mLogicalDevice->addAxis(mRotateXAxis);

    mRotateYAxis = new Qt3DInput::QAxis(this);
    axisInput = new Qt3DInput::QAnalogAxisInput(this);
    axisInput->setSourceDevice(mMouseDevice);
    axisInput->setAxis(Qt::YAxis);
    mRotateYAxis->addInput(axisInput);
    mLogicalDevice->addAxis(mRotateYAxis);

    mTranslateXAxis = new Qt3DInput::QAxis(this);
    axisInput = new Qt3DInput::QAnalogAxisInput(this);
    axisInput->setSourceDevice(mMouseDevice);
    axisInput->setAxis(Qt::XAxis);
    mTranslateXAxis->addInput(axisInput);
    mLogicalDevice->addAxis(mTranslateXAxis);

    mTranslateYAxis = new Qt3DInput::QAxis(this);
    axisInput = new Qt3DInput::QAnalogAxisInput(this);
    axisInput->setSourceDevice(mMouseDevice);
    axisInput->setAxis(Qt::YAxis);
    mTranslateYAxis->addInput(axisInput);
    mLogicalDevice->addAxis(mTranslateYAxis);

    mFrameAction = new Qt3DLogic::QFrameAction(this);
    connect(mFrameAction, &Qt3DLogic::QFrameAction::triggered,
            this, &Widget3DCameraController::frameActionTriggered);

    addComponent(mLogicalDevice);
    addComponent(mFrameAction);
}

Widget3DCameraController::~Widget3DCameraController()
{
}

void Widget3DCameraController::setCamera(Qt3DRender::QCamera *camera)
{
    mCamera = camera;
    mUpVector = camera->upVector();
}

Qt3DRender::QCamera * Widget3DCameraController::camera() const
{
    return mCamera;
}

void Widget3DCameraController::frameActionTriggered(float dt)
{
    static const float sZoomFactor = 0.1f;
    static const float sTranslationSpeed = 150.0f;
    static const float sRotationSpeed = 260.0f;

    if (mZoomInAction->isActive() || mZoomOutAction->isActive()) {
        const float zoom = 1.0 + (mZoomOutAction->isActive() ? sZoomFactor : -sZoomFactor);
        const QVector3D viewVector = (mCamera->position() - mCamera->viewCenter()) * zoom;
        mCamera->setPosition(mCamera->viewCenter() + viewVector);
    } else if (mMoveAction->isActive()) {
        mCamera->translate(QVector3D(sTranslationSpeed * -mTranslateXAxis->value(),
                                     sTranslationSpeed * -mTranslateYAxis->value(),
                                     0) * dt);
    } else if (mRotateAction->isActive()) {
        mCamera->panAboutViewCenter(sRotationSpeed * mRotateXAxis->value() * dt);
        mCamera->tiltAboutViewCenter(sRotationSpeed * mRotateYAxis->value() * dt);
        // lock the camera roll angle
        mCamera->setUpVector(mUpVector);
    }
}

