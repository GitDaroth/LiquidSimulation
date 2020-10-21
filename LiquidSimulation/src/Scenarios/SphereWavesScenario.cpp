#include "Scenarios/SphereWavesScenario.h"

SphereWavesScenario::SphereWavesScenario(QString name, SPHLiquidWorld* sphFluidWorld) :
	LiquidScenario(name, sphFluidWorld)
{
	m_frequency = 0.6f;
	m_amplitude = 0.1f;
	m_simulatedTime = 0.f;

	m_frequencyMinMaxDefault[0] = 0;
	m_frequencyMinMaxDefault[1] = 10;
	m_frequencyMinMaxDefault[2] = 6;
	m_frequencySliderStep = 0.1f;

	m_amplitudeMinMaxDefault[0] = 0;
	m_amplitudeMinMaxDefault[1] = 15;
	m_amplitudeMinMaxDefault[2] = 10;
	m_amplitudeSliderStep = 0.01f;

	buildScenarioWidget();

	restoreDefaultParameters();
}

void SphereWavesScenario::initScenario()
{
	std::vector<Vector3D> spawnedParticles = SPHParticleEmitter::spawnCube(Vector3D(0.f, -0.25f, 0.f), Vector3D(0.45f, 0.45f, 0.45f), m_sphLiquidWorld->getSPHSolver()->getParticleRadius());

	for (Vector3D particlePosition : spawnedParticles)
	{
		SPHParticle* sphParticle = new PCISPHParticle();
		sphParticle->setPosition(particlePosition);

		SPHParticleDrawable* particleDrawable = new SPHParticleDrawable(sphParticle, new InstancedDrawable());
		m_sphLiquidWorld->addSPHParticleDrawable(particleDrawable);
	}

	m_boundarySphere = new StaticCollisionSphere(Vector3D(0.f, 0.f, 0.f), 1.f, StaticCollisionObjectType::BOUNDARY);
	StaticCollisionObjectDrawable* boundarBoxDrawable = new StaticCollisionObjectDrawable(m_boundarySphere, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(boundarBoxDrawable);

	StaticCollisionBox* obstacleBox1 = new StaticCollisionBox(Vector3D(0.f, -0.65f, 0.f), Vector3D(0.1f, 0.5f, 0.1f), StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleBoxDrawable1 = new StaticCollisionObjectDrawable(obstacleBox1, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleBoxDrawable1);

	m_simulatedTime = 0.f;
}

void SphereWavesScenario::updateScenario(float deltaTime)
{
	float sinValue = sin(m_simulatedTime * m_frequency * 2.f * M_PI) * m_amplitude;
	m_simulatedTime += deltaTime;

	m_boundarySphere->setRadius(1.f + sinValue);

	m_sphLiquidWorld->getSPHSolver()->setHasCollisionObjectDataChanged(true);
}

void SphereWavesScenario::setFrequency(int sliderValue)
{
	float sinTimeValue = m_simulatedTime * m_frequency;
	m_frequency = m_frequencySliderStep * sliderValue;
	if (sliderValue > 0)
		m_simulatedTime = sinTimeValue / m_frequency;
	m_frequencyLabel->setText(QString().setNum(m_frequency, 'g', 6) + " 1/s");
}

void SphereWavesScenario::setAmplitude(int sliderValue)
{
	m_amplitude = m_amplitudeSliderStep * sliderValue;
	m_amplitudeLabel->setText(QString().setNum(m_amplitude, 'g', 6) + " m");
}

void SphereWavesScenario::restoreDefaultParameters()
{
	m_frequencySlider->setValue(m_frequencyMinMaxDefault[2]);
	m_amplitudeSlider->setValue(m_amplitudeMinMaxDefault[2]);
}

void SphereWavesScenario::buildScenarioWidget()
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
	connect(m_frequencySlider, &QSlider::valueChanged, this, &SphereWavesScenario::setFrequency);

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
	connect(m_amplitudeSlider, &QSlider::valueChanged, this, &SphereWavesScenario::setAmplitude);

	// Restore Default Parameters
	m_restoreDefaultParametersButton = new QPushButton("Restore Default Parameters");
	scenarioWidgetLayout->addWidget(m_restoreDefaultParametersButton);
	connect(m_restoreDefaultParametersButton, &QPushButton::pressed, this, &SphereWavesScenario::restoreDefaultParameters);
}
