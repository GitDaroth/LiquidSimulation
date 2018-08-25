#include "OpenGLWidget.h"

Mesh* InstancedDrawable::mesh;

OpenGLWidget::OpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    m_instancedShaderProgram(NULL),
    m_shaderProgram(NULL),
    m_camera(new Camera3D())
{
    setFormat(QSurfaceFormat::defaultFormat());
}

OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();

    m_meshBuffer.destroy();
    m_vao.destroy();
    delete m_shaderProgram;

    m_instancedMeshBuffer.destroy();
    m_instancedTranslationBuffer.destroy();
    m_instancedScaleBuffer.destroy();
    m_instancedColorBuffer.destroy();
    m_instancedVao.destroy();
    delete m_instancedShaderProgram;

    delete m_camera;

    doneCurrent();
}

void OpenGLWidget::addInstancedDrawable(InstancedDrawable* instancedDrawable)
{
    m_instancedDrawables.append(instancedDrawable);
}

void OpenGLWidget::addDrawable(Drawable* drawable)
{
    m_drawables.append(drawable);
}

void OpenGLWidget::cleanUp()
{
    int instacedDrawablesCount = m_instancedDrawables.size();
    for(int i = 0; i < instacedDrawablesCount; i++)
    {
        InstancedDrawable* removedInstancedDrawable = m_instancedDrawables.back();
        m_instancedDrawables.pop_back();
        delete removedInstancedDrawable;
    }

    int drawablesCount = m_drawables.size();
    for(int i = 0; i < drawablesCount; i++)
    {
        Drawable* removedDrawable = m_drawables.back();
        m_drawables.pop_back();
        delete removedDrawable;
    }
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);

    m_camera->setPosition(QVector3D(0.f, 0.f, m_cameraDistance));
    m_camera->lookAt(QVector3D(0.f, 0.f, 0.f));

    // Prepare instanced rendering
    InstancedDrawable::mesh = MeshGenerator::generateUnitSphere(MeshType::TRIANGLE_MESH, 8, 8);

	int instancedMeshDataSize = InstancedDrawable::mesh->getVerticesCount() * 6;
    GLfloat* instancedMeshData = new GLfloat[instancedMeshDataSize];
    for(int i = 0; i < InstancedDrawable::mesh->getVertices().size(); i++)
    {
        QVector3D vertex = InstancedDrawable::mesh->getVertices().at(i);
        QVector3D normal = InstancedDrawable::mesh->getNormals().at(i);
        instancedMeshData[6*i] = vertex.x();
        instancedMeshData[6*i + 1] = vertex.y();
        instancedMeshData[6*i + 2] = vertex.z();
        instancedMeshData[6*i + 3] = normal.x();
        instancedMeshData[6*i + 4] = normal.y();
        instancedMeshData[6*i + 5] = normal.z();
    }

    m_instancedShaderProgram = new QOpenGLShaderProgram();
    m_instancedShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/instancedRendering.vert");
    m_instancedShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/simplePhong.frag");
    m_instancedShaderProgram->link();
    m_instancedShaderProgram->bind();

    m_instancedViewMatrixUniform = m_instancedShaderProgram->uniformLocation("viewMatrix");
    m_instancedProjectionMatrixUniform = m_instancedShaderProgram->uniformLocation("projectionMatrix");

    m_instancedVao.create();
    m_instancedVao.bind();

    m_instancedMeshBuffer.create();
    m_instancedMeshBuffer.bind();
    m_instancedMeshBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_instancedMeshBuffer.allocate(instancedMeshData, instancedMeshDataSize * sizeof(GLfloat));
    m_instancedShaderProgram->enableAttributeArray(0);
    m_instancedShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    glVertexAttribDivisor(0, 0);
    m_instancedShaderProgram->enableAttributeArray(1);
    m_instancedShaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    glVertexAttribDivisor(1, 0);
    m_instancedMeshBuffer.release();

    m_instancedTranslationBuffer.create();
    m_instancedTranslationBuffer.bind();
    m_instancedTranslationBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_instancedShaderProgram->enableAttributeArray(2);
    m_instancedShaderProgram->setAttributeBuffer(2, GL_FLOAT, 0, 3);
    glVertexAttribDivisor(2, 1);
    m_instancedTranslationBuffer.release();

    m_instancedScaleBuffer.create();
    m_instancedScaleBuffer.bind();
    m_instancedScaleBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_instancedShaderProgram->enableAttributeArray(3);
    m_instancedShaderProgram->setAttributeBuffer(3, GL_FLOAT, 0, 3);
    glVertexAttribDivisor(3, 1);
    m_instancedScaleBuffer.release();

    m_instancedColorBuffer.create();
    m_instancedColorBuffer.bind();
    m_instancedColorBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_instancedShaderProgram->enableAttributeArray(4);
    m_instancedShaderProgram->setAttributeBuffer(4, GL_FLOAT, 0, 4);
    glVertexAttribDivisor(4, 1);
    m_instancedColorBuffer.release();

    m_instancedVao.release();
    m_instancedShaderProgram->release();

	delete[] instancedMeshData;

    // Prepare normal rendering
    m_shaderProgram = new QOpenGLShaderProgram();
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/simplePhong.vert");
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/simplePhong.frag");
    m_shaderProgram->link();
    m_shaderProgram->bind();

    m_translationUniform = m_shaderProgram->uniformLocation("translation");
    m_scaleUniform = m_shaderProgram->uniformLocation("scale");
    m_colorUniform = m_shaderProgram->uniformLocation("color");
    m_viewMatrixUniform = m_shaderProgram->uniformLocation("viewMatrix");
    m_projectionMatrixUniform = m_shaderProgram->uniformLocation("projectionMatrix");

    m_vao.create();
    m_vao.bind();

    m_meshBuffer.create();
    m_meshBuffer.bind();
    m_meshBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_shaderProgram->enableAttributeArray(0);
    m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    m_shaderProgram->enableAttributeArray(1);
    m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    m_meshBuffer.release();

    m_vao.release();
    m_shaderProgram->release();
}

void OpenGLWidget::resizeGL(int width, int height)
{
    m_camera->setAspectRatio(width, height);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Execute instanced rendering
	int translationDataSize = 3 * m_instancedDrawables.size();
	int scaleDataSize = 3 * m_instancedDrawables.size();
	int colorDataSize = 4 * m_instancedDrawables.size();
    GLfloat* translationData = new GLfloat[translationDataSize];
    GLfloat* scaleData = new GLfloat[scaleDataSize];
    GLfloat* colorData = new GLfloat[colorDataSize];

    for(int i = 0; i < m_instancedDrawables.size(); i++)
    {
        InstancedDrawable* instancedDrawable = m_instancedDrawables[i];
        translationData[3*i] = instancedDrawable->translation.x();
        translationData[3*i + 1] = instancedDrawable->translation.y();
        translationData[3*i + 2] = instancedDrawable->translation.z();

        scaleData[3*i] = instancedDrawable->scale.x();
        scaleData[3*i + 1] = instancedDrawable->scale.y();
        scaleData[3*i + 2] = instancedDrawable->scale.z();

        colorData[4*i] = (float)instancedDrawable->color.red() / 255.f;
        colorData[4*i + 1] = (float)instancedDrawable->color.green() / 255.f;
        colorData[4*i + 2] = (float)instancedDrawable->color.blue() / 255.f;
        colorData[4*i + 3] = (float)instancedDrawable->color.alpha() / 255.f;
    }

    m_instancedShaderProgram->bind();
    m_instancedShaderProgram->setUniformValue(m_instancedViewMatrixUniform, m_camera->getViewMatrix());
    m_instancedShaderProgram->setUniformValue(m_instancedProjectionMatrixUniform, m_camera->getProjectionMatrix());

    m_instancedVao.bind();

    m_instancedTranslationBuffer.bind();
    m_instancedTranslationBuffer.allocate(translationData, translationDataSize * sizeof(GLfloat));
    m_instancedTranslationBuffer.release();

    m_instancedScaleBuffer.bind();
    m_instancedScaleBuffer.allocate(scaleData, scaleDataSize * sizeof(GLfloat));
    m_instancedScaleBuffer.release();

    m_instancedColorBuffer.bind();
    m_instancedColorBuffer.allocate(colorData, colorDataSize * sizeof(GLfloat));
    m_instancedColorBuffer.release();

    GLenum drawMode;
    if(InstancedDrawable::mesh->getMeshType() == MeshType::TRIANGLE_MESH)
        drawMode = GL_TRIANGLES;
    else
        drawMode = GL_LINES;
    glDrawArraysInstanced(drawMode, 0, InstancedDrawable::mesh->getVerticesCount(), m_instancedDrawables.size());

    m_instancedVao.release();

    m_instancedShaderProgram->release();

    // Execute normal rendering
    for(int i = 0; i < m_drawables.size(); i++)
    {
        Drawable* drawable = m_drawables[i];

        m_shaderProgram->bind();
        m_shaderProgram->setUniformValue(m_viewMatrixUniform, m_camera->getViewMatrix());
        m_shaderProgram->setUniformValue(m_projectionMatrixUniform, m_camera->getProjectionMatrix());
        m_shaderProgram->setUniformValue(m_translationUniform, drawable->translation);
        m_shaderProgram->setUniformValue(m_scaleUniform, drawable->scale);
        m_shaderProgram->setUniformValue(m_colorUniform, drawable->color);

        m_vao.bind();

		int meshDataSize = drawable->mesh->getVerticesCount() * 6;
        GLfloat* meshData = new GLfloat[meshDataSize];
        for(int i = 0; i < drawable->mesh->getVertices().size(); i++)
        {
            QVector3D vertex = drawable->mesh->getVertices().at(i);
            QVector3D normal = drawable->mesh->getNormals().at(i);
            meshData[6*i] = vertex.x();
            meshData[6*i + 1] = vertex.y();
            meshData[6*i + 2] = vertex.z();
            meshData[6*i + 3] = normal.x();
            meshData[6*i + 4] = normal.y();
            meshData[6*i + 5] = normal.z();
        }

        m_meshBuffer.bind();
        m_meshBuffer.allocate(meshData, meshDataSize * sizeof(GLfloat));
        m_meshBuffer.release();

        GLenum drawMode;
        if(drawable->mesh->getMeshType() == MeshType::TRIANGLE_MESH)
            drawMode = GL_TRIANGLES;
        else
            drawMode = GL_LINES;
        glDrawArrays(drawMode, 0, drawable->mesh->getVerticesCount());

        m_vao.release();
        m_shaderProgram->release();

		delete[] meshData;
    }
	delete[] translationData;
	delete[] scaleData;
	delete[] colorData;
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(m_isRightMouseDown)
    {
        QPoint relativeMousePosition = event->globalPos() - m_prevMousePosition;
        m_prevMousePosition = event->globalPos();

        m_horizontalCameraAngle -= relativeMousePosition.x() * m_cameraMoveSpeed;
        m_verticalCameraAngle -= relativeMousePosition.y() * m_cameraMoveSpeed;
        m_verticalCameraAngle = std::max(m_verticalCameraAngle, 15.f);
        m_verticalCameraAngle = std::min(m_verticalCameraAngle, 165.f);

        QVector3D distanceDirection = qSin(qDegreesToRadians(m_horizontalCameraAngle)) * QVector3D(1.f, 0.f, 0.f);
        distanceDirection += qCos(qDegreesToRadians(m_horizontalCameraAngle)) * QVector3D(0.f, 0.f, 1.f);
        distanceDirection = qSin(qDegreesToRadians(m_verticalCameraAngle)) * distanceDirection;
        distanceDirection += qCos(qDegreesToRadians(m_verticalCameraAngle)) * QVector3D(0.f, 1.f, 0.f);

        m_camera->setPosition(distanceDirection * m_cameraDistance);
        m_camera->lookAt(QVector3D(0.f, 0.f, 0.f));
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        m_isRightMouseDown = true;
        m_prevMousePosition = event->globalPos();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        m_isRightMouseDown = false;
    }
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
    m_cameraDistance -= event->delta() * m_cameraScrollSpeed;
    m_cameraDistance = std::max(m_cameraDistance, 0.01f);
    m_cameraDistance = std::min(m_cameraDistance, 1000.f);

    QVector3D distanceDirection = qSin(qDegreesToRadians(m_horizontalCameraAngle)) * QVector3D(1.f, 0.f, 0.f);
    distanceDirection += qCos(qDegreesToRadians(m_horizontalCameraAngle)) * QVector3D(0.f, 0.f, 1.f);
    distanceDirection = qSin(qDegreesToRadians(m_verticalCameraAngle)) * distanceDirection;
    distanceDirection += qCos(qDegreesToRadians(m_verticalCameraAngle)) * QVector3D(0.f, 1.f, 0.f);

    m_camera->setPosition(distanceDirection * m_cameraDistance);
    m_camera->lookAt(QVector3D(0.f, 0.f, 0.f));
}
