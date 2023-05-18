#include "imitator.h"
#include "ui_imitator.h"

Imitator::Imitator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Imitator)
{
    ui->setupUi(this);
}

Imitator::~Imitator()
{
    delete ui;
}


void Imitator::on_pushButtonFillGPS_clicked()
{

    if(ui->textEditGPS->toPlainText() == "CLR")
    {
        ui->comboBoxGPSList->clear();
        return;
    }

    //if list are empty, generate random coordinates with boundies
    if(ui->textEditGPS->toPlainText() == "")
    {
        ui->comboBoxGPSList->addItem(QString(QString::number(randomNumnber(ui->doubleSpinBoxLatL->value(), ui->doubleSpinBoxLatR->value())) + " " +
                                             QString::number(randomNumnber(ui->doubleSpinBoxLonL->value(), ui->doubleSpinBoxLonR->value())) ));
        return;
    }
    else{

        //clear GPS list if it fill from text or JSON
        ui->comboBoxGPSList->clear();

        QStringList coordList = ui->textEditGPS->toPlainText().split("/\n");
        coordList.replaceInStrings(",", "");

        for(int i = 0; i < coordList.count(); i++ ){
            ui->comboBoxGPSList->addItem(coordList.at(i));
        }

    }

}

