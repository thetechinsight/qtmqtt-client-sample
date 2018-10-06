#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),_pub_client(nullptr)
{
    ui->setupUi(this);
    _pub_client = new QMqttClient(this);
    _pub_client->setHostname(ui->hostAddressLineEdit->text());
    _pub_client->setPort(ui->portSpinBox->value());

    connect(ui->portSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),_pub_client,&QMqttClient::setPort);

    QObject::connect(_pub_client, &QMqttClient::pingResponseReceived, this,&MainWindow::updateLog);

    QObject::connect(_pub_client, &QMqttClient::stateChanged,[=](){
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(": State Change")
                + QString::number(_pub_client->state())
                + QLatin1Char('\n');
        ui->log_TextBrowser->insertPlainText("pub state changed :"+content);

        if(_pub_client->state() == QMqttClient::Disconnected){
            ui->connectButton->setText("Connect");
        }else{
            ui->connectButton->setText("Disconnect");
        }
    });


    QObject::connect(_pub_client,&QMqttClient::errorChanged,[=](QMqttClient::ClientError error){
        qDebug()<<Q_FUNC_INFO<<error;
    });
    setWindowTitle("Mqtt publisher client");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateLog()
{
    const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String(" PingResponse")
            + QLatin1Char('\n');
    ui->log_TextBrowser->insertPlainText(content);
}


void MainWindow::on_publishButton_clicked()
{
    if(ui->messageLineEdit->text().isEmpty()|| ui->topic_Lineedit->text().isEmpty()){
        return;
    }
    if(_pub_client->state()== QMqttClient::Connected){
        qint32 bytes= _pub_client->publish(QMqttTopicName(ui->topic_Lineedit->text()),QByteArray(ui->messageLineEdit->text().toUtf8()));
        ui->log_TextBrowser->insertPlainText(QString("%1 bytes published\n").arg(bytes));
    }
}

void MainWindow::on_pingButton_clicked()
{
    _pub_client->requestPing();
}

void MainWindow::on_connectButton_clicked()
{
    if(_pub_client->state() == QMqttClient::Disconnected){
        _pub_client->connectToHost();
    }else{
        _pub_client->disconnectFromHost();
    }

}
