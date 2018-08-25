#pragma once

#include "LiquidScenario.h"

class DamBreakScenario : public LiquidScenario
{
	Q_OBJECT

public:
	DamBreakScenario(QString name, SPHLiquidWorld* sphFluidWorld);

	virtual void initScenario();
	virtual void updateScenario(float deltaTime);

protected:
	virtual void buildScenarioWidget();

private:
	float m_spawnRate;
	float m_dropSize;
};