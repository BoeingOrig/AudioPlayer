#include "AudioPlayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AudioPlayer window;
    window.show();
    return app.exec();
}
