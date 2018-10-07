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

    ui->publishButton->setEnabled(false);
    ui->pingButton->setEnabled(false);
    connect(ui->portSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),_pub_client,&QMqttClient::setPort);

    QObject::connect(_pub_client, &QMqttClient::pingResponseReceived, this,&MainWindow::updateLog);

    QObject::connect(_pub_client, &QMqttClient::stateChanged,[=](QMqttClient::ClientState state){

        if(_pub_client->state() != QMqttClient::Connected){
            ui->connectButton->setText("Connect");
            ui->pingButton->setEnabled(false);
            ui->publishButton->setEnabled(false);
            if(state == QMqttClient::Connecting){
                ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Connecting with Mqtt broker.\n");
            }else{
                ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Disconnected with Mqtt broker.\n");
            }
        }else{
            if(state == QMqttClient::Connected){
                ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Connected with Mqtt broker.\n");
            }

            ui->connectButton->setText("Disconnect");
            ui->pingButton->setEnabled(true);
            ui->publishButton->setEnabled(true);
            ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Now you are able to publish message.\n");
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
        _pub_client->publish(QMqttTopicName(ui->topic_Lineedit->text()),QByteArray(ui->messageLineEdit->text().toUtf8()));
        ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+" "+QString("message '%1' published on the topic '%2'\n").arg(ui->messageLineEdit->text()).arg(ui->topic_Lineedit->text()));
    }
}

void MainWindow::on_pingButton_clicked()
{
    _pub_client->requestPing();
    ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Pign Requested.\n");
}

void MainWindow::on_connectButton_clicked()
{
    if(_pub_client->state() == QMqttClient::Disconnected){

        QString host= ui->hostAddressLineEdit->text();

        if(host.isEmpty()){
            ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Please provide a valid host address\n");
            return;
        }
        _pub_client->setHostname(host);
        _pub_client->setPort(ui->portSpinBox->value());
        _pub_client->setUsername(ui->usernameEdit->text());
        _pub_client->setPassword(ui->passwordEdit->text());
        _pub_client->connectToHost();
    }else{
        _pub_client->unsubscribe(QMqttTopicFilter(ui->topic_Lineedit->text()));
        _pub_client->disconnectFromHost();
    }

}
