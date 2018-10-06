#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMqtt/QMqttClient>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateLog();
    void on_publishButton_clicked();

    void on_pingButton_clicked();

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;
    QMqttClient* _pub_client;
};

#endif // MAINWINDOW_H
