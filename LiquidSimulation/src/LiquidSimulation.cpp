#include "LiquidSimulation.h"
#include <QTime>

LiquidSimulation::LiquidSimulation()
{
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

    m_timeStepMinMaxDefault[0] = 1;
    m_timeStepMinMaxDefault[1] = 200;
    m_timeStepMinMaxDefault[2] = 83;
    m_timeStepSliderStep = 0.0001f;

    m_xGravityMinMaxDefault[0] = -200;
    m_xGravityMinMaxDefault[1] = 200;
    m_xGravityMinMaxDefault[2] = 0;
    m_xGravitySliderStep = 0.1f;

    m_yGravityMinMaxDefault[0] = -200;
    m_yGravityMinMaxDefault[1] = 200;
    m_yGravityMinMaxDefault[2] = -98;
    m_yGravitySliderStep = 0.1f;

    m_zGravityMinMaxDefault[0] = -200;
    m_zGravityMinMaxDefault[1] = 200;
    m_zGravityMinMaxDefault[2] = 0;
    m_zGravitySliderStep = 0.1f;

    m_particleRadiusMinMaxDefault[0] = 1;
    m_particleRadiusMinMaxDefault[1] = 100;
    m_particleRadiusMinMaxDefault[2] = 17;
    m_particleRadiusSliderStep = 0.001f;

    m_kernelRadiusFactorMinMaxDefault[0] = 100;
    m_kernelRadiusFactorMinMaxDefault[1] = 400;
    m_kernelRadiusFactorMinMaxDefault[2] = 269;
    m_kernelRadiusFactorSliderStep = 0.01f;

    m_restDensityMinMaxDefault[0] = 100;
    m_restDensityMinMaxDefault[1] = 2000;
    m_restDensityMinMaxDefault[2] = 998;
    m_restDensitySliderStep = 1.f;

    m_viscosityCoefficientMinMaxDefault[0] = 0;
    m_viscosityCoefficientMinMaxDefault[1] = 1000;
    m_viscosityCoefficientMinMaxDefault[2] = 35;
    m_viscosityCoefficientSliderStep = 0.1f;

    m_pressureStiffnessCoefficientMinMaxDefault[0] = 1;
    m_pressureStiffnessCoefficientMinMaxDefault[1] = 400;
    m_pressureStiffnessCoefficientMinMaxDefault[2] = 30;
    m_pressureStiffnessCoefficientSliderStep = 0.1f;

    m_negativePressureFactorMinMaxDefault[0] = 0;
    m_negativePressureFactorMinMaxDefault[1] = 100;
    m_negativePressureFactorMinMaxDefault[2] = 0;
    m_negativePressureFactorSliderStep = 0.01f;

    m_surfaceTensionCoefficientMinMaxDefault[0] = 0;
    m_surfaceTensionCoefficientMinMaxDefault[1] = 200;
    m_surfaceTensionCoefficientMinMaxDefault[2] = 73;
    m_surfaceTensionCoefficientSliderStep = 0.001f;

    m_surfaceTensionThresholdMinMaxDefault[0] = 10;
    m_surfaceTensionThresholdMinMaxDefault[1] = 200;
    m_surfaceTensionThresholdMinMaxDefault[2] = 71;
    m_surfaceTensionThresholdSliderStep = 0.1f;

    m_restitutionCoefficientMinMaxDefault[0] = 0;
    m_restitutionCoefficientMinMaxDefault[1] = 100;
    m_restitutionCoefficientMinMaxDefault[2] = 50;
    m_restitutionCoefficientSliderStep = 0.01f;

    m_frictionCoefficientMinMaxDefault[0] = 0;
    m_frictionCoefficientMinMaxDefault[1] = 100;
    m_frictionCoefficientMinMaxDefault[2] = 100;
    m_frictionCoefficientSliderStep = 0.01f;

    initializeGraphics();
    initializeSimulation();

    QTimer* updateTimer = new QTimer();
    connect(updateTimer, &QTimer::timeout, this, &LiquidSimulation::update);
    updateTimer->start(0);

    m_elapsedTimer.start();

    m_windowWidget->show();
    m_windowWidget->resize(1200, 800);
}

void LiquidSimulation::initializeGraphics()
{
    m_graphicsWidget = new OpenGLWidget();
    m_graphicsWidget->setMinimumSize(QSize(200, 100));

    m_controlWidget = new QWidget();
    m_controlWidget->setMinimumWidth(400);

    m_windowWidget = new QWidget;
    QHBoxLayout* hLayout = new QHBoxLayout(m_windowWidget);
    hLayout->addWidget(m_graphicsWidget, 1);
    hLayout->addWidget(m_controlWidget);

    m_windowWidget->setWindowTitle(QStringLiteral("Fluid Simulation"));
}

void LiquidSimulation::initializeSimulation()
{
    m_isSimulationStarted = false;
    m_fps = 0.f;
    m_simulatedTime = 0.f;

	m_currentSimulationMethod = SimulationMethod::SPH;
	m_sphSolver = new SPHSolver();
	m_pcisphSolver = NULL;

    m_sphLiquidWorld = new SPHLiquidWorld(m_sphSolver, m_graphicsWidget);

    buildControlWidget(m_controlWidget);

    m_scenarios.append(new WaterDropsScenario("Water Drops", m_sphLiquidWorld));
	m_scenarios.append(new WaterfallScenario("Waterfall", m_sphLiquidWorld));
	m_scenarios.append(new SphereWavesScenario("Sphere Waves", m_sphLiquidWorld));
	m_scenarios.append(new DamBreakScenario("Dam Break", m_sphLiquidWorld));
    m_scenarios.append(new WaveBreakerScenario("Wave Breaker", m_sphLiquidWorld));

    for(LiquidScenario* scenario : m_scenarios)
    {
        m_scenarioControlsStackedWidget->addWidget(scenario->getSpecificWidget());
    }

    m_currentScenario = m_scenarios[0];
    for(LiquidScenario* scenario : m_scenarios)
    {
        m_scenarioSelection->addItem(scenario->getName());
    }

	m_parallelizationSelection->setCurrentIndex(1);

    restoreDefaultParameters();
}

void LiquidSimulation::update()
{
    m_fps = 1000.f / m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();
    m_windowWidget->setWindowTitle("Fluid Simulation (" + QString::number(m_fps) + ")");

    // Simulate
    if(m_isSimulationStarted)
    {
		stepSimulation();
    }
    updateSimulationInfo();

    // Render
    m_graphicsWidget->update();
}

void LiquidSimulation::startStopSimulation()
{
    if(m_isSimulationStarted)
    {
        m_startStopButton->setText("Resume Simulation");
        m_isSimulationStarted = false;
    }
    else
    {
        m_startStopButton->setText("Stop Simulation");
        m_isSimulationStarted = true;
    }
}

void LiquidSimulation::stepSimulation()
{
    m_currentScenario->updateScenario(m_simulationTimeStep);
	if(m_sphLiquidWorld->getSPHSolver()->getParticleCount() >= 512)
		m_sphLiquidWorld->update(m_simulationTimeStep);
    m_simulatedTime += m_simulationTimeStep;
}

void LiquidSimulation::resetSimulation()
{
    changeScenario(m_scenarioSelection->currentIndex());
    m_simulatedTime = 0.f;
}

void LiquidSimulation::changeSimulationMethod(int index)
{
	ParallelizationType oldParallelType = m_sphLiquidWorld->getSPHSolver()->getParallelizationType();
	m_currentScenario->cleanUpScenario();

	if (m_sphSolver)
	{
		delete m_sphSolver;
		m_sphSolver = NULL;
	}
	if (m_pcisphSolver)
	{
		delete m_pcisphSolver;
		m_pcisphSolver = NULL;
	}

	switch (index)
	{
	default:
	case 0:
		m_currentSimulationMethod = SimulationMethod::SPH;
		m_sphSolver = new SPHSolver();
		m_sphLiquidWorld->setSPHSolver(m_sphSolver);
		break;

	case 1:
		m_currentSimulationMethod = SimulationMethod::PCISPH;
		m_pcisphSolver = new PCISPHSolver();
		m_sphLiquidWorld->setSPHSolver(m_pcisphSolver);
		break;
	}

	// Set parameters of solver
	m_sphLiquidWorld->getSPHSolver()->setParallelizationType(oldParallelType);

	float xGravity = m_xGravitySliderStep * m_xGravitySlider->value();
	float yGravity = m_yGravitySliderStep * m_yGravitySlider->value();
	float zGravity = m_zGravitySliderStep * m_zGravitySlider->value();
	m_sphLiquidWorld->getSPHSolver()->setGravity(Vector3D(xGravity, yGravity, zGravity));

	m_sphLiquidWorld->getSPHSolver()->setParticleRadius(m_particleRadiusSliderStep * m_particleRadiusSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setKernelRadiusFactor(m_kernelRadiusFactorSliderStep * m_kernelRadiusFactorSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setRestDensity(m_restDensitySliderStep * m_restDensitySlider->value());
	m_sphLiquidWorld->getSPHSolver()->setViscosityCoefficient(m_viscosityCoefficientSliderStep * m_viscosityCoefficientSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setPressureStiffnessCoefficient(m_pressureStiffnessCoefficientSliderStep * m_pressureStiffnessCoefficientSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setNegativePressureFactor(m_negativePressureFactorSliderStep * m_negativePressureFactorSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setSurfaceTensionCoefficient(m_surfaceTensionCoefficientSliderStep * m_surfaceTensionCoefficientSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setSurfaceTensionThreshold(m_surfaceTensionThresholdSliderStep * m_surfaceTensionThresholdSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setRestitutionCoefficient(m_restitutionCoefficientSliderStep * m_restitutionCoefficientSlider->value());
	m_sphLiquidWorld->getSPHSolver()->setFrictionCoefficient(m_frictionCoefficientSliderStep * m_frictionCoefficientSlider->value());

	// Reset Scenrio
	m_currentScenario->initScenario();
	m_startStopButton->setText("Start Simulation");
	m_isSimulationStarted = false;
	m_simulatedTime = 0.f;
}

void LiquidSimulation::changeParallelization(int index)
{
    if (m_sphLiquidWorld->getSPHSolver()->hasGPU() && m_sphLiquidWorld->getSPHSolver()->hasCPU())
    {
        if(index == 0)
            m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::GPU);
        else if (index == 1)
            m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::CPU);
        else
            m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::NONE);
    }
    else if (m_sphLiquidWorld->getSPHSolver()->hasGPU())
    {
        if (index == 0)
            m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::GPU);
        else
            m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::NONE);
    }
    else if (m_sphLiquidWorld->getSPHSolver()->hasCPU())
    {
        if (index == 0)
            m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::CPU);
        else
            m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::NONE);
    }
    else
        m_sphLiquidWorld->getSPHSolver()->setParallelizationType(ParallelizationType::NONE);
}

void LiquidSimulation::setTimeStep(int sliderValue)
{
    m_simulationTimeStep = m_timeStepSliderStep * sliderValue;
    m_timeStepLabel->setText(QString().setNum(m_simulationTimeStep, 'g', 6) + " s");
}

void LiquidSimulation::setXGravity(int sliderValue)
{
    float xGravity = m_xGravitySliderStep * sliderValue;
    float yGravity = m_yGravitySliderStep * m_yGravitySlider->value();
    float zGravity = m_zGravitySliderStep * m_zGravitySlider->value();
    m_gravityLabel->setText("(" + QString().setNum(xGravity, 'g', 6) + ", " +
                            QString().setNum(yGravity, 'g', 6) + ", " +
                            QString().setNum(zGravity, 'g', 6) + ") m/s²");

    m_sphLiquidWorld->getSPHSolver()->setGravity(Vector3D(xGravity, yGravity, zGravity));
}

void LiquidSimulation::setYGravity(int sliderValue)
{
    float xGravity = m_xGravitySliderStep * m_xGravitySlider->value();
    float yGravity = m_yGravitySliderStep * sliderValue;
    float zGravity = m_zGravitySliderStep * m_zGravitySlider->value();
    m_gravityLabel->setText("(" + QString().setNum(xGravity, 'g', 6) + ", " +
                            QString().setNum(yGravity, 'g', 6) + ", " +
                            QString().setNum(zGravity, 'g', 6) + ") m/s²");

    m_sphLiquidWorld->getSPHSolver()->setGravity(Vector3D(xGravity, yGravity, zGravity));
}

void LiquidSimulation::setZGravity(int sliderValue)
{
    float xGravity = m_xGravitySliderStep * m_xGravitySlider->value();
    float yGravity = m_yGravitySliderStep * m_yGravitySlider->value();
    float zGravity = m_zGravitySliderStep * sliderValue;
    m_gravityLabel->setText("(" + QString().setNum(xGravity, 'g', 6) + ", " +
                            QString().setNum(yGravity, 'g', 6) + ", " +
                            QString().setNum(zGravity, 'g', 6) + ") m/s²");

    m_sphLiquidWorld->getSPHSolver()->setGravity(Vector3D(xGravity, yGravity, zGravity));
}

void LiquidSimulation::setParticleRadius(int sliderValue)
{
    float particleRadius = m_particleRadiusSliderStep * sliderValue;
    m_particleRadiusLabel->setText(QString().setNum(particleRadius, 'g', 6) + " m");
    m_sphLiquidWorld->getSPHSolver()->setParticleRadius(particleRadius);
}

void LiquidSimulation::setKernelRadiusFactor(int sliderValue)
{
    float kernelRadiusFactor = m_kernelRadiusFactorSliderStep * sliderValue;
    m_kernelRadiusFactorLabel->setText(QString().setNum(kernelRadiusFactor, 'g', 6));
    m_sphLiquidWorld->getSPHSolver()->setKernelRadiusFactor(kernelRadiusFactor);
}

void LiquidSimulation::setRestDensity(int sliderValue)
{
    float restDensity = m_restDensitySliderStep * sliderValue;
    m_restDensityLabel->setText(QString().setNum(restDensity, 'g', 6) + " kg/m³");
    m_sphLiquidWorld->getSPHSolver()->setRestDensity(restDensity);
}

void LiquidSimulation::setViscosityCoefficient(int sliderValue)
{
    float viscosityCoefficient = m_viscosityCoefficientSliderStep * sliderValue;
    m_viscosityCoefficientLabel->setText(QString().setNum(viscosityCoefficient, 'g', 6) + " Pa*s");
    m_sphLiquidWorld->getSPHSolver()->setViscosityCoefficient(viscosityCoefficient);
}

void LiquidSimulation::setPressureStiffnessCoefficient(int sliderValue)
{
    float pressureStiffnessCoefficient = m_pressureStiffnessCoefficientSliderStep * sliderValue;
    m_pressureStiffnessCoefficientLabel->setText(QString().setNum(pressureStiffnessCoefficient, 'g', 6) + " J");
    m_sphLiquidWorld->getSPHSolver()->setPressureStiffnessCoefficient(pressureStiffnessCoefficient);
}

void LiquidSimulation::setNegativePressureFactor(int sliderValue)
{
    float negativePressureFactor = m_negativePressureFactorSliderStep * sliderValue;
    m_negativePressureFactorLabel->setText(QString().setNum(negativePressureFactor, 'g', 6));
    m_sphLiquidWorld->getSPHSolver()->setNegativePressureFactor(negativePressureFactor);
}

void LiquidSimulation::setSurfaceTensionCoefficient(int sliderValue)
{
    float surfaceTensionCoefficient = m_surfaceTensionCoefficientSliderStep * sliderValue;
    m_surfaceTensionCoefficientLabel->setText(QString().setNum(surfaceTensionCoefficient, 'g', 6) + " N/m");
    m_sphLiquidWorld->getSPHSolver()->setSurfaceTensionCoefficient(surfaceTensionCoefficient);
}

void LiquidSimulation::setSurfaceTensionThreshold(int sliderValue)
{
    float surfaceTensionThreshold = m_surfaceTensionThresholdSliderStep * sliderValue;
    m_surfaceTensionThresholdLabel->setText(QString().setNum(surfaceTensionThreshold, 'g', 6));
    m_sphLiquidWorld->getSPHSolver()->setSurfaceTensionThreshold(surfaceTensionThreshold);
}

void LiquidSimulation::setRestitutionCoefficient(int sliderValue)
{
    float restitutionCoefficient = m_restitutionCoefficientSliderStep * sliderValue;
    m_restitutionCoefficientLabel->setText(QString().setNum(restitutionCoefficient, 'g', 6));
    m_sphLiquidWorld->getSPHSolver()->setRestitutionCoefficient(restitutionCoefficient);
}

void LiquidSimulation::setFrictionCoefficient(int sliderValue)
{
    float frictionCoefficient = m_frictionCoefficientSliderStep * sliderValue;
    m_frictionCoefficientLabel->setText(QString().setNum(frictionCoefficient, 'g', 6));
    m_sphLiquidWorld->getSPHSolver()->setFrictionCoefficient(frictionCoefficient);
}

void LiquidSimulation::restoreDefaultParameters()
{
    m_timeStepSlider->setValue(m_timeStepMinMaxDefault[2]);
    m_xGravitySlider->setValue(m_xGravityMinMaxDefault[2]);
    m_yGravitySlider->setValue(m_yGravityMinMaxDefault[2]);
    m_zGravitySlider->setValue(m_zGravityMinMaxDefault[2]);
    m_particleRadiusSlider->setValue(m_particleRadiusMinMaxDefault[2]);
    m_kernelRadiusFactorSlider->setValue(m_kernelRadiusFactorMinMaxDefault[2]);
    m_restDensitySlider->setValue(m_restDensityMinMaxDefault[2]);
    m_viscosityCoefficientSlider->setValue(m_viscosityCoefficientMinMaxDefault[2]);
    m_pressureStiffnessCoefficientSlider->setValue(m_pressureStiffnessCoefficientMinMaxDefault[2]);
    m_negativePressureFactorSlider->setValue(1);
    m_negativePressureFactorSlider->setValue(m_negativePressureFactorMinMaxDefault[2]);
    m_surfaceTensionCoefficientSlider->setValue(m_surfaceTensionCoefficientMinMaxDefault[2]);
    m_surfaceTensionThresholdSlider->setValue(m_surfaceTensionThresholdMinMaxDefault[2]);
    m_restitutionCoefficientSlider->setValue(m_restitutionCoefficientMinMaxDefault[2]);
    m_frictionCoefficientSlider->setValue(m_frictionCoefficientMinMaxDefault[2]);
}

void LiquidSimulation::changeScenario(int index)
{
    m_currentScenario->cleanUpScenario();

    // Determine new Scenario from current selected Scenario
    m_currentScenario = m_scenarios[index];
    m_currentScenario->initScenario();

	m_simulatedTime = 0.f;
    m_startStopButton->setText("Start Simulation");
    m_isSimulationStarted = false;
}

void LiquidSimulation::updateSimulationInfo()
{
    m_framesPerSecondLabel->setText(QString().setNum(m_fps, 'g', 4) + " 1/s");
    m_simulatedTimeLabel->setText(QString().setNum(m_simulatedTime, 'g', 4) + " s");
    m_amountParticlesLabel->setText(QString::number(m_sphLiquidWorld->getSPHSolver()->getParticleCount()));
    m_particleMassLabel->setText(QString().setNum(m_sphLiquidWorld->getSPHSolver()->getParticleMass(), 'g', 3) + " kg");
    m_kernelRadiusLabel->setText(QString().setNum(m_sphLiquidWorld->getSPHSolver()->getKernelRadius(), 'g', 3) + " m");
}

void LiquidSimulation::buildControlWidget(QWidget* parent)
{
    QHBoxLayout* controlWidgetLayout = new QHBoxLayout(parent);

    //----------- Left controls -------------
    QVBoxLayout* leftControlLayout = new QVBoxLayout();
    controlWidgetLayout->addLayout(leftControlLayout);

    QGroupBox* simulationControlsGroupBox = new QGroupBox("Simulation Controls");
    simulationControlsGroupBox->setMinimumWidth(200);
    leftControlLayout->addWidget(simulationControlsGroupBox);

    QVBoxLayout* simulationControlsLayout = new QVBoxLayout(simulationControlsGroupBox);

    // Start/Stop + Reset buttons
    m_startStopButton = new QPushButton("Start Simulation");
    simulationControlsLayout->addWidget(m_startStopButton);
    connect(m_startStopButton, &QPushButton::pressed, this, &LiquidSimulation::startStopSimulation);
    m_stepButton = new QPushButton("Step Simulation");
    simulationControlsLayout->addWidget(m_stepButton);
    connect(m_stepButton, &QPushButton::pressed, this, &LiquidSimulation::stepSimulation);
    m_resetButton = new QPushButton("Reset Simulation");
    simulationControlsLayout->addWidget(m_resetButton);
    connect(m_resetButton, &QPushButton::pressed, this, &LiquidSimulation::resetSimulation);

	// Parallelization Selection
	QLabel* parallelizationSelectionLabel = new QLabel("Parallelization Selection:");
	simulationControlsLayout->addWidget(parallelizationSelectionLabel);
	m_parallelizationSelection = new QComboBox();
    if(m_sphLiquidWorld->getSPHSolver()->hasGPU())
	    m_parallelizationSelection->addItem("GPU");
    if (m_sphLiquidWorld->getSPHSolver()->hasCPU())
	    m_parallelizationSelection->addItem("CPU");
	m_parallelizationSelection->addItem("NONE");
	simulationControlsLayout->addWidget(m_parallelizationSelection);
	connect(m_parallelizationSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) { this->changeParallelization(index); });

	// Simulation Method Selection
	QLabel* simulationMethodSelectionLabel = new QLabel("Simulation Method Selection:");
	simulationControlsLayout->addWidget(simulationMethodSelectionLabel);
	m_simulationMethodSelection = new QComboBox();
	m_simulationMethodSelection->addItem("SPH");
	m_simulationMethodSelection->addItem("PCISPH");
	simulationControlsLayout->addWidget(m_simulationMethodSelection);
	connect(m_simulationMethodSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) { this->changeSimulationMethod(index); });

    // Time Step
    QHBoxLayout* timeStepLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(timeStepLayout);
    QLabel* timeStepLabel = new QLabel("Time Step:");
    timeStepLayout->addWidget(timeStepLabel);
    m_timeStepLabel = new QLabel();
    m_timeStepLabel->setAlignment(Qt::AlignRight);
    timeStepLayout->addWidget(m_timeStepLabel);

    m_timeStepSlider = new QSlider(Qt::Horizontal);
    m_timeStepSlider->setRange(m_timeStepMinMaxDefault[0], m_timeStepMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_timeStepSlider);
    connect(m_timeStepSlider, &QSlider::valueChanged, this, &LiquidSimulation::setTimeStep);

    // Gravity
    QHBoxLayout* gravityLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(gravityLayout);
    QLabel* gravityLabel = new QLabel("Gravity:");
    gravityLayout->addWidget(gravityLabel);
    m_gravityLabel = new QLabel();
    m_gravityLabel->setAlignment(Qt::AlignRight);
    gravityLayout->addWidget(m_gravityLabel);

    QHBoxLayout* xGravityLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(xGravityLayout);
    QLabel* xGravityLabel = new QLabel("X");
    xGravityLayout->addWidget(xGravityLabel);
    m_xGravitySlider = new QSlider(Qt::Horizontal);
    m_xGravitySlider->setRange(m_xGravityMinMaxDefault[0], m_xGravityMinMaxDefault[1]);
    xGravityLayout->addWidget(m_xGravitySlider);
    connect(m_xGravitySlider, &QSlider::valueChanged, this, &LiquidSimulation::setXGravity);

    QHBoxLayout* yGravityLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(yGravityLayout);
    QLabel* yGravityLabel = new QLabel("Y");
    yGravityLayout->addWidget(yGravityLabel);
    m_yGravitySlider = new QSlider(Qt::Horizontal);
    m_yGravitySlider->setRange(m_yGravityMinMaxDefault[0], m_yGravityMinMaxDefault[1]);
    yGravityLayout->addWidget(m_yGravitySlider);
    connect(m_yGravitySlider, &QSlider::valueChanged, this, &LiquidSimulation::setYGravity);

    QHBoxLayout* zGravityLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(zGravityLayout);
    QLabel* zGravityLabel = new QLabel("Z");
    zGravityLayout->addWidget(zGravityLabel);
    m_zGravitySlider = new QSlider(Qt::Horizontal);
    m_zGravitySlider->setRange(m_zGravityMinMaxDefault[0], m_zGravityMinMaxDefault[1]);
    zGravityLayout->addWidget(m_zGravitySlider);
    connect(m_zGravitySlider, &QSlider::valueChanged, this, &LiquidSimulation::setZGravity);

    // Particle Radius
    QHBoxLayout* particleRadiusLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(particleRadiusLayout);
    QLabel* particleRadiusLabel = new QLabel("Particle Radius:");
    particleRadiusLayout->addWidget(particleRadiusLabel);
    m_particleRadiusLabel = new QLabel();
    m_particleRadiusLabel->setAlignment(Qt::AlignRight);
    particleRadiusLayout->addWidget(m_particleRadiusLabel);

    m_particleRadiusSlider = new QSlider(Qt::Horizontal);
    m_particleRadiusSlider->setRange(m_particleRadiusMinMaxDefault[0], m_particleRadiusMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_particleRadiusSlider);
    connect(m_particleRadiusSlider, &QSlider::valueChanged, this, &LiquidSimulation::setParticleRadius);

    // Kernel Radius Factor
    QHBoxLayout* kernelRadiusFactorLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(kernelRadiusFactorLayout);
    QLabel* kernelRadiusFactorLabel = new QLabel("Kernel Radius Factor:");
    kernelRadiusFactorLayout->addWidget(kernelRadiusFactorLabel);
    m_kernelRadiusFactorLabel = new QLabel();
    m_kernelRadiusFactorLabel->setAlignment(Qt::AlignRight);
    kernelRadiusFactorLayout->addWidget(m_kernelRadiusFactorLabel);

    m_kernelRadiusFactorSlider = new QSlider(Qt::Horizontal);
    m_kernelRadiusFactorSlider->setRange(m_kernelRadiusFactorMinMaxDefault[0], m_kernelRadiusFactorMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_kernelRadiusFactorSlider);
    connect(m_kernelRadiusFactorSlider, &QSlider::valueChanged, this, &LiquidSimulation::setKernelRadiusFactor);

    // Rest Density
    QHBoxLayout* restDensityLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(restDensityLayout);
    QLabel* restDensityLabel = new QLabel("Rest Density:");
    restDensityLayout->addWidget(restDensityLabel);
    m_restDensityLabel = new QLabel();
    m_restDensityLabel->setAlignment(Qt::AlignRight);
    restDensityLayout->addWidget(m_restDensityLabel);

    m_restDensitySlider = new QSlider(Qt::Horizontal);
    m_restDensitySlider->setRange(m_restDensityMinMaxDefault[0], m_restDensityMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_restDensitySlider);
    connect(m_restDensitySlider, &QSlider::valueChanged, this, &LiquidSimulation::setRestDensity);

    // Viscosity Coefficient
    QHBoxLayout* viscosityCoefficientLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(viscosityCoefficientLayout);
    QLabel* viscosityCoefficientLabel = new QLabel("Viscosity Coefficient:");
    viscosityCoefficientLayout->addWidget(viscosityCoefficientLabel);
    m_viscosityCoefficientLabel = new QLabel();
    m_viscosityCoefficientLabel->setAlignment(Qt::AlignRight);
    viscosityCoefficientLayout->addWidget(m_viscosityCoefficientLabel);

    m_viscosityCoefficientSlider = new QSlider(Qt::Horizontal);
    m_viscosityCoefficientSlider->setRange(m_viscosityCoefficientMinMaxDefault[0], m_viscosityCoefficientMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_viscosityCoefficientSlider);
    connect(m_viscosityCoefficientSlider, &QSlider::valueChanged, this, &LiquidSimulation::setViscosityCoefficient);

    // Pressure Stiffness Coefficient
    QHBoxLayout* pressureStiffnessCoefficientLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(pressureStiffnessCoefficientLayout);
    QLabel* pressureStiffnessCoefficientLabel = new QLabel("Pressure Stiffness Coefficient:");
    pressureStiffnessCoefficientLayout->addWidget(pressureStiffnessCoefficientLabel);
    m_pressureStiffnessCoefficientLabel = new QLabel();
    m_pressureStiffnessCoefficientLabel->setAlignment(Qt::AlignRight);
    pressureStiffnessCoefficientLayout->addWidget(m_pressureStiffnessCoefficientLabel);

    m_pressureStiffnessCoefficientSlider = new QSlider(Qt::Horizontal);
    m_pressureStiffnessCoefficientSlider->setRange(m_pressureStiffnessCoefficientMinMaxDefault[0], m_pressureStiffnessCoefficientMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_pressureStiffnessCoefficientSlider);
    connect(m_pressureStiffnessCoefficientSlider, &QSlider::valueChanged, this, &LiquidSimulation::setPressureStiffnessCoefficient);

    // Negative Pressure Factor
    QHBoxLayout* negativePressureFactorLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(negativePressureFactorLayout);
    QLabel* negativePressureFactorLabel = new QLabel("Negative Pressure Factor:");
    negativePressureFactorLayout->addWidget(negativePressureFactorLabel);
    m_negativePressureFactorLabel = new QLabel();
    m_negativePressureFactorLabel->setAlignment(Qt::AlignRight);
    negativePressureFactorLayout->addWidget(m_negativePressureFactorLabel);

    m_negativePressureFactorSlider = new QSlider(Qt::Horizontal);
    m_negativePressureFactorSlider->setRange(m_negativePressureFactorMinMaxDefault[0], m_negativePressureFactorMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_negativePressureFactorSlider);
    connect(m_negativePressureFactorSlider, &QSlider::valueChanged, this, &LiquidSimulation::setNegativePressureFactor);

    // Surface Tension Coefficient
    QHBoxLayout* surfaceTensionCoefficientLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(surfaceTensionCoefficientLayout);
    QLabel* surfaceTensionCoefficientLabel = new QLabel("Surface Tension Coefficient:");
    surfaceTensionCoefficientLayout->addWidget(surfaceTensionCoefficientLabel);
    m_surfaceTensionCoefficientLabel = new QLabel();
    m_surfaceTensionCoefficientLabel->setAlignment(Qt::AlignRight);
    surfaceTensionCoefficientLayout->addWidget(m_surfaceTensionCoefficientLabel);

    m_surfaceTensionCoefficientSlider = new QSlider(Qt::Horizontal);
    m_surfaceTensionCoefficientSlider->setRange(m_surfaceTensionCoefficientMinMaxDefault[0], m_surfaceTensionCoefficientMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_surfaceTensionCoefficientSlider);
    connect(m_surfaceTensionCoefficientSlider, &QSlider::valueChanged, this, &LiquidSimulation::setSurfaceTensionCoefficient);

    // Surface Tension Threshold
    QHBoxLayout* surfaceTensionThresholdLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(surfaceTensionThresholdLayout);
    QLabel* surfaceTensionThresholdLabel = new QLabel("Surface Tension Threshold:");
    surfaceTensionThresholdLayout->addWidget(surfaceTensionThresholdLabel);
    m_surfaceTensionThresholdLabel = new QLabel();
    m_surfaceTensionThresholdLabel->setAlignment(Qt::AlignRight);
    surfaceTensionThresholdLayout->addWidget(m_surfaceTensionThresholdLabel);

    m_surfaceTensionThresholdSlider = new QSlider(Qt::Horizontal);
    m_surfaceTensionThresholdSlider->setRange(m_surfaceTensionThresholdMinMaxDefault[0], m_surfaceTensionThresholdMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_surfaceTensionThresholdSlider);
    connect(m_surfaceTensionThresholdSlider, &QSlider::valueChanged, this, &LiquidSimulation::setSurfaceTensionThreshold);

    // Restitution Coefficient
    QHBoxLayout* restitutionCoefficientLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(restitutionCoefficientLayout);
    QLabel* restitutionCoefficientLabel = new QLabel("Restitution Coefficient:");
    restitutionCoefficientLayout->addWidget(restitutionCoefficientLabel);
    m_restitutionCoefficientLabel = new QLabel();
    m_restitutionCoefficientLabel->setAlignment(Qt::AlignRight);
    restitutionCoefficientLayout->addWidget(m_restitutionCoefficientLabel);

    m_restitutionCoefficientSlider = new QSlider(Qt::Horizontal);
    m_restitutionCoefficientSlider->setRange(m_restitutionCoefficientMinMaxDefault[0], m_restitutionCoefficientMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_restitutionCoefficientSlider);
    connect(m_restitutionCoefficientSlider, &QSlider::valueChanged, this, &LiquidSimulation::setRestitutionCoefficient);

    // Friction Coefficient
    QHBoxLayout* frictionCoefficientLayout = new QHBoxLayout();
    simulationControlsLayout->addLayout(frictionCoefficientLayout);
    QLabel* frictionCoefficientLabel = new QLabel("Friction Coefficient:");
    frictionCoefficientLayout->addWidget(frictionCoefficientLabel);
    m_frictionCoefficientLabel = new QLabel();
    m_frictionCoefficientLabel->setAlignment(Qt::AlignRight);
    frictionCoefficientLayout->addWidget(m_frictionCoefficientLabel);

    m_frictionCoefficientSlider = new QSlider(Qt::Horizontal);
    m_frictionCoefficientSlider->setRange(m_frictionCoefficientMinMaxDefault[0], m_frictionCoefficientMinMaxDefault[1]);
    simulationControlsLayout->addWidget(m_frictionCoefficientSlider);
    connect(m_frictionCoefficientSlider, &QSlider::valueChanged, this, &LiquidSimulation::setFrictionCoefficient);

    // Restore Default Parameters
    m_restoreDefaultParametersButton = new QPushButton("Restore Default Parameters");
    simulationControlsLayout->addWidget(m_restoreDefaultParametersButton);
    connect(m_restoreDefaultParametersButton, &QPushButton::pressed, this, &LiquidSimulation::restoreDefaultParameters);

    leftControlLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

    //----------- Right controls -------------
    QVBoxLayout* rightControlLayout = new QVBoxLayout();
    controlWidgetLayout->addLayout(rightControlLayout);

    QGroupBox* scenarioControlsGroupBox = new QGroupBox("Scenario Controls");
    scenarioControlsGroupBox->setMinimumWidth(200);
    rightControlLayout->addWidget(scenarioControlsGroupBox);

    QVBoxLayout* scenarioControlsLayout = new QVBoxLayout(scenarioControlsGroupBox);

    // Scenario Selection
    QLabel* scenarioSelectionLabel = new QLabel("Scenario Selection:");
    scenarioControlsLayout->addWidget(scenarioSelectionLabel);
    m_scenarioSelection = new QComboBox();
    scenarioControlsLayout->addWidget(m_scenarioSelection);
    connect(m_scenarioSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){ this->changeScenario(index); });

    // Scenario Stacked Widget
    m_scenarioControlsStackedWidget = new QStackedWidget();
    m_scenarioControlsStackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    scenarioControlsLayout->addWidget(m_scenarioControlsStackedWidget);
    connect(m_scenarioSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){ m_scenarioControlsStackedWidget->setCurrentIndex(index); });

    QGroupBox* simulationInfoGroupBox = new QGroupBox("Simulation Information");
    rightControlLayout->addWidget(simulationInfoGroupBox);

    QVBoxLayout* simulationInfoLayout = new QVBoxLayout(simulationInfoGroupBox);

    // Frames per Second
    QHBoxLayout* framesPerSecondLayout = new QHBoxLayout();
    simulationInfoLayout->addLayout(framesPerSecondLayout);
    QLabel* framesPerSecondLabel = new QLabel("Frames per Second:");
    framesPerSecondLayout->addWidget(framesPerSecondLabel);
    m_framesPerSecondLabel = new QLabel();
    m_framesPerSecondLabel->setAlignment(Qt::AlignRight);
    framesPerSecondLayout->addWidget(m_framesPerSecondLabel);

    // Simulated Time
    QHBoxLayout* simulatedTimeLayout = new QHBoxLayout();
    simulationInfoLayout->addLayout(simulatedTimeLayout);
    QLabel* simulatedTimeLabel = new QLabel("Simulated Time:");
    simulatedTimeLayout->addWidget(simulatedTimeLabel);
    m_simulatedTimeLabel = new QLabel();
    m_simulatedTimeLabel->setAlignment(Qt::AlignRight);
    simulatedTimeLayout->addWidget(m_simulatedTimeLabel);

    // Amount of Particles
    QHBoxLayout* amountParticlesLayout = new QHBoxLayout();
    simulationInfoLayout->addLayout(amountParticlesLayout);
    QLabel* amountParticlesLabel = new QLabel("Amount of Particles:");
    amountParticlesLayout->addWidget(amountParticlesLabel);
    m_amountParticlesLabel = new QLabel();
    m_amountParticlesLabel->setAlignment(Qt::AlignRight);
    amountParticlesLayout->addWidget(m_amountParticlesLabel);

    // Particle Mass
    QHBoxLayout* particleMassLayout = new QHBoxLayout();
    simulationInfoLayout->addLayout(particleMassLayout);
    QLabel* particleMassLabel = new QLabel("Particle Mass:");
    particleMassLayout->addWidget(particleMassLabel);
    m_particleMassLabel = new QLabel();
    m_particleMassLabel->setAlignment(Qt::AlignRight);
    particleMassLayout->addWidget(m_particleMassLabel);

    // Kernel Radius
    QHBoxLayout* kernelRadiusLayout = new QHBoxLayout();
    simulationInfoLayout->addLayout(kernelRadiusLayout);
    QLabel* kernelRadiusLabel = new QLabel("Kernel Radius:");
    kernelRadiusLayout->addWidget(kernelRadiusLabel);
    m_kernelRadiusLabel = new QLabel();
    m_kernelRadiusLabel->setAlignment(Qt::AlignRight);
    kernelRadiusLayout->addWidget(m_kernelRadiusLabel);

    rightControlLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
}
