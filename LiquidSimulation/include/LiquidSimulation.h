#ifndef LIQUIDSIMULATION_H
#define LIQUIDSIMULATION_H

#include <QElapsedTimer>
#include <QTimer>
#include "Rendering/OpenGLWidget.h"
#include "SPHLiquidWorld.h"
#include "Scenarios/WaveBreakerScenario.h"
#include "Scenarios/WaterDropsScenario.h"
#include "Scenarios/WaterfallScenario.h"
#include "Scenarios/SphereWavesScenario.h"
#include "Scenarios/DamBreakScenario.h"

enum class SimulationMethod {
	SPH,
	PCISPH
};

class LiquidSimulation : public QObject
{
    Q_OBJECT

public:
    LiquidSimulation();

    void initializeGraphics();
    void initializeSimulation();

private slots:
    void update();

    void startStopSimulation();
    void stepSimulation();
    void resetSimulation();
	void changeParallelization(int index);
	void changeSimulationMethod(int index);
    void setTimeStep(int sliderValue);
    void setXGravity(int sliderValue);
    void setYGravity(int sliderValue);
    void setZGravity(int sliderValue);
    void setParticleRadius(int sliderValue);
    void setKernelRadiusFactor(int sliderValue);
    void setRestDensity(int sliderValue);
    void setViscosityCoefficient(int sliderValue);
    void setPressureStiffnessCoefficient(int sliderValue);
    void setNegativePressureFactor(int sliderValue);
    void setSurfaceTensionCoefficient(int sliderValue);
    void setSurfaceTensionThreshold(int sliderValue);
    void setRestitutionCoefficient(int sliderValue);
    void setFrictionCoefficient(int sliderValue);
    void restoreDefaultParameters();

    void changeScenario(int index);
    void updateSimulationInfo();

private:
    void buildControlWidget(QWidget* parent);

    QWidget* m_windowWidget;
    OpenGLWidget* m_graphicsWidget;
    QWidget* m_controlWidget;
    QStackedWidget* m_scenarioControlsStackedWidget;

    LiquidScenario* m_currentScenario;
    QVector<LiquidScenario*> m_scenarios;

    QElapsedTimer m_elapsedTimer;

	SimulationMethod m_currentSimulationMethod;
	SPHSolver* m_sphSolver;
	PCISPHSolver* m_pcisphSolver;
    SPHLiquidWorld* m_sphLiquidWorld;

    bool m_isSimulationStarted;
    float m_simulationTimeStep;
    float m_fps;
    float m_simulatedTime;


    QPushButton* m_startStopButton;
    QPushButton* m_stepButton;
    QPushButton* m_resetButton;

	QComboBox* m_parallelizationSelection;

	QComboBox* m_simulationMethodSelection;

    QLabel* m_timeStepLabel;
    QSlider* m_timeStepSlider;
    float m_timeStepSliderStep;
    int m_timeStepMinMaxDefault[3];

    QLabel* m_gravityLabel;
    QSlider* m_xGravitySlider;
    QSlider* m_yGravitySlider;
    QSlider* m_zGravitySlider;
    float m_xGravitySliderStep;
    float m_yGravitySliderStep;
    float m_zGravitySliderStep;
    int m_xGravityMinMaxDefault[3];
    int m_yGravityMinMaxDefault[3];
    int m_zGravityMinMaxDefault[3];

    QLabel* m_particleRadiusLabel;
    QSlider* m_particleRadiusSlider;
    float m_particleRadiusSliderStep;
    int m_particleRadiusMinMaxDefault[3];

    QLabel* m_kernelRadiusFactorLabel;
    QSlider* m_kernelRadiusFactorSlider;
    float m_kernelRadiusFactorSliderStep;
    int m_kernelRadiusFactorMinMaxDefault[3];

    QLabel* m_restDensityLabel;
    QSlider* m_restDensitySlider;
    float m_restDensitySliderStep;
    int m_restDensityMinMaxDefault[3];

    QLabel* m_viscosityCoefficientLabel;
    QSlider* m_viscosityCoefficientSlider;
    float m_viscosityCoefficientSliderStep;
    int m_viscosityCoefficientMinMaxDefault[3];

    QLabel* m_pressureStiffnessCoefficientLabel;
    QSlider* m_pressureStiffnessCoefficientSlider;
    float m_pressureStiffnessCoefficientSliderStep;
    int m_pressureStiffnessCoefficientMinMaxDefault[3];

    QLabel* m_negativePressureFactorLabel;
    QSlider* m_negativePressureFactorSlider;
    float m_negativePressureFactorSliderStep;
    int m_negativePressureFactorMinMaxDefault[3];

    QLabel* m_surfaceTensionCoefficientLabel;
    QSlider* m_surfaceTensionCoefficientSlider;
    float m_surfaceTensionCoefficientSliderStep;
    int m_surfaceTensionCoefficientMinMaxDefault[3];

    QLabel* m_surfaceTensionThresholdLabel;
    QSlider* m_surfaceTensionThresholdSlider;
    float m_surfaceTensionThresholdSliderStep;
    int m_surfaceTensionThresholdMinMaxDefault[3];

    QLabel* m_restitutionCoefficientLabel;
    QSlider* m_restitutionCoefficientSlider;
    float m_restitutionCoefficientSliderStep;
    int m_restitutionCoefficientMinMaxDefault[3];

    QLabel* m_frictionCoefficientLabel;
    QSlider* m_frictionCoefficientSlider;
    float m_frictionCoefficientSliderStep;
    int m_frictionCoefficientMinMaxDefault[3];

    QPushButton* m_restoreDefaultParametersButton;

    QComboBox* m_scenarioSelection;

    QLabel* m_framesPerSecondLabel;
    QLabel* m_simulatedTimeLabel;
    QLabel* m_amountParticlesLabel;
    QLabel* m_particleMassLabel;
    QLabel* m_kernelRadiusLabel;
};

#endif // LIQUIDSIMULATION_H
