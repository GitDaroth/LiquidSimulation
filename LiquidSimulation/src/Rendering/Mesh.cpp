#include "Mesh.h"

Mesh::Mesh()
{

}

void Mesh::setVertices(QVector<QVector3D> vertices)
{
    m_vertices = vertices;
}

void Mesh::setNormals(QVector<QVector3D> normals)
{
    m_normals = normals;
}

void Mesh::setMeshType(MeshType type)
{
    m_meshType = type;
}

int Mesh::getVerticesCount() const
{
    return m_vertices.size();
}

QVector<QVector3D> Mesh::getVertices() const
{
    return m_vertices;
}

QVector<QVector3D> Mesh::getNormals() const
{
    return m_normals;
}

MeshType Mesh::getMeshType() const
{
    return m_meshType;
}


Mesh* MeshGenerator::generateUnitSphere(MeshType type, int latitide, int longitude)
{
    Mesh* unitSphereMesh = new Mesh();
    unitSphereMesh->setMeshType(type);
    QVector<QVector3D> vertices;

    float latitude_increment = 360.0f / latitide;
    float longitude_increment = 180.0f / longitude;

    for (float t = 0.f; t < 180.0f; t += longitude_increment) {
        for (float u = 0.f; u < 360.0f; u += latitude_increment) {
            if(t < longitude_increment)
            {
                if(type == MeshType::TRIANGLE_MESH)
                {
                    vertices.append(QVector3D(0.f, 1.f, 0.f));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                }
                else
                {
                    vertices.append(QVector3D(0.f, 1.f, 0.f));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u)))));

                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u + latitude_increment)))));

                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                    vertices.append(QVector3D(0.f, 1.f, 0.f));
                }

            }
            else if(t >= 180.f - longitude_increment)
            {
                if(type == MeshType::TRIANGLE_MESH)
                {
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D(0.f, -1.f, 0.f));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                }
                else
                {
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D(0.f, -1.f, 0.f));

                    vertices.append(QVector3D(0.f, -1.f, 0.f));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                }
            }
            else
            {
                if(type == MeshType::TRIANGLE_MESH)
                {
                    // Top left Triangle of Quad
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u + latitude_increment)))));

                    // Bottom right Triangle of Quad
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                }
                else
                {
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u)))));

                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u + latitude_increment)))));

                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t + longitude_increment)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t + longitude_increment))),
                                              (float) (qSin(qDegreesToRadians(t + longitude_increment)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                    vertices.append(QVector3D((float) (qSin(qDegreesToRadians(t)) * qSin(qDegreesToRadians(u + latitude_increment))),
                                              (float) (qCos(qDegreesToRadians(t))),
                                              (float) (qSin(qDegreesToRadians(t)) * qCos(qDegreesToRadians(u + latitude_increment)))));
                }
            }
        }
    }

    unitSphereMesh->setVertices(vertices);
    unitSphereMesh->setNormals(vertices);
    return unitSphereMesh;
}

Mesh* MeshGenerator::generateUnitBox(MeshType type)
{
    Mesh* unitBoxMesh = new Mesh();
    unitBoxMesh->setMeshType(type);
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;

    if(type == MeshType::TRIANGLE_MESH)
    {
        // Front Quad
        vertices.append(QVector3D(-1.f, 1.f, 1.f));
        vertices.append(QVector3D(-1.f, -1.f, 1.f));
        vertices.append(QVector3D(1.f, 1.f, 1.f));

        vertices.append(QVector3D(1.f, 1.f, 1.f));
        vertices.append(QVector3D(-1.f, -1.f, 1.f));
        vertices.append(QVector3D(1.f, -1.f, 1.f));

        normals.append(QVector3D(0.f, 0.f, 1.f));
        normals.append(QVector3D(0.f, 0.f, 1.f));
        normals.append(QVector3D(0.f, 0.f, 1.f));
        normals.append(QVector3D(0.f, 0.f, 1.f));
        normals.append(QVector3D(0.f, 0.f, 1.f));
        normals.append(QVector3D(0.f, 0.f, 1.f));

        // Back Quad
        vertices.append(QVector3D(1.f, 1.f, -1.f));
        vertices.append(QVector3D(1.f, -1.f, -1.f));
        vertices.append(QVector3D(-1.f, 1.f, -1.f));

        vertices.append(QVector3D(-1.f, 1.f, -1.f));
        vertices.append(QVector3D(1.f, -1.f, -1.f));
        vertices.append(QVector3D(-1.f, -1.f, -1.f));

        normals.append(QVector3D(0.f, 0.f, -1.f));
        normals.append(QVector3D(0.f, 0.f, -1.f));
        normals.append(QVector3D(0.f, 0.f, -1.f));
        normals.append(QVector3D(0.f, 0.f, -1.f));
        normals.append(QVector3D(0.f, 0.f, -1.f));
        normals.append(QVector3D(0.f, 0.f, -1.f));

        // Top Quad
        vertices.append(QVector3D(-1.f, 1.f, -1.f));
        vertices.append(QVector3D(-1.f, 1.f, 1.f));
        vertices.append(QVector3D(1.f, 1.f, -1.f));

        vertices.append(QVector3D(1.f, 1.f, -1.f));
        vertices.append(QVector3D(-1.f, 1.f, 1.f));
        vertices.append(QVector3D(1.f, 1.f, 1.f));

        normals.append(QVector3D(0.f, 1.f, 0.f));
        normals.append(QVector3D(0.f, 1.f, 0.f));
        normals.append(QVector3D(0.f, 1.f, 0.f));
        normals.append(QVector3D(0.f, 1.f, 0.f));
        normals.append(QVector3D(0.f, 1.f, 0.f));
        normals.append(QVector3D(0.f, 1.f, 0.f));

        // Bottom Quad
        vertices.append(QVector3D(1.f, -1.f, -1.f));
        vertices.append(QVector3D(1.f, -1.f, 1.f));
        vertices.append(QVector3D(-1.f, -1.f, -1.f));

        vertices.append(QVector3D(-1.f, -1.f, -1.f));
        vertices.append(QVector3D(1.f, -1.f, 1.f));
        vertices.append(QVector3D(-1.f, -1.f, 1.f));

        normals.append(QVector3D(0.f, -1.f, 0.f));
        normals.append(QVector3D(0.f, -1.f, 0.f));
        normals.append(QVector3D(0.f, -1.f, 0.f));
        normals.append(QVector3D(0.f, -1.f, 0.f));
        normals.append(QVector3D(0.f, -1.f, 0.f));
        normals.append(QVector3D(0.f, -1.f, 0.f));

        // Right Quad
        vertices.append(QVector3D(1.f, 1.f, 1.f));
        vertices.append(QVector3D(1.f, -1.f, 1.f));
        vertices.append(QVector3D(1.f, 1.f, -1.f));

        vertices.append(QVector3D(1.f, 1.f, -1.f));
        vertices.append(QVector3D(1.f, -1.f, 1.f));
        vertices.append(QVector3D(1.f, -1.f, -1.f));

        normals.append(QVector3D(1.f, 0.f, 0.f));
        normals.append(QVector3D(1.f, 0.f, 0.f));
        normals.append(QVector3D(1.f, 0.f, 0.f));
        normals.append(QVector3D(1.f, 0.f, 0.f));
        normals.append(QVector3D(1.f, 0.f, 0.f));
        normals.append(QVector3D(1.f, 0.f, 0.f));

        // Left Quad
        vertices.append(QVector3D(-1.f, 1.f, -1.f));
        vertices.append(QVector3D(-1.f, -1.f, -1.f));
        vertices.append(QVector3D(-1.f, 1.f, 1.f));

        vertices.append(QVector3D(-1.f, 1.f, 1.f));
        vertices.append(QVector3D(-1.f, -1.f, -1.f));
        vertices.append(QVector3D(-1.f, -1.f, 1.f));

        normals.append(QVector3D(-1.f, 0.f, 0.f));
        normals.append(QVector3D(-1.f, 0.f, 0.f));
        normals.append(QVector3D(-1.f, 0.f, 0.f));
        normals.append(QVector3D(-1.f, 0.f, 0.f));
        normals.append(QVector3D(-1.f, 0.f, 0.f));
        normals.append(QVector3D(-1.f, 0.f, 0.f));

        unitBoxMesh->setNormals(normals);
    }
    else
    {
        // Front Quad
        vertices.append(QVector3D(-1.f, 1.f, 1.f));
        vertices.append(QVector3D(-1.f, -1.f, 1.f));

        vertices.append(QVector3D(-1.f, -1.f, 1.f));
        vertices.append(QVector3D(1.f, -1.f, 1.f));

        vertices.append(QVector3D(1.f, -1.f, 1.f));
        vertices.append(QVector3D(1.f, 1.f, 1.f));

        vertices.append(QVector3D(1.f, 1.f, 1.f));
        vertices.append(QVector3D(-1.f, 1.f, 1.f));

        // Back Quad
        vertices.append(QVector3D(1.f, 1.f, -1.f));
        vertices.append(QVector3D(1.f, -1.f, -1.f));

        vertices.append(QVector3D(1.f, -1.f, -1.f));
        vertices.append(QVector3D(-1.f, -1.f, -1.f));

        vertices.append(QVector3D(-1.f, -1.f, -1.f));
        vertices.append(QVector3D(-1.f, 1.f, -1.f));

        vertices.append(QVector3D(-1.f, 1.f, -1.f));
        vertices.append(QVector3D(1.f, 1.f, -1.f));

        // Connections Front -> Back
        vertices.append(QVector3D(-1.f, 1.f, 1.f));
        vertices.append(QVector3D(-1.f, 1.f, -1.f));

        vertices.append(QVector3D(-1.f, -1.f, 1.f));
        vertices.append(QVector3D(-1.f, -1.f, -1.f));

        vertices.append(QVector3D(1.f, -1.f, 1.f));
        vertices.append(QVector3D(1.f, -1.f, -1.f));

        vertices.append(QVector3D(1.f, 1.f, 1.f));
        vertices.append(QVector3D(1.f, 1.f, -1.f));

        unitBoxMesh->setNormals(vertices);
    }

    unitBoxMesh->setVertices(vertices);
    return unitBoxMesh;
}
