#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMqttClient>
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
    void on_connectButton_clicked();

    void on_pingButton_clicked();

    void on_subcribeButton_clicked();

private:
    Ui::MainWindow *ui;
    QMqttClient *_sub_client;
};

#endif // MAINWINDOW_H
