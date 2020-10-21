#ifndef STATICCOLLISIONOBJECTDRAWABLE_H
#define STATICCOLLISIONOBJECTDRAWABLE_H

#include <Collision/StaticCollisionBox.h>
#include <Collision/StaticCollisionSphere.h>
#include "Rendering/OpenGLWidget.h"

using namespace LiPhEn;

class StaticCollisionObjectDrawable
{
public:
    StaticCollisionObjectDrawable(StaticCollisionObject* collisionObject, Drawable* drawable);

    void update();

    StaticCollisionObject* getCollisionObject();
    Drawable* getDrawable();

    void setCollisionObject(StaticCollisionObject* collisionObject);
    void setDrawable(Drawable* drawable);

private:
    StaticCollisionObject* m_collisionObject;
    Drawable* m_drawable;
};

#endif // STATICCOLLISIONOBJECTDRAWABLE_H
