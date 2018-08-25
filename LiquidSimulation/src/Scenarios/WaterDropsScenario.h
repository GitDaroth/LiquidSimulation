#ifndef WATERDROPSSCENARIO_H
#define WATERDROPSSCENARIO_H

#include "LiquidScenario.h"

class WaterDropsScenario : public LiquidScenario
{
    Q_OBJECT

public:
    WaterDropsScenario(QString name, SPHLiquidWorld* sphFluidWorld);

    virtual void initScenario();
    virtual void updateScenario(float deltaTime);

protected:
    virtual void buildScenarioWidget();

private slots:
	void setSpawnRate(int sliderValue);
	void setDropSize(int sliderValue);
	void restoreDefaultParameters();

private:
	float m_simulatedTime;
    float m_spawnRate;
    float m_dropSize;

	QLabel* m_spawnRateLabel;
	QSlider* m_spawnRateSlider;
	float m_spawnRateSliderStep;
	int m_spawnRateMinMaxDefault[3];

	QLabel* m_dropSizeLabel;
	QSlider* m_dropSizeSlider;
	float m_dropSizeSliderStep;
	int m_dropSizeMinMaxDefault[3];

	QPushButton* m_restoreDefaultParametersButton;
};

#endif // WATERDROPSSCENARIO_H
