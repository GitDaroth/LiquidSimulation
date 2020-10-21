#ifndef WAVEGENERATIONSCENARIO_H
#define WAVEGENERATIONSCENARIO_H

#include "Scenarios/LiquidScenario.h"

class WaveBreakerScenario : public LiquidScenario
{
    Q_OBJECT

public:
    WaveBreakerScenario(QString name, SPHLiquidWorld* sphFluidWorld);

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
    StaticCollisionBox* m_boundaryBox;

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

#endif // WAVEGENERATIONSCENARIO_H
