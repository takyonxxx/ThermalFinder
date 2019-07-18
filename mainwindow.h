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

#if defined(ANDROID) || defined(__ANDROID__)
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QKeyEvent>
#endif

#define SCREEN_ORIENTATION_LANDSCAPE 0
#define SCREEN_ORIENTATION_PORTRAIT 1

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

#if defined(ANDROID) || defined(__ANDROID__)
    void keep_screen_on(bool on) {
        QtAndroid::runOnAndroidThread([on]{
            QAndroidJniObject activity = QtAndroid::androidActivity();
            if (activity.isValid()) {
                QAndroidJniObject window =
                        activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

                if (window.isValid()) {
                    const int FLAG_KEEP_SCREEN_ON = 128;
                    if (on) {
                        window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                    } else {
                        window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                    }
                }
            }
            QAndroidJniEnvironment env;
            if (env->ExceptionCheck()) {
                env->ExceptionClear();
            }
        });
    }

    bool requestFineLocationPermission()
    {
        QtAndroid::PermissionResult request = QtAndroid::checkPermission("android.permission.ACCESS_FINE_LOCATION");
        if (request == QtAndroid::PermissionResult::Denied){
            QtAndroid::requestPermissionsSync(QStringList() <<  "android.permission.ACCESS_FINE_LOCATION");
            request = QtAndroid::checkPermission("android.permission.ACCESS_FINE_LOCATION");
            if (request == QtAndroid::PermissionResult::Denied)
            {
                qDebug() << "FineLocation Permission denied";
                return false;
            }
        }
        qDebug() << "FineLocation Permissions granted!";
        return true;
    }

    bool requestStorageWritePermission() {
        QtAndroid::PermissionResult request = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
        if(request == QtAndroid::PermissionResult::Denied) {
            QtAndroid::requestPermissionsSync( QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE" );
            request = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
            if(request == QtAndroid::PermissionResult::Denied)
            {
                qDebug() << "StorageWrite Permission denied";
                return false;
            }
        }
        qDebug() << "StorageWrite Permissions granted!";
        return true;
    }
#endif

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
#if defined(ANDROID) || defined(__ANDROID__)
    bool setScreenOrientation(int orientation);
    void keyPressEvent(QKeyEvent *k);
#endif


    QGeoPositionInfoSource *m_posSource{};

    TrackManager *mTrackManager{};
    QQuickView *mapView{};
    QObject *rootObject{};
    QLabel *label_TrackCount{};
    QLabel *label_WayPointCount{};
    QLineEdit *lineEdit_VarioMin{};
    QLineEdit *lineEdit_VarioFactor{};
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
