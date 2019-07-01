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
    setGeometry(0, 0, 900, 650);


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

    QLabel *label_VUp = new QLabel(" Vario Up (sec)\t");
    label_VUp->setStyleSheet("font-size: 10pt; color: white;background-color: rgba(0, 102, 102, 175);");
    label_VUp->setFixedWidth(110);

    lineEdit_VarioUp = new QLineEdit("60");
    lineEdit_VarioUp->setStyleSheet("font-size: 10pt; color: black; background-color: white;");
    lineEdit_VarioUp->setFixedWidth(32);
    lineEdit_VarioUp->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), lineEdit_VarioUp));

    QLabel *label_VDown = new QLabel(" Vario Down (sec)\t");
    label_VDown->setStyleSheet("font-size: 10pt; color: white;background-color: rgba(0, 102, 102, 175);");
    label_VDown->setFixedWidth(110);

    lineEdit_VarioDown = new QLineEdit("360");
    lineEdit_VarioDown->setStyleSheet("font-size: 10pt; color: black; background-color: white;");
    lineEdit_VarioDown->setFixedWidth(32);
    lineEdit_VarioDown->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), lineEdit_VarioDown));

    textBrowser = new QTextBrowser();
    textBrowser->setStyleSheet("font: 10pt; color: #00cccc; background-color: #001a1a;");
    textBrowser->append("Please Load Igc File...");
    textBrowser->setFixedWidth(150);

    mapView = new QQuickView();
    mapView->setSource(QUrl(QStringLiteral("qrc:/qml/MapView.qml")));
    mapView->setResizeMode(QQuickView::SizeRootObjectToView);
    mapView->rootContext()->setContextProperty("pathController", &pathController);

    rootObject = mapView->rootObject();


    QWidget * wdg = new QWidget(this);
    QHBoxLayout *hlay = new QHBoxLayout(wdg);
    vLayMap = new QVBoxLayout();
    vLayControl = new QVBoxLayout();

    QHBoxLayout *hlayEditMin = new QHBoxLayout();
    hlayEditMin->addWidget(label_VMin);
    hlayEditMin->addWidget(lineEdit_VarioMin);

    QHBoxLayout *hlayEditUp = new QHBoxLayout();
    hlayEditUp->addWidget(label_VUp);
    hlayEditUp->addWidget(lineEdit_VarioUp);

    QHBoxLayout *hlayEditDown= new QHBoxLayout();
    hlayEditDown->addWidget(label_VDown);
    hlayEditDown->addWidget(lineEdit_VarioDown);

    QWidget *qmlWidgetMap = QWidget::createWindowContainer(mapView, this);
    vLayMap->addWidget(qmlWidgetMap);
    hlay->addLayout(vLayMap);

    vLayControl->addWidget(pushButton_LoadIgc);
    vLayControl->addWidget(pushButton_LoadIgcPath);
    vLayControl->addWidget(pushButton_CreateWp);
    vLayControl->addWidget(pushButton_LoadWp);
    vLayControl->addWidget(pushButton_Clear);
    vLayControl->addWidget(label_TCount);
    vLayControl->addWidget(label_TrackCount);
    vLayControl->addWidget(label_WCount);
    vLayControl->addWidget(label_WayPointCount);
    vLayControl->addLayout(hlayEditMin);
    vLayControl->addLayout(hlayEditUp);
    vLayControl->addLayout(hlayEditDown);
    vLayControl->addWidget(textBrowser);
    vLayControl->addWidget(pushButton_Exit);
    vLayControl->setAlignment(Qt::AlignTop);

    for (int i = 0; i < vLayControl->count(); ++i)
    {
        QWidget *widget = vLayControl->itemAt(i)->widget();
        if (widget != nullptr)
        {
            QTextBrowser *tempTextBrowser = dynamic_cast<QTextBrowser*>(widget);
            if(!tempTextBrowser)
                vLayControl->setAlignment(widget, Qt::AlignCenter);
        }
    }

    hlay->addLayout(vLayControl);

    wdg->setLayout(hlay);
    setCentralWidget(wdg);

    setMapTilt(0);
    setMapRotation(0);
    setMapZoom(12);

    startGpsSource();

    CopyResources();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::CopyResources()
{
    QString wptFileName(":/igc/Ayas_4_Vario.wpt");

    QDataStream in, out;
    QFile wptFile(wptFileName);
    if (!wptFile.open(QIODevice::ReadOnly))
        return EXIT_FAILURE;

    in.setDevice(&wptFile);

    QFile result("CurrentTask.wpt");
    if (!result.open(QIODevice::WriteOnly))
        return EXIT_FAILURE;

    out.setDevice(&result);

    char *data = new char[wptFile.size()];

    in.readRawData(data, wptFile.size());
    out.writeRawData(data, wptFile.size());

    result.close();
    wptFile.close();

    delete [] data;

    addWpFile("CurrentTask.wpt");
    return EXIT_SUCCESS;
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

        auto min = static_cast<qreal>(lineEdit_VarioMin->text().toDouble());
        auto up = static_cast<int>(lineEdit_VarioUp->text().toInt());
        auto down = static_cast<int>(lineEdit_VarioDown->text().toInt());

        mTrackManager->setVarioMin(min);
        mTrackManager->setVarioUpCount(up);
        mTrackManager->setVarioDownCount(down);

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

        addWayPointToMap(coord.latitude(), coord.longitude(), wPoint.width, text);
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
    QString path = QFileDialog::getExistingDirectory(this,tr("Choose a Igc Folder"), QDir::currentPath(), QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
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
    auto fileName = QFileDialog::getOpenFileName(this,  tr("Choose Igc File"), QDir::currentPath(), tr("Igc File (*.igc)"));
    if (!fileName.isEmpty())
    {
        addIgcFile(fileName);
    }
}

void MainWindow::on_pushButton_LoadWpt_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,  tr("Choose WP File"), QDir::currentPath(), tr("Wpt File (*.wpt)"));
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

        if(mTrackManager->writeWayPoints())
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
    if(IsNan(static_cast<float>(m_direction))) m_direction = 0;

    setMapRotation(360 - m_direction);
}

void MainWindow::updateTimeout(void)
{
    qDebug() << "updateTimeout";
}

