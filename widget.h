#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "QMenuBar"
#include <QAction>
#include <QVBoxLayout>
#include <QFileDialog>
#include "QMessageBox"
#include "QTextEdit"
#include "QPalette"
//#include "backup.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QString selectSrcFilename();
    QString selectDestFilename();
    void Backup();
    void Compress();
    void Uncompress();
    void Pack();
    void Unpack();
    void Encrypt();
    void Decrypt();
    QString enpass();
    QString depass();
    int valid();

private slots:


private:
    Ui::Widget *ui;
    QString src_path;
    QString dest_path;
};
#endif // WIDGET_H
