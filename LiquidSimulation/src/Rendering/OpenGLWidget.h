#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QtMath>
#include "Camera3D.h"
#include "Mesh.h"

struct InstancedDrawable
{
    static Mesh* mesh;
    QVector3D translation;
    QVector3D scale;
    QColor color;
};

struct Drawable
{
    Mesh* mesh;
    QVector3D translation;
    QVector3D scale;
    QColor color;
};

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = 0);
    ~OpenGLWidget();

    void addInstancedDrawable(InstancedDrawable* instancedDrawable);
    void addDrawable(Drawable* drawable);
    void cleanUp();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    QOpenGLVertexArrayObject m_instancedVao;
    QOpenGLBuffer m_instancedMeshBuffer;
    QOpenGLBuffer m_instancedTranslationBuffer;
    QOpenGLBuffer m_instancedScaleBuffer;
    QOpenGLBuffer m_instancedColorBuffer;
    QOpenGLShaderProgram* m_instancedShaderProgram;
    GLuint m_instancedViewMatrixUniform;
    GLuint m_instancedProjectionMatrixUniform;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_meshBuffer;
    QOpenGLShaderProgram* m_shaderProgram;
    GLuint m_translationUniform;
    GLuint m_scaleUniform;
    GLuint m_colorUniform;
    GLuint m_viewMatrixUniform;
    GLuint m_projectionMatrixUniform;

    QVector<InstancedDrawable*> m_instancedDrawables;
    QVector<Drawable*> m_drawables;

    Camera3D* m_camera;
    float m_isRightMouseDown = false;
    QPoint m_prevMousePosition;
    float m_horizontalCameraAngle = 0.f;
    float m_verticalCameraAngle = 90.f;
    float m_cameraDistance = 4.f;
    float m_cameraScrollSpeed = 0.0004f;
    float m_cameraMoveSpeed = 1.f;
};

#endif // OPENGLWIDGET_H
