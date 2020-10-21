#include "Scenarios/WaterDropsScenario.h"

WaterDropsScenario::WaterDropsScenario(QString name, SPHLiquidWorld* sphFluidWorld) :
    LiquidScenario(name, sphFluidWorld)
{
	m_spawnRate = 1.f;
	m_dropSize = 0.1f;
	m_simulatedTime = 0.f;

	m_spawnRateMinMaxDefault[0] = 0;
	m_spawnRateMinMaxDefault[1] = 50;
	m_spawnRateMinMaxDefault[2] = 10;
	m_spawnRateSliderStep = 0.1f;

	m_dropSizeMinMaxDefault[0] = 5;
	m_dropSizeMinMaxDefault[1] = 20;
	m_dropSizeMinMaxDefault[2] = 10;
	m_dropSizeSliderStep = 0.01f;

	buildScenarioWidget();

	restoreDefaultParameters();
}

void WaterDropsScenario::initScenario()
{
    std::vector<Vector3D> spawnedParticles = SPHParticleEmitter::spawnCube(Vector3D(0.f, -0.7f, 0.f), Vector3D(0.6f, 0.1f, 0.6f), m_sphLiquidWorld->getSPHSolver()->getParticleRadius());

    for (Vector3D particlePosition : spawnedParticles)
    {
        SPHParticle* sphParticle = new PCISPHParticle();
        sphParticle->setPosition(particlePosition);

        SPHParticleDrawable* particleDrawable = new SPHParticleDrawable(sphParticle, new InstancedDrawable());
        m_sphLiquidWorld->addSPHParticleDrawable(particleDrawable);
    }

    Vector3D halfDimensions(0.6f, 0.8f, 0.6f);

    StaticCollisionBox* m_boundaryBox = new StaticCollisionBox(Vector3D(0.f, 0.f, 0.f), halfDimensions, StaticCollisionObjectType::BOUNDARY);
    StaticCollisionObjectDrawable* collisionObjectDrawable = new StaticCollisionObjectDrawable(m_boundaryBox, new Drawable());
    m_sphLiquidWorld->addStaticCollisionObjectDrawable(collisionObjectDrawable);

	m_simulatedTime = 0.f;
}

void WaterDropsScenario::updateScenario(float deltaTime)
{
	m_simulatedTime += deltaTime;

	if (m_simulatedTime >= (1.f / m_spawnRate))
	{
		float xRand = (qrand() % (100 + 1)) * 0.01f * 1.2f - 0.6f;
		float zRand = (qrand() % (100 + 1)) * 0.01f * 1.2f - 0.6f;

		if (xRand < 0.f)
			xRand += m_dropSize;
		else
			xRand -= m_dropSize;

		if (zRand < 0.f)
			zRand += m_dropSize;
		else
			zRand -= m_dropSize;

		std::vector<Vector3D> spawnedParticles = SPHParticleEmitter::spawnSphere(Vector3D(xRand, 0.5f, zRand), m_dropSize, m_sphLiquidWorld->getSPHSolver()->getParticleRadius());
		for (Vector3D particlePosition : spawnedParticles)
		{
			SPHParticle* sphParticle = new PCISPHParticle();
			sphParticle->setPosition(particlePosition);

			SPHParticleDrawable* particleDrawable = new SPHParticleDrawable(sphParticle, new InstancedDrawable());
			m_sphLiquidWorld->addSPHParticleDrawable(particleDrawable);
		}

		m_simulatedTime = 0.f;
	}
}

void WaterDropsScenario::setSpawnRate(int sliderValue)
{
	m_spawnRate = m_spawnRateSliderStep * sliderValue;
	m_spawnRateLabel->setText(QString().setNum(m_spawnRate, 'g', 6) + " 1/s");
}

void WaterDropsScenario::setDropSize(int sliderValue)
{
	m_dropSize = m_dropSizeSliderStep * sliderValue;
	m_dropSizeLabel->setText(QString().setNum(m_dropSize, 'g', 6) + " m");
}

void WaterDropsScenario::restoreDefaultParameters()
{
	m_spawnRateSlider->setValue(m_spawnRateMinMaxDefault[2]);
	m_dropSizeSlider->setValue(m_dropSizeMinMaxDefault[2]);
}

void WaterDropsScenario::buildScenarioWidget()
{
	m_specificScenarioWidget = new QWidget();
	QVBoxLayout* scenarioWidgetLayout = new QVBoxLayout(m_specificScenarioWidget);
	scenarioWidgetLayout->setMargin(0);

	// Spawn Rate
	QHBoxLayout* spawnRateLayout = new QHBoxLayout();
	scenarioWidgetLayout->addLayout(spawnRateLayout);
	QLabel* spawnRateLabel = new QLabel("Spawn Rate:");
	spawnRateLayout->addWidget(spawnRateLabel);
	m_spawnRateLabel = new QLabel();
	m_spawnRateLabel->setAlignment(Qt::AlignRight);
	spawnRateLayout->addWidget(m_spawnRateLabel);

	m_spawnRateSlider = new QSlider(Qt::Horizontal);
	m_spawnRateSlider->setRange(m_spawnRateMinMaxDefault[0], m_spawnRateMinMaxDefault[1]);
	scenarioWidgetLayout->addWidget(m_spawnRateSlider);
	connect(m_spawnRateSlider, &QSlider::valueChanged, this, &WaterDropsScenario::setSpawnRate);

	// Drop Size
	QHBoxLayout* dropSizeLayout = new QHBoxLayout();
	scenarioWidgetLayout->addLayout(dropSizeLayout);
	QLabel* dropSizeLabel = new QLabel("Drop Size:");
	dropSizeLayout->addWidget(dropSizeLabel);
	m_dropSizeLabel = new QLabel();
	m_dropSizeLabel->setAlignment(Qt::AlignRight);
	dropSizeLayout->addWidget(m_dropSizeLabel);

	m_dropSizeSlider = new QSlider(Qt::Horizontal);
	m_dropSizeSlider->setRange(m_dropSizeMinMaxDefault[0], m_dropSizeMinMaxDefault[1]);
	scenarioWidgetLayout->addWidget(m_dropSizeSlider);
	connect(m_dropSizeSlider, &QSlider::valueChanged, this, &WaterDropsScenario::setDropSize);

	// Restore Default Parameters
	m_restoreDefaultParametersButton = new QPushButton("Restore Default Parameters");
	scenarioWidgetLayout->addWidget(m_restoreDefaultParametersButton);
	connect(m_restoreDefaultParametersButton, &QPushButton::pressed, this, &WaterDropsScenario::restoreDefaultParameters);
}
