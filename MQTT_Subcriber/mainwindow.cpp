#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),_sub_client(nullptr)
{
    ui->setupUi(this);
    _sub_client = new QMqttClient;

    ui->pingButton->setEnabled(false);
    ui->subcribeButton->setEnabled(false);
    connect(ui->portSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),_sub_client,&QMqttClient::setPort);
    connect(_sub_client,&QMqttClient::stateChanged,[=](QMqttClient::ClientState state){
        if(state != QMqttClient::Connected){
            ui->pingButton->setEnabled(false);
            ui->subcribeButton->setEnabled(false);
            ui->connectButton->setText("Connect");
            if(state == QMqttClient::Connecting){
                ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Connecting with Mqtt broker.\n");
            }else{
                ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Disconnected with Mqtt broker.\n");
            }
        }else{
            if(state == QMqttClient::Connected){
                ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Connected with Mqtt broker.\n");
            }

            ui->pingButton->setEnabled(true);
            ui->subcribeButton->setEnabled(true);
            ui->connectButton->setText("Disconnect");

            ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Now you are able to subscribe topic.\n");
        }

    });
    connect(_sub_client,&QMqttClient::errorChanged,this,[=](QMqttClient::ClientError error){
        ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Error occured: "+QString::number(error)+"\n");
    });
    connect(_sub_client,&QMqttClient::pingResponseReceived,[=](){
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" PingResponse")
                + QLatin1Char('\n');
        ui->log_TextBrowser->insertPlainText(content);
    });

    QObject::connect(_sub_client, &QMqttClient::messageReceived,[=](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message
                + QLatin1Char('\n');
        ui->log_TextBrowser->insertPlainText(content);
        ui->textBrowser->insertPlainText(message.constData());
        ui->textBrowser->insertPlainText("\n");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    if(_sub_client->state() != QMqttClient::Connected){
        QString host= ui->hostAddressLineEdit->text();

        if(host.isEmpty()){
            ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Please provide a valid host address\n");
            return;
        }
        _sub_client->setHostname(host);
        _sub_client->setPort(ui->portSpinBox->value());
        _sub_client->setUsername(ui->usernameEdit->text());
        _sub_client->setPassword(ui->passwordEdit->text());
        _sub_client->connectToHost();

    }else{
        _sub_client->unsubscribe(QMqttTopicFilter(ui->topic_Lineedit->text()));
        _sub_client->disconnectFromHost();
    }
}

void MainWindow::on_pingButton_clicked()
{
    _sub_client->requestPing();
    ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Pign Requested.\n");
}

void MainWindow::on_subcribeButton_clicked()
{
    if(ui->topic_Lineedit->text().isEmpty()){
        ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": Please provide a valid topic for subscribe.\n");
        return;
    }
    _sub_client->subscribe(QMqttTopicFilter(ui->topic_Lineedit->text()));
    ui->log_TextBrowser->insertPlainText(QDateTime::currentDateTime().toString()+": "+QString("Subscribe to %1.\n").arg(ui->topic_Lineedit->text()));
}
