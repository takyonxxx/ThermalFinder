#ifndef THERMALPOINTS_H
#define THERMALPOINTS_H
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <QDate>
#include <QFile>
#include <QWidget>
#include <QGeoPath>
#include <QGeoPositionInfoSource>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QtMath>
using namespace std;

struct WayPoint{
    QString name;
    QGeoCoordinate coord;
    qreal vario;
    qreal distance;
    qreal width;
    QDateTime dateTime;
    QString toString()
    {
        return  coord.toString()
                + QString(" ")
                + QString::number(vario)
                + QString(" ")
                + QString::number(distance)
                + QString(" ")
                + dateTime.toString();
    }
};

class TrackManager : public QObject
{    
    Q_OBJECT

public:
    TrackManager();
    ~TrackManager();

    static TrackManager* getInstance();

    bool addIgcFile(const QString &);
    unsigned int calculateWayPoints();
    bool writeWayPoints(const QString &);
    bool addWayPointFile(const QString &);
    bool isExistFile(const QString &);
    bool isExistWPT(const QGeoCoordinate &m_coord);
    bool isExistTrack(const QGeoCoordinate &m_coord);
    void addFileToList(const QString &);
    void addTrackPointToList(const WayPoint &);
    void addWayPointToList(const WayPoint &);
    void clearIgcFiles();
    void clearTrackPoints();
    void clearWayPoints();
    void resetValues();
    QGeoCoordinate GetMidPoint(std::vector<WayPoint> data);

    QDate getFlightDate(const QString &dateString);
    unsigned int getTrackCount();
    unsigned int getWayPointCount();

    qreal getMaxAltitude() const;
    qreal getMinAltitude() const;
    qreal getMaxDistance() const;
    qreal getMaxVario() const;
    qreal getMinVario() const;
    QString getDuration() const;

    void setVarioMin(const qreal &value);
    void setVarioUpCount(const int &value);
    void setVarioDownCount(const int &value);

    QList<QGeoCoordinate> getPath() const;    

    std::vector<WayPoint> getWayPointList() const;

private:

    std::vector<shared_ptr<WayPoint>> mTrackPointList{};
    std::vector<WayPoint> mWayPointList;
    std::vector<shared_ptr<QString>>  mFileList{};

    QDateTime  oldTrackTime{};
    qreal      oldAltitude{0};
    qreal      varioMin{INT_MAX};
    qreal      maxAltitude{INT_MIN};
    qreal      minAltitude{INT_MAX};
    qreal      maxDistance{INT_MIN};
    qreal      maxVarioValue{INT_MIN};
    qreal      minVarioValue{INT_MAX};

    int        varioUpCount{0};
    int        varioDownCount{0};    

    static TrackManager* instance;

};

#endif // THERMALPOINTS_H
