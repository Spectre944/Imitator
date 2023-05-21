#ifndef IMITATOR_H
#define IMITATOR_H

#include <QMainWindow>
#include <QWidget>
#include <QScreen>
#include <QVariantList>
#include <QTimer>
#include <random>
#include <QString>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QDir>
#include <QFileInfoList>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include "coord_work.h"


#define ACTION_FOLER "C:\\RCB\\Imitator\\"


QT_BEGIN_NAMESPACE
namespace Ui { class Imitator; }
QT_END_NAMESPACE

struct SVNG{
    double PEDL;
    int STL;

    double PEDR;
    int STR;

    double PEDF;
    int STF;

    double PEDB;
    int STB;
};

struct SRHBR{
    double PED;
    int ST;

    double HNO;
    double P;
    double As;
    double S;
    int CH;
    bool BIO;

};

struct Airmar{
    double WindSp;
    double WindDir;
    double Temp;
    double Hum;
    double Dew;
    double Compass;
    double Pressure;
};

struct ChemPro{
    QString GasName;
    int Conc;
};

struct Devices{
    SVNG    SV{0,3,0,3,0,3,0,3};
    SRHBR   SR{0,3,1,1,1,1,1,0};
    Airmar  AM{0,0,0,0,0,0,0};
    ChemPro CP{"",0};

};

class Imitator : public QMainWindow
{
    Q_OBJECT

public:
    Imitator(QWidget *parent = nullptr);
    ~Imitator();


private slots:

    bool imitationProcess();
    void addWayFromJson();
    std::vector<Point> parseJsonArea(QString);
    void generateDevData();
    Point getCurCoordinates();
    QVariantList fillArrayWithData();

    void getRealCoordinates();

    double randomNumnber(double leftRange, double rightRange)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<double> dis(leftRange, rightRange);
        return dis(gen);
    }

    int randomNumnber(int leftRange, int rightRange)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<double> dis(leftRange, rightRange);
        return dis(gen);
    }

    void on_pushButtonRefteshList_clicked();

    void on_pushButtonStartScenario_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonFillGPS_clicked();

    void on_textEditGPS_customContextMenuRequested(const QPoint &pos);

private:

    QString treadFromJSON;
    Devices *devs;
    Point *CurrCoordinates;

    Ui::Imitator *ui;
    Coord_work *coordwork;

signals:
    void transmitDataObjectVariant(QVariantList);
    void set_param(double, double, double);


};
#endif // IMITATOR_H
