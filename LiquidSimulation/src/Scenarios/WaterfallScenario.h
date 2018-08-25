#pragma once

#include "LiquidScenario.h"

class WaterfallScenario : public LiquidScenario
{
	Q_OBJECT

public:
	WaterfallScenario(QString name, SPHLiquidWorld* sphFluidWorld);

	virtual void initScenario();
	virtual void updateScenario(float deltaTime);

protected:
	virtual void buildScenarioWidget();

	private slots:
	void setInflowSpeed(int sliderValue);
	void setInflowSize(int sliderValue);
	void restoreDefaultParameters();

private:
	float m_simulatedTime;
	float m_inflowSpeed;
	float m_inflowSize;

	QLabel* m_inflowSpeedLabel;
	QSlider* m_inflowSpeedSlider;
	float m_inflowSpeedSliderStep;
	int m_inflowSpeedMinMaxDefault[3];

	QLabel* m_inflowSizeLabel;
	QSlider* m_inflowSizeSlider;
	float m_inflowSizeSliderStep;
	int m_inflowSizeMinMaxDefault[3];

	QPushButton* m_restoreDefaultParametersButton;
};