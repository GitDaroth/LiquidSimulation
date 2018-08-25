#include <QApplication>
#include <QSurfaceFormat>
#include "LiquidSimulation.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QSurfaceFormat surfaceFormat;
	surfaceFormat.setDepthBufferSize(24);
	surfaceFormat.setStencilBufferSize(8);
	surfaceFormat.setSamples(16);
	surfaceFormat.setVersion(3, 3);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	QSurfaceFormat::setDefaultFormat(surfaceFormat);

	new LiquidSimulation();

	return a.exec();
}
