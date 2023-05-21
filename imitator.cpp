#include "imitator.h"
#include "ui_imitator.h"

Imitator::Imitator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Imitator)
{
    ui->setupUi(this);

    coordwork = new Coord_work();

    devs = new Devices();

    CurrCoordinates = new Point(0,0);

    //load all files(contaminated areas) in folder
    QDir directory(ACTION_FOLER); // Replace with the desired folder path

    QFileInfoList fileInfoList = directory.entryInfoList(QDir::Files);
    for (const QFileInfo& fileInfo : fileInfoList)
    {
        QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName());
        ui->listWidgetArchiveDanger->addItem(item);
    }


}

Imitator::~Imitator()
{
    delete ui;
}

bool Imitator::imitationProcess()
{

    Point GPS = this->getCurCoordinates();

    generateDevData();

    for (int i = 0; i < ui->listWidgetActiveDanger->count(); ++i)
    {
        QListWidgetItem* existingItem = ui->listWidgetActiveDanger->item(i);

        std::vector<Point> area = parseJsonArea(existingItem->text());

        if( coordwork->pointIntoArea(GPS, area) ){

            //DER Area contaminbation
            if(treadFromJSON.contains("DER")){

                QStringList values = treadFromJSON.split(" ");

                if(values.count() >= 2){
                    double DER = values.at(1).toDouble();

                    devs->SV.PEDF = DER;
                    devs->SV.PEDB = DER;
                    devs->SV.PEDL = DER;
                    devs->SV.PEDR = DER;

                    //коэфицент понижения гамма излучения бронёй
                    devs->SV.PEDF = DER*0.2;

                    qDebug() << "Radiation thread";
                }
            }

            //Chemical HNO Area contaminbation
            if(treadFromJSON.contains("HNO")){

                QStringList values = treadFromJSON.split(" ");

                if(values.count() >= 2){
                    double Conc = values.at(1).toDouble();
                    devs->SR.HNO = Conc;

                    qDebug() << "HNO thread";
                }
            }

            //Chemical Arsenic Area contaminbation
            if(treadFromJSON.contains("Arsenic")){

                QStringList values = treadFromJSON.split(" ");

                if(values.count() >= 2){
                    double Conc = values.at(1).toDouble();
                    devs->SR.As = Conc;
                }

                qDebug() << "Arsenic thread";
            }

            //Chemical Phosphorus Area contaminbation
            if(treadFromJSON.contains("Phosphorus")){

                QStringList values = treadFromJSON.split(" ");

                if(values.count() >= 2){
                    double Conc = values.at(1).toDouble();
                    devs->SR.P = Conc;
                }

                qDebug() << "Phosphorus thread";
            }

            //Chemical Sulfur Area contaminbation
            if(treadFromJSON.contains("Sulfur")){

                QStringList values = treadFromJSON.split(" ");

                if(values.count() >= 2){
                    double Conc = values.at(1).toDouble();
                    devs->SR.S = Conc;
                }

                qDebug() << "Sulfur thread";
            }

            //Chemical Sulfur Area contaminbation
            if(treadFromJSON.contains("Hydrocarbon")){

                QStringList values = treadFromJSON.split(" ");

                if(values.count() >= 2){
                    double Conc = values.at(1).toDouble();
                    devs->SR.CH = Conc;
                }

                qDebug() << "Hydrocarbon thread";
            }

            //Chemical BIO Area contaminbation
            if(treadFromJSON.contains("BIO")){

                devs->SR.BIO = true;

                qDebug() << "BIO thread";

            }

            //Chemical BIO Area contaminbation
            if(treadFromJSON.contains("ChemPro")){

                QStringList values = treadFromJSON.split(" ");

                if(values.count() >= 3){
                    QString Gas = values.at(1);
                    double Conc = values.at(2).toInt();

                    devs->CP.GasName = Gas;
                    devs->SR.CH = Conc;
                }

                qDebug() << "ChemPro thread";

            }

        }

    }

    QVariantList data = fillArrayWithData();

    emit set_param(CurrCoordinates->getX(), CurrCoordinates->getY(), ui->checkBoxMovement->isChecked());

    emit transmitDataObjectVariant(data);

    return true;
}


void Imitator::addWayFromJson()
{
    // Parse the JSON data
    QJsonDocument jsonDocument = QJsonDocument::fromJson(ui->textEditGPS->toPlainText().toUtf8());
    if (!jsonDocument.isNull() && jsonDocument.isObject()) {
        QJsonObject jsonObject = jsonDocument.object();

        // Extract coordinates
        if (jsonObject.contains("geometry") && jsonObject["geometry"].isObject()) {
            QJsonObject geometryObject = jsonObject["geometry"].toObject();
            if (geometryObject.contains("coordinates") && geometryObject["coordinates"].isArray()) {
                QJsonArray coordinatesArray = geometryObject["coordinates"].toArray();
                if (coordinatesArray.size() > 0 && coordinatesArray[0].isArray()) {

                    // Extracting the first coordinate pair
                    if (coordinatesArray.size() > 0) {
                        for(int i = 0; i < coordinatesArray.size(); i++){
                            QJsonArray coordinatePairArray = coordinatesArray[i].toArray();
                            if (coordinatePairArray.size() == 2) {
                                qreal longitude = coordinatePairArray[0].toDouble();
                                qreal latitude = coordinatePairArray[1].toDouble();

                                //reverse pair of coordinate here
                                ui->comboBoxGPSList->addItem(QString::number(latitude) + " " + QString::number(longitude));
                            }
                        }
                    }
                }
            }
        }

        // Extract name
        if (jsonObject.contains("properties") && jsonObject["properties"].isObject()) {
            QJsonObject propertiesObject = jsonObject["properties"].toObject();
            if (propertiesObject.contains("name")) {
                QString name = propertiesObject["name"].toString();
                qDebug() << "Name:" << name;
            }
        }
    }



}

std::vector<Point> Imitator::parseJsonArea(QString fileName)
{
    std::vector<Point> areaVector;
    areaVector.clear();
    treadFromJSON.clear();

    QFile JSONFile(ACTION_FOLER + fileName);
    if (!JSONFile.open(QIODevice::ReadOnly | QIODevice::Text))
             return std::vector<Point>(0);  //return empty vector

    //JSONFile.open(QIODevice::ReadOnly);
    QByteArray jsonContent = JSONFile.readAll();
    JSONFile.close();

    // Parse the JSON data
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonContent);
    if (!jsonDocument.isNull() && jsonDocument.isObject()) {
        QJsonObject jsonObject = jsonDocument.object();

        // Extract coordinates
        if (jsonObject.contains("geometry") && jsonObject["geometry"].isObject()) {
            QJsonObject geometryObject = jsonObject["geometry"].toObject();
            if (geometryObject.contains("coordinates") && geometryObject["coordinates"].isArray()) {
                QJsonArray coordinatesArray = geometryObject["coordinates"].toArray();
                if (coordinatesArray.size() > 0 && coordinatesArray[0].isArray()) {
                    QJsonArray firstPolygonArray = coordinatesArray[0].toArray();

                    // Extracting the first coordinate pair
                    if (firstPolygonArray.size() > 0) {
                        for(int i = 0; i < firstPolygonArray.size(); i++){
                            QJsonArray coordinatePairArray = firstPolygonArray[i].toArray();
                            if (coordinatePairArray.size() == 2) {
                                qreal longitude = coordinatePairArray[0].toDouble();
                                qreal latitude = coordinatePairArray[1].toDouble();
                                //reverse pair of coordinate here
                                areaVector.push_back(Point(latitude, longitude));
                            }
                        }
                    }
                }
            }
        }

        // Extract name
        if (jsonObject.contains("properties") && jsonObject["properties"].isObject()) {
            QJsonObject propertiesObject = jsonObject["properties"].toObject();
            if (propertiesObject.contains("name")) {
                QString name = propertiesObject["name"].toString();
                //write type of threat (writing here because i cant return QString in this function)
                treadFromJSON = name;
                //qDebug() << "Name:" << name;
            }
        }
    }

    return areaVector;  //return empty vector
}

void Imitator::generateDevData()
{
    double der1, der2;

    der1 = ui->doubleSpinBoxSVPedL->value();
    der2 = ui->doubleSpinBoxSVPedR->value();

    devs->SV.PEDF = randomNumnber(der1, der2);
    devs->SV.PEDB = randomNumnber(der1, der2);
    devs->SV.PEDL = randomNumnber(der1, der2);
    devs->SV.PEDR = randomNumnber(der1, der2);

    devs->SV.STF = 3;
    devs->SV.STB = 3;
    devs->SV.STL = 3;
    devs->SV.STR = 3;

    der1 = ui->doubleSpinBoxSRPedL->value();
    der2 = ui->doubleSpinBoxSRPedR->value();

    devs->SR.PED = randomNumnber(der1, der2);
    devs->SR.ST = 3;

    devs->SR.HNO    = randomNumnber(ui->doubleSpinBoxHNOL->value(), ui->doubleSpinBoxHNOR->value());
    devs->SR.P      = randomNumnber(ui->doubleSpinBoxPL->value(), ui->doubleSpinBoxPR->value());
    devs->SR.As     = randomNumnber(ui->doubleSpinBoxAsL->value(), ui->doubleSpinBoxAsR->value());
    devs->SR.S      = randomNumnber(ui->doubleSpinBoxSL->value(), ui->doubleSpinBoxSR->value());
    devs->SR.BIO    = ui->checkBoxBIO->isChecked();

    devs->AM.WindSp     = randomNumnber(ui->doubleSpinBoxAMSpL->value(), ui->doubleSpinBoxAMSpR->value());
    devs->AM.WindDir    = randomNumnber(ui->spinBoxAMDirL->value(), ui->spinBoxAMDirR->value());
    devs->AM.Temp       = randomNumnber(ui->doubleSpinBoxAMTempL->value(), ui->doubleSpinBoxAMTempR->value());
    devs->AM.Hum        = randomNumnber(ui->spinBoxAMHumL->value(), ui->spinBoxAMHumR->value());
    devs->AM.Dew        = randomNumnber(ui->doubleSpinBoxAMDewL->value(), ui->doubleSpinBoxAMDewR->value());
    devs->AM.Pressure   = randomNumnber(ui->spinBoxAMPressL->value(), ui->spinBoxAMPressR->value());
    devs->AM.Compass    = randomNumnber(ui->spinBoxAMCompL->value(), ui->spinBoxAMCompR->value());

    devs->CP.GasName = ui->comboBoxCPEl->currentText();
    devs->CP.Conc = ui->comboBoxCPConc->currentIndex();

}

Point Imitator::getCurCoordinates()
{
    return *CurrCoordinates;
}

QVariantList Imitator::fillArrayWithData()
{

    QVariantList list;
    list.clear();
    int i = 0;

    list << ui->checkBoxMovement->isChecked();

    list << ui->comboBoxGPSList->currentText();

    list << true;   // флаг изменения данных (false - не менялись , true - менялись)


///////////////////////////////////// СВНГ-Т НАЧАЛО /////////////////////////////////////////////////////////

    list << 0;                     // Угол направления к источнику радиации

    list << devs->SV.PEDB;   // 1 - пер, 0 - зад, 2 - лев,  3 - пр
    list << devs->SV.PEDF;
    list << devs->SV.PEDL;
    list << devs->SV.PEDR;    // Массив ПАЕД датчиков СВНГ-Т ( 0 - 3 )

    double maxPed = devs->SV.PEDF;

    maxPed < devs->SV.PEDB ? maxPed = devs->SV.PEDB : 0;
    maxPed < devs->SV.PEDL ? maxPed = devs->SV.PEDL : 0;
    maxPed < devs->SV.PEDR ? maxPed = devs->SV.PEDR : 0;

    list << maxPed;                                                     // Максимальное значение ПАЕД

    list << 3;
    list << 3;
    list << 3;
    list << 3;                          // Массив стат. оценок измерений датчиков


    list << 1 ;
    list << 1 ;
    list << 1 ;
    list << 1 ;                         // Массив валидности данных датчиков (0 - валидный, 1 - невалидный)

    for(i = 0; i < 4; ++i)
        list << ui->checkBoxSVHSF->isChecked();               // Массив флагов неисправностей высокочувствительных датчиков СВНГ-Т ( 0 - 3 )

    for(i = 0; i < 4; ++i)
         list << ui->checkBoxSVLSF->isChecked();              // Массив флагов неисправностей низкочувствительных датчиков СВНГ-Т ( 0 - 3 )

    for(i = 0; i < 4; ++i)
         list << ui->checkBoxSVLSF->isChecked();              // Массив  флагов неисправностей датчиков (для GUI) // ??? не понял что значит

    for(i = 0; i < 4; ++i)
        ui->comboBoxSVEL->currentIndex() == 1 ? list << 1 : list << 0;          //Массив потери связи с детекторами

    maxPed > ui->doubleSpinBoxSVTr->value() ? list << false : list << true;                          // false - рисуем,  true - не рисуем

    ui->comboBoxSVEL->currentIndex() == 2 ? list << 1 : list << 0;   // Флаг ошибки открытия СОМ-порта  СВНГ-Т

    ui->comboBoxSVEL->currentIndex() == 3 ? list << 1 : list << 0;   // Флаг ошибки записи в СОМ-порт   СВНГ-Т

    ui->comboBoxSVEL->currentIndex() == 4 ? list << 1 : list << 0;   // Флаг ошибки CRC принятого пакета    СВНГ-Т

    ui->comboBoxSVEL->currentIndex() == 5 ? list << 1 : list << 0;   // Флаг ошибки размера принятого пакета    СВНГ-Т

    ui->comboBoxSVEL->currentIndex() == 6 ? list << 1 : list << 0;   // Флаг неответа прибора СВНГ-Т

    ui->comboBoxSVEL->currentIndex() != 0 ? list << 1 : list << 0;   // Общий флаг неисправности СВНГ-Т




///////////////////////////////////// AIR_MAR НАЧАЛО /////////////////////////////////////////////////////////


    list << devs->AM.WindSp;        // Скорость ветра

    list << devs->AM.WindDir;       // Направление ветра (относительное)

    list << devs->AM.Temp;          // Температура воздуха

    list << devs->AM.Hum;           // Влажность воздуха

    list << devs->AM.Pressure;      // Давление воздуха

    list << devs->AM.Dew;           // Точка росы

    list << devs->AM.Compass;       // Компас (направление на магнитный полюс)

    list << devs->AM.WindDir;       // Направление ветра (откорректированное)

    ui->comboBoxAMEL->currentIndex() == 1 ? list << 1 : list << 0;      // Флаг ошибки открытия СОМ-порта  AirMar

    ui->comboBoxAMEL->currentIndex() == 2 ? list << 1 : list << 0;      // Флаг неответа прибора    AirMar

    ui->comboBoxAMEL->currentIndex() == 3 ? list << 1 : list << 0;      // Флаг ошибки CRC пакета WIMWV

    ui->comboBoxAMEL->currentIndex() == 4 ? list << 1 : list << 0;      // Флаг ошибки CRC пакета WIMDA

    ui->comboBoxAMEL->currentIndex() == 5 ? list << 1 : list << 0;      // Флаг ошибки CRC пакета GPVTG

    ui->comboBoxAMEL->currentIndex() != 0 ? list << 1 : list << 0;      // Общий флаг неисправности AirMar


///////////////////////////////////// CHEM_PRO НАЧАЛО /////////////////////////////////////////////////////////



    list << ui->checkBoxCPDE->isChecked();          // Флаг отсутствия данных в пакете

    list << ui->checkBoxCPDI->isChecked();          // Флаг валидности данных

    list << ui->spinBoxCPEC->value();               // Причины невалидности

    list << devs->CP.Conc;                          // Уровень концентрации

    list << devs->CP.GasName;                       // Имя газа в библиотеке

    ui->comboBoxCPEL->currentIndex() == 1 ? list << 1 : list << 0;       // Флаг зависания задания ChemPro

    ui->comboBoxCPEL->currentIndex() == 2 ? list << 1 : list << 0;       // Флаг зависания АЦП ChemPro

    ui->comboBoxCPEL->currentIndex() == 3 ? list << 1 : list << 0;       // Флаг аппаратной ошибки ChemPro (Ст. байт - 1(есть ош), Мл. байт - тип ошибки)

    ui->comboBoxCPEL->currentIndex() == 4 ? list << 1 : list << 0;       // Флаг ошибки открытия СОМ-порта  ChemPro

    ui->comboBoxCPEL->currentIndex() == 5 ? list << 1 : list << 0;       // Флаг ошибки записи в СОМ-порт   ChemPro

    ui->comboBoxCPEL->currentIndex() == 6 ? list << 1 : list << 0;       // Флаг ошибки CRC принятого пакета ChemPro

    ui->comboBoxCPEL->currentIndex() == 7 ? list << 1 : list << 0;       // Флаг неответа прибора ChemPro

    ui->comboBoxCPEL->currentIndex() != 0 ? list << 1 : list << 0;       // Общий флаг неисправности ChemPro

///////////////////////////////////// CРХБР НАЧАЛО /////////////////////////////////////////////////////////


    list << devs->SR.PED;             //  ПАЕД датчика СРХБР

    list << ui->checkBoxSRHSF->isChecked();       //  Флаг неисправности высокочувствительного датчика СРХБР

    list << ui->checkBoxSRLSF->isChecked();       // Флаг неисправности низкочувствительного датчика СРХБР

    list << 3;                                  // Значение валидности данных радиации СРХБР

    list << devs->SR.CH;                  // Значение концентрации CH

    devs->SR.CH > ui->spinBoxCHTr->value() ? list << 1 : list << 0;      // Флаг срабатывания сигнализации CH

    list << devs->SR.As;                          // Значение концентрации Ars

    devs->SR.As > ui->doubleSpinBoxAsTr->value() ? list << 1 : list << 0;      // Флаг срабатывания сигнализации Ars

    list << devs->SR.HNO;                          // Значение концентрации HNO

    devs->SR.HNO > ui->doubleSpinBoxHNOTr->value() ? list << 1 : list << 0;     // Флаг срабатывания сигнализации HNO

    list << devs->SR.P;                            // Значение концентрации Phs

    devs->SR.P > ui->doubleSpinBoxPTr->value() ? list << 1 : list << 0;       // Флаг срабатывания сигнализации P

    list << devs->SR.S;                         // Значение концентрации S

    devs->SR.S > ui->doubleSpinBoxSTr->value() ? list << 1 : list << 0;       // Флаг срабатывания сигнализации S

    devs->SR.BIO == true ? list << 1 : list << 0;     // Флаг срабатывания сигнализации  BIO

    devs->SR.PED > ui->doubleSpinBoxSRATOM->value() ? list << 1 : list << 0;  // Флаг срабатывания сигнализации  ATOM

    list << ui->checkBoxSR9B->isChecked();         // 9 байт пакета ответа СРХБР

    list << 0;                                              // Не нужно (всунуть 0х00)

    list << ui->checkBoxSR2B->isChecked();          // 2 бит 10 байта  пакета ответа СРХБР

    list << QString();                                      // Инфо строка прибора ( не нужно, засунуть пустой QString())

    list << ui->checkBoxSRNM->isChecked();   // Флаг необходимости обслуживания СРХБР

    list << 10;                                             // Минимальный уровень радиации СРХБР  ???

    list << ui->checkBoxSRHT->isChecked();         // Флаг нагрева прибора (неготовность)

    list << ui->checkBoxSRLH->isChecked();           // Флаг нехватки H2

    list << ui->checkBoxSRER->isChecked();        // Флаг ошибки химии

    ui->comboBoxSREL->currentIndex() == 1 ? list << 1 : list << 0;       // Флаг ошибки открытия СОМ-порта  СРХБР

    ui->comboBoxSREL->currentIndex() == 2 ? list << 1 : list << 0;       // Флаг ошибки записи в СОМ-порт   СРХБР

    ui->comboBoxSREL->currentIndex() == 3 ? list << 1 : list << 0;       // Флаг ошибки CRC принятого пакета СРХБР

    ui->comboBoxSREL->currentIndex() == 4 ? list << 1 : list << 0;       // Флаг неответа прибора СРХБР

    ui->comboBoxSREL->currentIndex() == 5 ? list << 1 : list << 0;       // Не отвечает "химик" СРХБР

    ui->comboBoxSREL->currentIndex() != 0 ? list << 1 : list << 0;       // Общий флаг неисправности СРХБР


    return list;
}

void Imitator::getRealCoordinates()
{

}

void Imitator::on_pushButtonRefteshList_clicked()
{
    ui->listWidgetArchiveDanger->clear();

    //load all files(contaminated areas) in folder
    QDir directory(ACTION_FOLER); // Replace with the desired folder path

    QFileInfoList fileInfoList = directory.entryInfoList(QDir::Files);
    for (const QFileInfo& fileInfo : fileInfoList)
    {
        QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName());
        ui->listWidgetArchiveDanger->addItem(item);
    }
}

void Imitator::on_pushButtonStartScenario_clicked()
{
    imitationProcess();
}

void Imitator::on_pushButtonSend_clicked()
{

    ui->comboBoxGPSList->setCurrentIndex(ui->spinBoxStep->value());

    QStringList coordPair = ui->comboBoxGPSList->currentText().split(" ");

    if(coordPair.count() >= 2){
        CurrCoordinates->setXY(coordPair.at(0).toDouble(), coordPair.at(1).toDouble());
    }

    imitationProcess();

    ui->spinBoxStep->stepUp();
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


void Imitator::on_textEditGPS_customContextMenuRequested(const QPoint &pos)
{
    // Start with the standard menu.
    QMenu * pMenu = ui->textEditGPS->createStandardContextMenu();
    QAction * addWatFromJson;

    // Clear.
    // Because QPlainTextEdit::clear() is a slot method, I can connect directly to it.
    addWatFromJson = new QAction( "Заповнити шлях з JSON", this );
    connect(addWatFromJson, &QAction::triggered,
            this, &Imitator::addWayFromJson);
    pMenu->addAction( addWatFromJson );


    // Show the menu.
    QPoint q = ui->textEditGPS->mapToGlobal( pos );
    pMenu->exec( q );

    // The menu's ownership is transferred to me, so I must destroy it.
    delete pMenu;
}



/*
bool Imitator::eventFilter(QObject *watched, QEvent *event)
{
     if (event->type() == QEvent::DragEnter)
     {
         QDragEnterEvent* dragEnterEvent = static_cast<QDragEnterEvent*>(event);
         if (dragEnterEvent->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
         {
             dragEnterEvent->acceptProposedAction();
             return true;
         }
     }
     else if (event->type() == QEvent::Drop)
     {
         QDropEvent* dropEvent = static_cast<QDropEvent*>(event);
         const QMimeData* mimeData = dropEvent->mimeData();

         if (mimeData->hasFormat("application/x-qabstractitemmodeldatalist"))
         {
             QByteArray encodedData = mimeData->data("application/x-qabstractitemmodeldatalist");
             QDataStream stream(&encodedData, QIODevice::ReadOnly);

             while (!stream.atEnd())
             {
                 int row, col;
                 QMap<int, QVariant> roleDataMap;
                 stream >> row >> col >> roleDataMap;

                 if (watched == ui->listWidgetActiveDanger && roleDataMap.contains(Qt::DisplayRole))
                 {
                     QString itemName = roleDataMap[Qt::DisplayRole].toString();

                     // Check if the item already exists in the destination list
                     bool itemExists = false;
                     for (int i = 0; i < ui->listWidgetActiveDanger->count(); ++i)
                     {
                         QListWidgetItem* existingItem = ui->listWidgetActiveDanger->item(i);
                         if (existingItem->text() == itemName)
                         {
                             itemExists = true;
                             break;
                         }
                     }

                     // Add the item if it doesn't already exist
                     if (!itemExists)
                     {
                         QListWidgetItem* item = new QListWidgetItem(itemName);
                         ui->listWidgetActiveDanger->addItem(item);
                     }
                 }
             }

             dropEvent->acceptProposedAction();
             return true;
     }

     return QObject::eventFilter(watched, event);

    }


}
*/














