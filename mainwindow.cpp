#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QQmlEngine>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QQuickView>
#include <QQmlApplicationEngine>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    totalFiles(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Thermal Finder");
    setGeometry(0, 0, 800, 600);

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();

    move(static_cast<int>(center.x() - width() * 0.5), static_cast<int>(center.y() - height() * 0.5));

    mTrackManager = TrackManager::getInstance();

    QPushButton *pushButton_LoadIgc = new QPushButton("Load Igc File");
    connect(pushButton_LoadIgc, &QPushButton::clicked, this, &MainWindow::on_pushButton_LoadIgc_clicked);
    pushButton_LoadIgc->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 153, 175);");
    pushButton_LoadIgc->setFixedWidth(150);

    QPushButton *pushButton_LoadIgcPath = new QPushButton("Load Igc Path");
    connect(pushButton_LoadIgcPath, &QPushButton::clicked, this, &MainWindow::on_pushButton_LoadIgcPath_clicked);
    pushButton_LoadIgcPath->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 153, 175);");
    pushButton_LoadIgcPath->setFixedWidth(150);

    QPushButton *pushButton_CreateWp = new QPushButton("Create WPT File");
    connect(pushButton_CreateWp, &QPushButton::clicked, this, &MainWindow::on_pushButton_CreateWpt_clicked);
    pushButton_CreateWp->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 153, 175);");
    pushButton_CreateWp->setFixedWidth(150);

    QPushButton *pushButton_LoadWp = new QPushButton("Load WPT File");
    connect(pushButton_LoadWp, &QPushButton::clicked, this, &MainWindow::on_pushButton_LoadWpt_clicked);
    pushButton_LoadWp->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 153, 175);");
    pushButton_LoadWp->setFixedWidth(150);

    QPushButton *pushButton_Clear = new QPushButton("Clear All");
    connect(pushButton_Clear, &QPushButton::clicked, this, &MainWindow::on_pushButton_Clear_clicked);
    pushButton_Clear->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 153, 175);");
    pushButton_Clear->setFixedWidth(150);

    QPushButton *pushButton_Exit = new QPushButton("Exit");
    connect(pushButton_Exit, &QPushButton::clicked, this, &MainWindow::on_pushButton_Exit_clicked);
    pushButton_Exit->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 153, 175);");
    pushButton_Exit->setFixedWidth(150);

    QLabel *label_TCount = new QLabel(" Current Track\t");
    label_TCount->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 102, 175);");
    label_TCount->setFixedWidth(150);

    label_TrackCount = new QLabel("0");
    label_TrackCount->setStyleSheet("font-size: 12pt; color: black; background-color: white;");
    label_TrackCount->setFixedWidth(150);

    QLabel *label_WCount = new QLabel(" Total WPT\t");
    label_WCount->setStyleSheet("font-size: 12pt; color: white;background-color: rgba(0, 102, 102, 175);");
    label_WCount->setFixedWidth(150);

    label_WayPointCount = new QLabel("0");
    label_WayPointCount->setStyleSheet("font-size: 12pt; color: black; background-color: white;");
    label_WayPointCount->setFixedWidth(150);

    QLabel *label_VMin = new QLabel(" Vario Min (m/s)\t");
    label_VMin->setStyleSheet("font-size: 10pt; color: white;background-color: rgba(0, 102, 102, 175);");
    label_VMin->setFixedWidth(110);

    lineEdit_VarioMin = new QLineEdit("3");
    lineEdit_VarioMin->setStyleSheet("font-size: 10pt; color: black; background-color: white;");
    lineEdit_VarioMin->setFixedWidth(32);
    lineEdit_VarioMin->setValidator( new QDoubleValidator(0, 100, 2, this));   

    QLabel *label_VFactor = new QLabel(" Vario Factor\t");
    label_VFactor->setStyleSheet("font-size: 10pt; color: white;background-color: rgba(0, 102, 102, 175);");
    label_VFactor->setFixedWidth(110);

    lineEdit_VarioFactor = new QLineEdit("30");
    lineEdit_VarioFactor->setStyleSheet("font-size: 10pt; color: black; background-color: white;");
    lineEdit_VarioFactor->setFixedWidth(32);
    lineEdit_VarioFactor->setValidator( new QDoubleValidator(0, 100, 2, this));

    textBrowser = new QTextBrowser();
    textBrowser->setStyleSheet("font: 10pt; color: #00cccc; background-color: #001a1a;");
    textBrowser->setFixedWidth(150);

    mapView = new QQuickView();
    mapView->setSource(QUrl(QStringLiteral("qrc:/qml/MapView.qml")));
    mapView->rootContext()->setContextProperty("pathController", &pathController);

    rootObject = mapView->rootObject();

    QHBoxLayout *hlayEditVMin = new QHBoxLayout();
    hlayEditVMin->addWidget(label_VMin);
    hlayEditVMin->addWidget(lineEdit_VarioMin);

    QHBoxLayout *hlayEditVFactor = new QHBoxLayout();
    hlayEditVFactor->addWidget(label_VFactor);
    hlayEditVFactor->addWidget(lineEdit_VarioFactor);

    QVBoxLayout *vLeft = new QVBoxLayout();
    QVBoxLayout *vRight = new QVBoxLayout();

    QWidget *qmlWidgetMap = QWidget::createWindowContainer(mapView, this);
    vLeft ->addWidget(qmlWidgetMap);

    QGridLayout *mainLayout  = new QGridLayout(this);

    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);
    label_TrackCount->setMargin(5);
    label_WayPointCount->setMargin(5);

    vRight->addWidget(pushButton_LoadIgc);
    vRight->addWidget(pushButton_LoadIgcPath);
    vRight->addWidget(pushButton_CreateWp);
    vRight->addWidget(pushButton_LoadWp);
    vRight->addWidget(pushButton_Clear);
    vRight->addWidget(label_TCount);
    vRight->addWidget(label_TrackCount);
    vRight->addWidget(label_WCount);
    vRight->addWidget(label_WayPointCount);
    vRight->addLayout(hlayEditVMin);
    vRight->addLayout(hlayEditVFactor);
    vRight->addWidget(textBrowser);
    vRight->addWidget(pushButton_Exit);

    mainLayout->addLayout(vLeft, 0 , 0);
    mainLayout->addLayout(vRight, 0 , 1);

    QWidget* central_widget = new QWidget(this);
    central_widget->setLayout(mainLayout);
    setCentralWidget(central_widget);
    setMapTilt(0);
    setMapRotation(0);
    setMapZoom(12);

    startGpsSource();

    CopyResources();

    textBrowser->setText("Please Load Igc File...");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CopyResources()
{ 
    QString dataLocation = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0);
    if (QDir(dataLocation).exists()) {
        qDebug() << "App data directory exists. " << dataLocation;
    }
    else
    {
        if (QDir("").mkpath(dataLocation))
        {
            qDebug() << "Created app data directory. " << dataLocation;
        }
        else
        {
            qDebug() << "Failed to create app data directory. " << dataLocation;
        }
    }

    QDirIterator it(":/igc", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString resourceFile =  it.next();
        copyResource(resourceFile, dataLocation);
    }
}

void MainWindow::copyResource(QString & resourceFile, const QString & dataLocation)
{

    QDataStream in, out;
    QFile wptFile(resourceFile);
    if (!wptFile.open(QIODevice::ReadOnly))
        return;

    QString resultFile(dataLocation + resourceFile.remove(":/igc"));

    in.setDevice(&wptFile);

    QFile result(resultFile);
    if (!result.open(QIODevice::WriteOnly))
        return;

    out.setDevice(&result);

    char *data = new char[wptFile.size()];

    in.readRawData(data, wptFile.size());
    out.writeRawData(data, wptFile.size());

    result.close();
    wptFile.close();

    delete [] data;
}

void MainWindow::changeEvent( QEvent* e )
{
    if( e->type() == QEvent::WindowStateChange )
    {

    }

    QWidget::changeEvent(e);
}


void MainWindow::addIgcFile(const QString &fileName)
{
    if(mTrackManager)
    {
        if(mTrackManager->isExistFile(fileName))
        {
            QMessageBox::warning(
                        this,
                        tr("Thermal Finder"),
                        tr("The Igc file already has been loaded."));

            return;
        }

        auto vMin = static_cast<qreal>(lineEdit_VarioMin->text().toDouble());
        auto vFactor = static_cast<int>(lineEdit_VarioFactor->text().toInt());

        mTrackManager->setVarioMin(vMin);
        mTrackManager->setVarioFactor(vFactor);
        mTrackManager->clearTrackPoints();

        if(mTrackManager->addIgcFile(fileName))
        {
            mTrackManager->addFileToList(fileName);
            mTrackManager->calculateWayPoints();
            addTrack(mTrackManager->getPath());
            refreshInfo();
            drawWayPoints(true);
            totalFiles++;
        }
        else
        {
            label_TrackCount->setText("0 / 0");
            label_WayPointCount->setText("0");
            textBrowser->clear();
            textBrowser->append("There is no track.");
        }
    }
}

void MainWindow::addWpFile(const QString &fileName)
{
    if(mTrackManager)
    {
        if(mTrackManager->isExistFile(fileName))
        {
            QMessageBox::warning(
                        this,
                        tr("Thermal Finder"),
                        tr("The Igc file already has been loaded."));

            return;
        }

        if(mTrackManager->addWayPointFile(fileName))
        {
            mTrackManager->addFileToList(fileName);
            refreshInfo();
            drawWayPoints(false);
        }
        else
        {
            QMessageBox::critical(
                        this,
                        tr("Thermal Finder"),
                        tr("Error: Waypoint file not loaded.") );
        }
    }
}

void MainWindow::drawWayPoints(bool showVario)
{  
    auto mWayPointList = mTrackManager->getWayPointList();

    if(mWayPointList.size() == 0)
        return;

    auto midPoint = mTrackManager->GetMidPoint(mWayPointList);

    setMapCenter(midPoint.latitude(), midPoint.longitude());

    for (auto wPoint : mWayPointList)
    {
        auto coord = wPoint.coord;

        QString text;

        if(showVario)
            text = QString::number(wPoint.vario);
        else
            text = wPoint.name;

        addWayPointToMap(coord.latitude(), coord.longitude(), wPoint.radius, text);
    }
}

void MainWindow::refreshInfo()
{    
    textBrowser->clear();

    textBrowser->append("Duration:");
    textBrowser->append(mTrackManager->getDuration() + " Hour");

    textBrowser->append("Max / Min Altitude:");
    textBrowser->append(QString::number(mTrackManager->getMaxAltitude(), 'f', 1)
                        + " / "
                        + QString::number(mTrackManager->getMinAltitude(), 'f', 1)
                        + " m"
                        );

    textBrowser->append("Max / Min Vario:");
    textBrowser->append(QString::number(mTrackManager->getMaxVario(), 'f', 1)
                        + " / "
                        + QString::number(mTrackManager->getMinVario(), 'f', 1)
                        + " m/s"
                        );

    textBrowser->append("Max Distance:");
    textBrowser->append(QString::number(mTrackManager->getMaxDistance()/1000, 'f', 1) + " km");

    label_TrackCount->setText(QString::number(mTrackManager->getTrackCount()));
    label_WayPointCount->setText(QString::number(mTrackManager->getWayPointCount()));
}

void MainWindow::setMapRotation(qreal angle)
{
    QVariant vAngle = QVariant::fromValue(angle);
    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setRotation",
                                  Q_ARG(QVariant, vAngle));
    }
}

void MainWindow::setMapTilt(qreal tilt)
{
    QVariant vTilt = QVariant::fromValue(tilt);
    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setTilt",
                                  Q_ARG(QVariant, vTilt));
    }
}

void MainWindow::setMapZoom(qreal zoom)
{
    QVariant vZoom = QVariant::fromValue(zoom);
    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setZoom",
                                  Q_ARG(QVariant, vZoom));
    }
}

void MainWindow::setMapCenter(qreal lat, qreal lng)
{
    QVariant posx = QVariant::fromValue(lat);
    QVariant posy = QVariant::fromValue(lng);

    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setCenter",
                                  Q_ARG(QVariant, posx),
                                  Q_ARG(QVariant, posy));
    }
}

void MainWindow::addWayPointToMap(QVariant latitude, QVariant longitude, QVariant radius, QVariant text)
{
    QVariant returnedValue;

    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "addWayPoint",
                                  Q_ARG(QVariant, QVariant::fromValue(latitude)),
                                  Q_ARG(QVariant, QVariant::fromValue(longitude)),
                                  Q_ARG(QVariant, QVariant::fromValue(radius)),
                                  Q_ARG(QVariant, QVariant::fromValue(text))
                                  );
    }
}

void MainWindow::addTrack(const QList<QGeoCoordinate>& coordList)
{
    QGeoPath mGeoPath;

    for(auto coord: coordList)
    {
        mGeoPath.addCoordinate(coord);
    }

    pathController.setGeoPath(mGeoPath);
}

void MainWindow::clearMap()
{
    QVariant returnedValue;

    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "clearMap");
    }
}

void MainWindow::on_pushButton_LoadIgcPath_clicked()
{
    QString dataLocation = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0);
    if (QDir(dataLocation).exists()) {
        qDebug() << "App data directory exists. " << dataLocation;
    }

    QString path = QFileDialog::getExistingDirectory(this,tr("Choose a Igc Folder"), dataLocation, QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
    if (path.isEmpty())
        return;

    QDir directory(path);

    QStringList igcFiles = directory.entryList(QStringList() << "*.igc" << "*.IGC", QDir::Files);
    for(auto igcFile: igcFiles)
    {
        addIgcFile( path + "/" + igcFile);
    }
}

void MainWindow::on_pushButton_LoadIgc_clicked()
{    
    QString dataLocation = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0);
    if (QDir(dataLocation).exists()) {
        qDebug() << "App data directory exists. " << dataLocation;
    }

    auto fileName = QFileDialog::getOpenFileName(this,  tr("Choose Igc File"), dataLocation, tr("Igc File (*.igc)"));
    if (!fileName.isEmpty())
    {
        addIgcFile(fileName);
    }
}

void MainWindow::on_pushButton_LoadWpt_clicked()
{
    QString dataLocation = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0);
    if (QDir(dataLocation).exists()) {
        qDebug() << "App data directory exists. " << dataLocation;
    }

    auto fileName = QFileDialog::getOpenFileName(this,  tr("Choose WP File"), dataLocation, tr("Wpt File (*.wpt)"));
    if (!fileName.isEmpty())
    {
        addWpFile(fileName);
    }
    else
    {
        addWpFile("CurrentTask.wpt");
    }
}

void MainWindow::on_pushButton_CreateWpt_clicked()
{
    if(mTrackManager)
    {
        if(mTrackManager->getWayPointCount() == 0)
        {
            QMessageBox::warning(
                        this,
                        tr("Thermal Finder"),
                        tr("No Waypoint found.\nPlease load igc file first.") );
            return;
        }

        QDateTime currentDate = QDateTime::currentDateTime();
        currentDate.setTimeSpec(Qt::UTC);
        QDateTime localTime = currentDate.toLocalTime();

        QString dataLocation = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).value(0);
        if (QDir(dataLocation).exists()) {
            qDebug() << "App data directory exists. " << dataLocation;
        }

        QString wptName = "Wpt_" +  localTime.toString("dd-MM-yyyy_HH-mm-ss") + ".wpt";
        QString filters("WayPoint files (*.wpt)");
        QString defaultFilter("WayPoint files (*.wpt)");

        QString fileName =  QFileDialog::getSaveFileName(nullptr, tr("Save WayPoint file"), dataLocation + "/" + wptName,
                                                         filters, &defaultFilter);

        if (fileName.isEmpty())
            return;

        if(mTrackManager->writeWayPoints(fileName))
        {
            QMessageBox::information(
                        this,
                        tr("Thermal Finder"),
                        tr("Waypoint file created successfully.\n" ) + QDir::currentPath() );
        }
        else
        {
            QMessageBox::critical(
                        this,
                        tr("Thermal Finder"),
                        tr("Error: Waypoint file not created.") );
        }
    }
}

void MainWindow::on_pushButton_Clear_clicked()
{
    totalFiles = 0;
    textBrowser->clear();
    mTrackManager->resetValues();
    label_TrackCount->setText("0");
    label_WayPointCount->setText("0");
    textBrowser->append("Please Load Igc File...");
    clearMap();
}

void MainWindow::on_pushButton_Exit_clicked()
{
    exit(0);
}

bool MainWindow::startGpsSource()
{
    m_posSource = QGeoPositionInfoSource::createDefaultSource(this);
    if (m_posSource == nullptr)
    {
        qDebug() << "error: createDefaultSource";
        return false;
    }

    if(!m_posSource->supportedPositioningMethods())
    {
        m_posSource = nullptr;
        qDebug() << "error: supportedPositioningMethods";
        return false;
    }

    m_posSource->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
    m_posSource->setUpdateInterval(0);
    connect(m_posSource, &QGeoPositionInfoSource::positionUpdated, this, &MainWindow::positionUpdated);
    connect(m_posSource, &QGeoPositionInfoSource::updateTimeout, this, &MainWindow::updateTimeout);

    QString status;
    status.append("Active Gps Source is: ");
    status.append( m_posSource->sourceName());

    qDebug() << status;
    m_posSource->startUpdates();

    return true;
}

void MainWindow::positionUpdated(QGeoPositionInfo gpsPos)
{
    if (!gpsPos.isValid() || !gpsPos.coordinate().isValid())
        return;

    setMapCenter(gpsPos.coordinate().latitude() , gpsPos.coordinate().longitude() );

    auto m_direction = gpsPos.attribute(QGeoPositionInfo::Direction);    
    setMapRotation(360 - m_direction);
}

void MainWindow::updateTimeout(void)
{
    qDebug() << "updateTimeout";
}

