#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QVector3D>
#include <QtMath>

enum class MeshType
{
    LINE_MESH,
    TRIANGLE_MESH
};

class Mesh
{
public:
    Mesh();

    void setVertices(QVector<QVector3D> vertices);
    void setNormals(QVector<QVector3D> normals);
    void setMeshType(MeshType type);

    int getVerticesCount() const;
    QVector<QVector3D> getVertices() const;
    QVector<QVector3D> getNormals() const;
    MeshType getMeshType() const;

private:
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    MeshType m_meshType;
};

class MeshGenerator
{
public:
    static Mesh* generateUnitSphere(MeshType type, int latitide, int longitude);
    static Mesh* generateUnitBox(MeshType type);
};

#endif // MESH_H
