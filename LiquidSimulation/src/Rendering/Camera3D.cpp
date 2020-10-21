#include "Rendering/Camera3D.h"

Camera3D::Camera3D() :
    Camera3D(800, 600)
{

}

Camera3D::Camera3D(int width, int height) :
    m_position(QVector3D(0.f, 0.f, 0.f)),
    m_lookAtTarget(QVector3D(0.f, 0.f, -1.f)),
    m_fieldOfView(45.f),
    m_aspectRatio(width / (float)height),
    m_nearPlane(0.001f),
    m_farPlane(9999.f)
{
    recalcViewMatrix();
    recalcProjectionMatrix();
}

void Camera3D::lookAt(const QVector3D& target)
{
    m_lookAtTarget = target;
    recalcViewMatrix();
}

void Camera3D::setPosition(const QVector3D& position)
{
    m_position = position;
    recalcViewMatrix();
}

void Camera3D::setFieldOfView(float fieldOfView)
{
    m_fieldOfView = fieldOfView;
    recalcProjectionMatrix();
}

void Camera3D::setAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
    recalcProjectionMatrix();
}

void Camera3D::setAspectRatio(int width, int height)
{
    setAspectRatio(width / (float)height);
}

void Camera3D::setNearPlane(float nearPlane)
{
    m_nearPlane = nearPlane;
    recalcProjectionMatrix();
}

void Camera3D::setFarPlane(float farPlane)
{
    m_farPlane = farPlane;
    recalcProjectionMatrix();
}

QMatrix4x4 Camera3D::getViewMatrix() const
{
    return m_viewMatrix;
}

QMatrix4x4 Camera3D::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

void Camera3D::recalcViewMatrix()
{
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(m_position, m_lookAtTarget, QVector3D(0.f, 1.f, 0.f));
}

void Camera3D::recalcProjectionMatrix()
{
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
}
