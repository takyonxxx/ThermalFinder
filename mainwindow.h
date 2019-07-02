#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQuickWidget>
#include <QtWidgets>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QQuickItem>
#include <QGeoCoordinate>
#include "trackmanager.h"
#include "pathcontroller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool IsNan( float value )
    {
        return ((*(uint*)&value) & 0x7fffffff) > 0x7f800000;
    }

private slots:
    void on_pushButton_LoadIgc_clicked();
    void on_pushButton_LoadIgcPath_clicked();
    void on_pushButton_LoadWpt_clicked();
    void on_pushButton_CreateWpt_clicked();
    void on_pushButton_Clear_clicked();
    void on_pushButton_Exit_clicked();
    void positionUpdated(QGeoPositionInfo);
    void updateTimeout(void);

private:
    void addIgcFile(const QString &);
    void addWpFile(const QString &);
    void drawWayPoints(bool);
    void refreshInfo();
    void setMapCenter(qreal, qreal);
    void setMapRotation(qreal);
    void setMapTilt(qreal);
    void setMapZoom(qreal);
    void addWayPointToMap(QVariant, QVariant, QVariant, QVariant);
    void addTrack(const QList<QGeoCoordinate>&);
    void clearMap();
    void CopyResources();
    void copyResource(QString &, const QString &);

    bool startGpsSource();
    QGeoPositionInfoSource *m_posSource{};

    TrackManager *mTrackManager{};
    QQuickView *mapView{};
    QObject *rootObject{};
    QLabel *label_TrackCount{};
    QLabel *label_WayPointCount{};
    QLineEdit *lineEdit_VarioMin{};
    QLineEdit *lineEdit_VarioUp{};
    QLineEdit *lineEdit_VarioDown{};
    QTextBrowser *textBrowser{};
    QVBoxLayout *vLayMap{};
    QVBoxLayout *vLayControl{};
    int totalFiles{0};

    PathController pathController{};

protected:
    void changeEvent( QEvent* e );

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
