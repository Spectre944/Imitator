#ifndef IMITATOR_H
#define IMITATOR_H

#include <QMainWindow>
#include <QWidget>
#include <QScreen>
#include <QVariantList>
#include <QTimer>
#include <random>
#include <QString>

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
    void on_pushButtonFillGPS_clicked();

private:
    Ui::Imitator *ui;


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

};
#endif // IMITATOR_H
