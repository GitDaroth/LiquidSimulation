#include "Scenarios/LiquidScenario.h"

LiquidScenario::LiquidScenario(QString name, SPHLiquidWorld* sphLiquidWorld) :
    m_name(name),
    m_sphLiquidWorld(sphLiquidWorld)
{

}

void LiquidScenario::cleanUpScenario()
{
    m_sphLiquidWorld->cleanUp();
}

QWidget *LiquidScenario::getSpecificWidget()
{
    return m_specificScenarioWidget;
}

QString LiquidScenario::getName() const
{
    return m_name;
}

void LiquidScenario::setName(QString name)
{
    m_name = name;
}
