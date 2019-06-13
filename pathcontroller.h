#ifndef PATHCONTROLLER_H
#define PATHCONTROLLER_H
#include <QGeoPath>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class PathController : public QObject {
    Q_OBJECT
    Q_PROPERTY(
            QGeoPath geopath READ geoPath NOTIFY geopathChanged)
public:
    PathController(QObject *parent = nullptr) : QObject(parent) {}

    QGeoPath geoPath() const { return mGeoPath; }

    Q_INVOKABLE void setGeoPath(const QGeoPath &geoPath)
    {
        if (geoPath == mGeoPath)
            return;
        mGeoPath = geoPath;
        emit geopathChanged();
    }  

    Q_INVOKABLE void clear()
    {
        mGeoPath.clearPath();
        emit geopathChanged();
    }

signals:
    void geopathChanged();

private:
    QGeoPath mGeoPath;
};


#endif // PATHCONTROLLER_H
