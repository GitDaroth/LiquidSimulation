#include "StaticCollisionObjectDrawable.h"

StaticCollisionObjectDrawable::StaticCollisionObjectDrawable(StaticCollisionObject *collisionObject, Drawable *drawable) :
    m_collisionObject(collisionObject),
    m_drawable(drawable)
{
    if(dynamic_cast<StaticCollisionBox*>(m_collisionObject))
    {
        if(m_collisionObject->getType() == StaticCollisionObjectType::OBSTACLE)
            m_drawable->mesh = MeshGenerator::generateUnitBox(MeshType::TRIANGLE_MESH);
        else
            m_drawable->mesh = MeshGenerator::generateUnitBox(MeshType::LINE_MESH);
    }
    else if(dynamic_cast<StaticCollisionSphere*>(m_collisionObject))
    {
        if(m_collisionObject->getType() == StaticCollisionObjectType::OBSTACLE)
            m_drawable->mesh = MeshGenerator::generateUnitSphere(MeshType::TRIANGLE_MESH, 30, 30);
        else
            m_drawable->mesh = MeshGenerator::generateUnitSphere(MeshType::LINE_MESH, 30, 30);
    }
}

void StaticCollisionObjectDrawable::update()
{
    m_drawable->translation = QVector3D(m_collisionObject->getPosition().getX(),
                                        m_collisionObject->getPosition().getY(),
                                        m_collisionObject->getPosition().getZ());

    if(m_collisionObject->getType() == StaticCollisionObjectType::OBSTACLE)
        m_drawable->color = QColor(240, 0, 0);
    else
        m_drawable->color = QColor(0, 240, 0);

    if(dynamic_cast<StaticCollisionBox*>(m_collisionObject))
    {
        Vector3D halfDimensions = dynamic_cast<StaticCollisionBox*>(m_collisionObject)->getHalfDimensions();
        m_drawable->scale = QVector3D(halfDimensions.getX(), halfDimensions.getY(), halfDimensions.getZ());
    }
    else if(dynamic_cast<StaticCollisionSphere*>(m_collisionObject))
    {
        float radius = dynamic_cast<StaticCollisionSphere*>(m_collisionObject)->getRadius();
        m_drawable->scale = QVector3D(radius, radius, radius);
    }
}

StaticCollisionObject *StaticCollisionObjectDrawable::getCollisionObject()
{
    return m_collisionObject;
}

Drawable *StaticCollisionObjectDrawable::getDrawable()
{
    return m_drawable;
}

void StaticCollisionObjectDrawable::setCollisionObject(StaticCollisionObject *collisionObject)
{
    m_collisionObject = collisionObject;
}

void StaticCollisionObjectDrawable::setDrawable(Drawable *drawable)
{
    m_drawable = drawable;
}
