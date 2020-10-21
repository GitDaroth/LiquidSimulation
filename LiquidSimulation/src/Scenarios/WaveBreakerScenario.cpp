#include "Scenarios/WaveBreakerScenario.h"

WaveBreakerScenario::WaveBreakerScenario(QString name, SPHLiquidWorld* sphFluidWorld) :
    LiquidScenario(name, sphFluidWorld)
{
    m_frequency = 1.f;
    m_amplitude = 0.1f;
    m_simulatedTime = 0.f;

    m_frequencyMinMaxDefault[0] =  0;
    m_frequencyMinMaxDefault[1] =  40;
    m_frequencyMinMaxDefault[2] =  10;
    m_frequencySliderStep = 0.1f;

    m_amplitudeMinMaxDefault[0] =  0;
    m_amplitudeMinMaxDefault[1] =  20;
    m_amplitudeMinMaxDefault[2] =  10;
    m_amplitudeSliderStep = 0.01f;

    buildScenarioWidget();

    restoreDefaultParameters();
}

void WaveBreakerScenario::initScenario()
{
    Vector3D halfDimensions(1.2f, 0.6f, 0.6f);
	std::vector<Vector3D> spawnedParticles = SPHParticleEmitter::spawnSphere(Vector3D(0.6f, 0.f, 0.f), 0.6f, m_sphLiquidWorld->getSPHSolver()->getParticleRadius());

    for (Vector3D particlePosition : spawnedParticles)
    {
        SPHParticle* sphParticle = new PCISPHParticle();
        sphParticle->setPosition(particlePosition);

        SPHParticleDrawable* particleDrawable = new SPHParticleDrawable(sphParticle, new InstancedDrawable());
        m_sphLiquidWorld->addSPHParticleDrawable(particleDrawable);
    }

    m_boundaryBox = new StaticCollisionBox(Vector3D(0.f, 0.f, 0.f), halfDimensions, StaticCollisionObjectType::BOUNDARY);
    StaticCollisionObjectDrawable* boundarBoxDrawable = new StaticCollisionObjectDrawable(m_boundaryBox, new Drawable());
    m_sphLiquidWorld->addStaticCollisionObjectDrawable(boundarBoxDrawable);

	StaticCollisionBox* obstacleBox1 = new StaticCollisionBox(Vector3D(-0.2f, -0.2f, 0.f), Vector3D(0.1f, 0.4f, 0.1f), StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleBoxDrawable1 = new StaticCollisionObjectDrawable(obstacleBox1, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleBoxDrawable1);

	StaticCollisionBox* obstacleBox2 = new StaticCollisionBox(Vector3D(-0.2f, -0.2f, -0.4f), Vector3D(0.1f, 0.4f, 0.1f), StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleBoxDrawable2 = new StaticCollisionObjectDrawable(obstacleBox2, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleBoxDrawable2);

	StaticCollisionBox* obstacleBox3 = new StaticCollisionBox(Vector3D(-0.2f, -0.2f, 0.4f), Vector3D(0.1f, 0.4f, 0.1f), StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleBoxDrawable3 = new StaticCollisionObjectDrawable(obstacleBox3, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleBoxDrawable3);

    m_simulatedTime = 0.f;
}

void WaveBreakerScenario::updateScenario(float deltaTime)
{
    float sinValue = sin(m_simulatedTime * m_frequency * 2.f * M_PI) * m_amplitude;
    m_simulatedTime += deltaTime;

    Vector3D halfDimensions = m_boundaryBox->getHalfDimensions();
    halfDimensions.setX(1.2f - sinValue / 2.f);
    m_boundaryBox->setHalfDimensions(halfDimensions);

    Vector3D position = m_boundaryBox->getPosition();
    position.setX(sinValue / 2.f);
    m_boundaryBox->setPosition(position);

	m_sphLiquidWorld->getSPHSolver()->setHasCollisionObjectDataChanged(true);
}

void WaveBreakerScenario::setFrequency(int sliderValue)
{
    float sinTimeValue = m_simulatedTime * m_frequency;
    m_frequency = m_frequencySliderStep * sliderValue;
    if(sliderValue > 0)
        m_simulatedTime = sinTimeValue / m_frequency;
    m_frequencyLabel->setText(QString().setNum(m_frequency, 'g', 6) + " 1/s");
}

void WaveBreakerScenario::setAmplitude(int sliderValue)
{
    m_amplitude = m_amplitudeSliderStep * sliderValue;
    m_amplitudeLabel->setText(QString().setNum(m_amplitude, 'g', 6) + " m");
}

void WaveBreakerScenario::restoreDefaultParameters()
{
    m_frequencySlider->setValue(m_frequencyMinMaxDefault[2]);
    m_amplitudeSlider->setValue(m_amplitudeMinMaxDefault[2]);
}

void WaveBreakerScenario::buildScenarioWidget()
{
    m_specificScenarioWidget = new QWidget();
    QVBoxLayout* scenarioWidgetLayout = new QVBoxLayout(m_specificScenarioWidget);
    scenarioWidgetLayout->setMargin(0);

    // Frequency
    QHBoxLayout* frequencyLayout = new QHBoxLayout();
    scenarioWidgetLayout->addLayout(frequencyLayout);
    QLabel* frequencyLabel = new QLabel("Generation Frequency:");
    frequencyLayout->addWidget(frequencyLabel);
    m_frequencyLabel = new QLabel();
    m_frequencyLabel->setAlignment(Qt::AlignRight);
    frequencyLayout->addWidget(m_frequencyLabel);

    m_frequencySlider = new QSlider(Qt::Horizontal);
    m_frequencySlider->setRange(m_frequencyMinMaxDefault[0], m_frequencyMinMaxDefault[1]);
    scenarioWidgetLayout->addWidget(m_frequencySlider);
    connect(m_frequencySlider, &QSlider::valueChanged, this, &WaveBreakerScenario::setFrequency);

    // Amplitude
    QHBoxLayout* amplitudeLayout = new QHBoxLayout();
    scenarioWidgetLayout->addLayout(amplitudeLayout);
    QLabel* amplitudeLabel = new QLabel("Generation Amplitude:");
    amplitudeLayout->addWidget(amplitudeLabel);
    m_amplitudeLabel = new QLabel();
    m_amplitudeLabel->setAlignment(Qt::AlignRight);
    amplitudeLayout->addWidget(m_amplitudeLabel);

    m_amplitudeSlider = new QSlider(Qt::Horizontal);
    m_amplitudeSlider->setRange(m_amplitudeMinMaxDefault[0], m_amplitudeMinMaxDefault[1]);
    scenarioWidgetLayout->addWidget(m_amplitudeSlider);
    connect(m_amplitudeSlider, &QSlider::valueChanged, this, &WaveBreakerScenario::setAmplitude);

    // Restore Default Parameters
    m_restoreDefaultParametersButton = new QPushButton("Restore Default Parameters");
    scenarioWidgetLayout->addWidget(m_restoreDefaultParametersButton);
    connect(m_restoreDefaultParametersButton, &QPushButton::pressed, this, &WaveBreakerScenario::restoreDefaultParameters);
}
