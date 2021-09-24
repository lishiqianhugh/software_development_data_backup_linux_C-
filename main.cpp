#include "widget.h"

#include <QApplication>
#include <QPushButton>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
//    QPushButton *button = new QPushButton("Quit");
//    QObject::connect(button, SIGNAL(clicked()),&a,SLOT(quit()));

//    button->show();
    return a.exec();
}
