#ifndef FLUIDSCENARIO_H
#define FLUIDSCENARIO_H

#include <QStackedWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

#include "SPHLiquidWorld.h"
#include <Particles/SPHParticleEmitter.h>

using namespace LiPhEn;

class LiquidScenario : public QObject
{
    Q_OBJECT

public:
    LiquidScenario(QString name, SPHLiquidWorld* sphLiquidWorld);

    virtual void initScenario() = 0;
    virtual void updateScenario(float deltaTime) = 0;
    void cleanUpScenario();

    QWidget* getSpecificWidget();
    QString getName() const;

    void setName(QString name);

protected:
    virtual void buildScenarioWidget() = 0;

    QString m_name;
    QWidget* m_specificScenarioWidget;
    SPHLiquidWorld* m_sphLiquidWorld;
};

#endif // FLUIDSCENARIO_H
