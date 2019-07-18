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

using namespace std;

struct WayPoint{
    QString name;
    QGeoCoordinate coord;
    qreal vario;
    qreal distance;
    qreal radius;
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

    bool IsNan( float value )
    {
        return ((*(uint*)&value) & 0x7fffffff) > 0x7f800000;
    }

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
    WayPoint getWPTbyIndex(int index);
    //WayPoint getTrackbyIndex(int index);
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
    QList<QGeoCoordinate> getPath() const;    

    std::vector<WayPoint> getWayPointList() const;

    void setVarioFactor(int value);

private:

    std::vector<shared_ptr<WayPoint>> mTrackPointList{};
    std::vector<WayPoint> mWayPointList;
    std::vector<shared_ptr<QString>>  mFileList{};

    QDateTime  oldTrackTime{};
    qreal      oldAltitude{0};
    qreal      varioMin{INT_MAX};
    int        varioFactor{0};
    qreal      maxAltitude{INT_MIN};
    qreal      minAltitude{INT_MAX};
    qreal      maxDistance{INT_MIN};
    qreal      maxVarioValue{INT_MIN};
    qreal      minVarioValue{INT_MAX};

    static TrackManager* instance;

};

#endif // THERMALPOINTS_H
