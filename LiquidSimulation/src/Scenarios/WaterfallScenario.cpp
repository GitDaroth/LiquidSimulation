#include "WaterFallScenario.h"

WaterfallScenario::WaterfallScenario(QString name, SPHLiquidWorld* sphFluidWorld) :
	LiquidScenario(name, sphFluidWorld)
{
	m_inflowSpeed = 1.8f;
	m_inflowSize = 0.1f;
	m_simulatedTime = 0.f;

	m_inflowSpeedMinMaxDefault[0] = 0;
	m_inflowSpeedMinMaxDefault[1] = 30;
	m_inflowSpeedMinMaxDefault[2] = 18;
	m_inflowSpeedSliderStep = 0.1f;

	m_inflowSizeMinMaxDefault[0] = 5;
	m_inflowSizeMinMaxDefault[1] = 20;
	m_inflowSizeMinMaxDefault[2] = 10;
	m_inflowSizeSliderStep = 0.01f;

	buildScenarioWidget();

	restoreDefaultParameters();
}

void WaterfallScenario::initScenario()
{
	std::vector<Vector3D> spawnedParticles = SPHParticleEmitter::spawnCube(Vector3D(-0.75f, -0.2f, 0.f), Vector3D(0.45f, 0.1f, 0.6f), m_sphLiquidWorld->getSPHSolver()->getParticleRadius());

	for (Vector3D particlePosition : spawnedParticles)
	{
		SPHParticle* sphParticle = new PCISPHParticle();
		sphParticle->setPosition(particlePosition);

		SPHParticleDrawable* particleDrawable = new SPHParticleDrawable(sphParticle, new InstancedDrawable());
		m_sphLiquidWorld->addSPHParticleDrawable(particleDrawable);
	}

	Vector3D halfDimensions(1.2f, 0.6f, 0.6f);

	StaticCollisionBox* m_boundaryBox = new StaticCollisionBox(Vector3D(0.f, 0.f, 0.f), halfDimensions, StaticCollisionObjectType::BOUNDARY);
	StaticCollisionObjectDrawable* collisionObjectDrawable = new StaticCollisionObjectDrawable(m_boundaryBox, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(collisionObjectDrawable);

	StaticCollisionBox* obstacleBox1 = new StaticCollisionBox(Vector3D(-0.2f, -0.35f, 0.f), Vector3D(0.1f, 0.25f, 0.6f), StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleBoxDrawable1 = new StaticCollisionObjectDrawable(obstacleBox1, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleBoxDrawable1);

	StaticCollisionBox* obstacleBox2 = new StaticCollisionBox(Vector3D(-0.7f, -0.45f, 0.f), Vector3D(0.5f, 0.15f, 0.6f), StaticCollisionObjectType::OBSTACLE);
	StaticCollisionObjectDrawable* obstacleBoxDrawable2 = new StaticCollisionObjectDrawable(obstacleBox2, new Drawable());
	m_sphLiquidWorld->addStaticCollisionObjectDrawable(obstacleBoxDrawable2);

	m_simulatedTime = 0.f;
}

void WaterfallScenario::updateScenario(float deltaTime)
{
	m_simulatedTime += deltaTime;

	if (m_simulatedTime >= ((m_sphLiquidWorld->getSPHSolver()->getParticleRadius() * 1.6f) / m_inflowSpeed))
	{
		std::vector<Vector3D> spawnedParticles = SPHParticleEmitter::spawnCircleZ(Vector3D(-0.8f, 0.3f, -0.6f + m_sphLiquidWorld->getSPHSolver()->getParticleRadius()), m_inflowSize, m_sphLiquidWorld->getSPHSolver()->getParticleRadius());
		for (Vector3D particlePosition : spawnedParticles)
		{
			SPHParticle* sphParticle = new PCISPHParticle();
			sphParticle->setPosition(particlePosition);
			sphParticle->setVelocity(Vector3D(0.f, 0.f, m_inflowSpeed));

			SPHParticleDrawable* particleDrawable = new SPHParticleDrawable(sphParticle, new InstancedDrawable());
			m_sphLiquidWorld->addSPHParticleDrawable(particleDrawable);
		}

		m_simulatedTime = 0.f;
	}
}

void WaterfallScenario::setInflowSpeed(int sliderValue)
{
	m_inflowSpeed = m_inflowSpeedSliderStep * sliderValue;
	m_inflowSpeedLabel->setText(QString().setNum(m_inflowSpeed, 'g', 6) + " m/s");
}

void WaterfallScenario::setInflowSize(int sliderValue)
{
	m_inflowSize = m_inflowSizeSliderStep * sliderValue;
	m_inflowSizeLabel->setText(QString().setNum(m_inflowSize, 'g', 6) + " m");
}

void WaterfallScenario::restoreDefaultParameters()
{
	m_inflowSpeedSlider->setValue(m_inflowSpeedMinMaxDefault[2]);
	m_inflowSizeSlider->setValue(m_inflowSizeMinMaxDefault[2]);
}

void WaterfallScenario::buildScenarioWidget()
{
	m_specificScenarioWidget = new QWidget();
	QVBoxLayout* scenarioWidgetLayout = new QVBoxLayout(m_specificScenarioWidget);
	scenarioWidgetLayout->setMargin(0);

	// Inflow Speed
	QHBoxLayout* inflowSpeedLayout = new QHBoxLayout();
	scenarioWidgetLayout->addLayout(inflowSpeedLayout);
	QLabel* inflowSpeedLabel = new QLabel("Inflow Speed:");
	inflowSpeedLayout->addWidget(inflowSpeedLabel);
	m_inflowSpeedLabel = new QLabel();
	m_inflowSpeedLabel->setAlignment(Qt::AlignRight);
	inflowSpeedLayout->addWidget(m_inflowSpeedLabel);

	m_inflowSpeedSlider = new QSlider(Qt::Horizontal);
	m_inflowSpeedSlider->setRange(m_inflowSpeedMinMaxDefault[0], m_inflowSpeedMinMaxDefault[1]);
	scenarioWidgetLayout->addWidget(m_inflowSpeedSlider);
	connect(m_inflowSpeedSlider, &QSlider::valueChanged, this, &WaterfallScenario::setInflowSpeed);

	// Inflow Size
	QHBoxLayout* inflowSizeLayout = new QHBoxLayout();
	scenarioWidgetLayout->addLayout(inflowSizeLayout);
	QLabel* inflowSizeLabel = new QLabel("Inflow Size:");
	inflowSizeLayout->addWidget(inflowSizeLabel);
	m_inflowSizeLabel = new QLabel();
	m_inflowSizeLabel->setAlignment(Qt::AlignRight);
	inflowSizeLayout->addWidget(m_inflowSizeLabel);

	m_inflowSizeSlider = new QSlider(Qt::Horizontal);
	m_inflowSizeSlider->setRange(m_inflowSizeMinMaxDefault[0], m_inflowSizeMinMaxDefault[1]);
	scenarioWidgetLayout->addWidget(m_inflowSizeSlider);
	connect(m_inflowSizeSlider, &QSlider::valueChanged, this, &WaterfallScenario::setInflowSize);

	// Restore Default Parameters
	m_restoreDefaultParametersButton = new QPushButton("Restore Default Parameters");
	scenarioWidgetLayout->addWidget(m_restoreDefaultParametersButton);
	connect(m_restoreDefaultParametersButton, &QPushButton::pressed, this, &WaterfallScenario::restoreDefaultParameters);
}
