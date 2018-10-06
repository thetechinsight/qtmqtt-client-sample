#include "mainwindow.h"
#include <QApplication>
#include <QtMqtt/QMqttClient>
#include <QDebug>
#include <QDateTime>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    MainWindow w;
    //    w.show();

    QMqttClient* client = new QMqttClient;
    client->setHostname("127.0.0.1");
    client->setPort(1883);

    QObject::connect(client,&QMqttClient::connected,[=](){
        qDebug()<<Q_FUNC_INFO << "connected with server";
    });
    QObject::connect(client,&QMqttClient::disconnected,[=](){
        qDebug()<<Q_FUNC_INFO << "disconnected with server";
    });

    QMqttSubscription* subscription = client->subscribe(QMqttTopicFilter("topic/#"));
    if(subscription)
        qDebug()<<Q_FUNC_INFO<<subscription->qos()<<subscription->topic().isValid();

    QObject::connect(client,&QMqttClient::errorChanged,[=](QMqttClient::ClientError error){
        qDebug()<<Q_FUNC_INFO<<error;
    });


    QObject::connect(client, &QMqttClient::stateChanged,[=](){
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(": State Change")
                + QString::number(client->state())
                + QLatin1Char('\n');
        qDebug()<<Q_FUNC_INFO<<"Sub state changed :"<<content;
    });

    QObject::connect(client, &QMqttClient::messageReceived,[=](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message
                + QLatin1Char('\n');
        qDebug()<<Q_FUNC_INFO<<"client sub received: "<<content;
    });
    QObject::connect(client, &QMqttClient::pingResponseReceived, []() {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" PingResponse")
                + QLatin1Char('\n');
        qDebug()<<Q_FUNC_INFO<<"ping sub received: "<<content;
    });
    client->connectToHost();

    return a.exec();
}
