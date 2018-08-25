#pragma once

#include "LiquidScenario.h"

class SphereWavesScenario : public LiquidScenario
{
	Q_OBJECT

public:
	SphereWavesScenario(QString name, SPHLiquidWorld* sphFluidWorld);

	virtual void initScenario();
	virtual void updateScenario(float deltaTime);

	private slots:
	void setFrequency(int sliderValue);
	void setAmplitude(int sliderValue);
	void restoreDefaultParameters();

protected:
	virtual void buildScenarioWidget();

private:
	float m_frequency;
	float m_amplitude;
	float m_simulatedTime;
	StaticCollisionSphere* m_boundarySphere;

	QLabel* m_frequencyLabel;
	QSlider* m_frequencySlider;
	float m_frequencySliderStep;
	int m_frequencyMinMaxDefault[3];

	QLabel* m_amplitudeLabel;
	QSlider* m_amplitudeSlider;
	float m_amplitudeSliderStep;
	int m_amplitudeMinMaxDefault[3];

	QPushButton* m_restoreDefaultParametersButton;
};