#ifndef CAMERA3D_H
#define CAMERA3D_H

#include <QVector3D>
#include <QMatrix4x4>

class Camera3D
{
public:
    Camera3D();
    Camera3D(int width, int height);

    void lookAt(const QVector3D& target);
    void setPosition(const QVector3D& position);
    void setFieldOfView(float fieldOfView);
    void setAspectRatio(float aspectRatio);
    void setAspectRatio(int width, int height);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

    QMatrix4x4 getViewMatrix() const;
    QMatrix4x4 getProjectionMatrix() const;

private:
    void recalcViewMatrix();
    void recalcProjectionMatrix();

    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    QVector3D m_position;
    QVector3D m_lookAtTarget;
    float m_aspectRatio;
    float m_fieldOfView;
    float m_nearPlane;
    float m_farPlane;
};

#endif // CAMERA3D_H
