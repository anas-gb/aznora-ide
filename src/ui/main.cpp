#include <QApplication>

#include "ui/MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Aznora"));
    app.setApplicationDisplayName(QStringLiteral("Aznora IDE"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));

    aznora::ui::MainWindow window;
    window.show();

    return app.exec();
}
