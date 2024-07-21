#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zlgcan.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    DEVICE_HANDLE  canHandle;
private slots:
    void on_pushButton_clicked();
    void on_btncon_click();
    void point_click();
    void on_openbtn_clicked();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
