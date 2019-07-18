#include "trackmanager.h"
#include "conversions.h"
#include <QtDebug>

static const QString newDateFormat = QString("HFDTEDATE:"); // DD MM YY , NN CRLF HFDTEDATE:270418,01
static const QString oldDateFormat = QString("HFDTE");      // DD MM YY CRLF HFDTE270418
static int WPTCount = 0;

TrackManager* TrackManager::instance = nullptr;

TrackManager* TrackManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new TrackManager();
    }
    return instance;
}

TrackManager::TrackManager()   
{   
}

TrackManager:: ~TrackManager()
{
}

bool TrackManager::addIgcFile(const QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(nullptr, "error", file.errorString());
        return false;
    }

    QTextStream in(&file);

    if(in.atEnd())
        return false;

    QDate flightDate{};

    while(!in.atEnd())
    {
        WayPoint p;
        QString line = in.readLine();
        QDateTime trackTime{};

        //B,100000,3959138,N,03222035,E,A,01847,01945

        if(line.startsWith(newDateFormat))
        {
            auto strDate = line.mid(newDateFormat.size(), 6);
            flightDate = getFlightDate(strDate);
        }
        else if(line.startsWith(oldDateFormat))
        {
            auto strDate = line.mid(oldDateFormat.size(), 6);
            flightDate = getFlightDate(strDate);
        }
        else if(line.startsWith("B"))
        {
            auto strDateTime = flightDate.toString("dd-MM-yyyy") + " "
                    + line.mid(1, 2) + ":" + line.mid(3, 2) + ":" + line.mid(5, 2);

            trackTime =  QDateTime::fromString(strDateTime,"dd-MM-yyyy HH:mm:ss");

            auto lat      = line.mid(7, 7);
            auto lathems  = line.mid(14, 1);
            auto lon      = line.mid(15, 8);
            auto lonhems  = line.mid(23, 1);
            //auto alt1   = line.mid(25,5);
            auto altitude   = line.mid(30,5).toDouble();//gps
            auto latDec = DMS::DDMMmmmToDecimalLat(lat.toStdWString(), lathems.toStdWString());
            auto lonDec = DMS::DDMMmmmToDecimalLon(lon.toStdWString(), lonhems.toStdWString());

            auto secondsDiff = static_cast<qreal>(oldTrackTime.msecsTo(trackTime) / 1000.0);

            qreal vario{0};

            if(secondsDiff > 0)
            {
                auto altitudeDiff = altitude - oldAltitude;
                vario = static_cast<qreal>(altitudeDiff / secondsDiff);
            }

            QGeoCoordinate m_coord;

            m_coord.setLatitude(latDec);
            m_coord.setLongitude(lonDec);
            m_coord.setAltitude(altitude);

            p.coord = m_coord;
            p.vario = vario;
            p.dateTime = trackTime;
            addTrackPointToList(p);

            oldTrackTime = trackTime;
            oldAltitude = altitude;
        }
    }

    file.close();
    return true;
}

unsigned int TrackManager::calculateWayPoints()
{
    vector<qreal> vecWPVario{};
    auto foundThermal = false;
    auto beginCoord = mTrackPointList.at(0)->coord;
    QGeoCoordinate beginThermalCoord {};
    QGeoCoordinate currentThermalCoord {};
    int sinkCount = 0;
    int upCount = 0;
    int trackCount = 0;
    auto beginThermalAlt = 0.0;

    for (auto trackPoint : mTrackPointList)
    {
        QGeoCoordinate m_coord;
        m_coord.setLatitude(trackPoint.get()->coord.latitude());
        m_coord.setLongitude(trackPoint.get()->coord.longitude());
        m_coord.setAltitude(trackPoint.get()->coord.altitude());

        if(trackPoint.get()->vario >= varioMin)
        {
            if(!foundThermal)
            {
                beginThermalAlt = m_coord.altitude();
                beginThermalCoord = m_coord;
                foundThermal = true;
            }

            vecWPVario.emplace_back(trackPoint.get()->vario);
            currentThermalCoord = m_coord;
            sinkCount = 0;
            upCount++;
        }
        else
        {
            if(sinkCount >= varioFactor)
            {
                auto average = accumulate(vecWPVario.begin(), vecWPVario.end(), 0.0) / vecWPVario.size();
                auto radius = beginThermalCoord.distanceTo(currentThermalCoord) / 2;
                auto distanceWP = beginCoord.distanceTo(currentThermalCoord);
                auto name = QString("Wpt") + QString::number(WPTCount + 1);

                if(upCount > varioFactor)
                {
                    int wpPoint = trackCount - sinkCount - static_cast<int>(vecWPVario.size()/2);

                    WayPoint p;
                    p.name = name;
                    p.coord = mTrackPointList.at(wpPoint).get()->coord;
                    p.vario = QString::number(average, 'f', 2).toDouble();
                    p.radius = radius;
                    p.distance = distanceWP;
                    p.dateTime = mTrackPointList.at(wpPoint).get()->dateTime;
                    addWayPointToList(p);
                    WPTCount++;
                }

                foundThermal = false;
                vecWPVario.clear();
                upCount = 0;
            }

            sinkCount++;
        }

        auto distance = beginCoord.distanceTo(m_coord);
        auto vario = trackPoint.get()->vario;
        auto altitude = trackPoint.get()->coord.altitude();

        if(distance > maxDistance)
            maxDistance = distance;

        if(vario > maxVarioValue)
            maxVarioValue = vario;
        else if(vario < minVarioValue)
            minVarioValue = vario;

        if(altitude > maxAltitude)
            maxAltitude = altitude;
        else if(altitude < minAltitude)
            minAltitude = altitude;

        trackCount++;
    }

    return static_cast<unsigned int>(WPTCount);
}

bool TrackManager::isExistFile(const QString &filename)
{
    /*auto alreadyExists = any_of(mFileList.begin() , mFileList.end() , [&filename](auto entry){
        return entry.get() == filename;
    });*/

    for (auto it = mFileList.begin();it < mFileList.end(); it++) {
        if(it->get() == filename)
            return true;
    }

    return false;
}

bool TrackManager::isExistWPT(const QGeoCoordinate &m_coord)
{
    /*auto alreadyExists = any_of(mWayPointList.begin() , mWayPointList.end() , [&m_coord](auto entry){
        return entry.coord == m_coord;
    });*/


    for (auto it = mWayPointList.begin();it < mWayPointList.end(); it++) {
        if(it->coord == m_coord)
            return true;
    }

    return false;
}

bool TrackManager::isExistTrack(const QGeoCoordinate &m_coord)
{
    /*auto alreadyExists = any_of(mTrackPointList.begin() , mTrackPointList.end() , [&m_coord](auto entry){
        return entry.get()->coord == m_coord;
    });*/

    for (auto it = mTrackPointList.begin();it < mTrackPointList.end(); it++) {
        if(it->get()->coord == m_coord)
            return true;
    }

    return false;
}

WayPoint TrackManager::getWPTbyIndex(int index)
{
    auto wpt = mWayPointList.at(index);
    return wpt;
}

void TrackManager::addFileToList(const QString &filename)
{
    mFileList.emplace_back(make_shared<QString>(filename));
}

void TrackManager::clearIgcFiles()
{
    mFileList.clear();
}

void TrackManager::addTrackPointToList(const WayPoint &point)
{    
    mTrackPointList.emplace_back(make_shared<WayPoint>(point));
}

unsigned int TrackManager::getTrackCount()
{
    return static_cast<unsigned int>(mTrackPointList.size());
}

void TrackManager::clearTrackPoints()
{
    mTrackPointList.clear();
}

void TrackManager::addWayPointToList(const WayPoint &point)
{
    if(!isExistWPT(point.coord))
        mWayPointList.emplace_back(point);
}

unsigned int TrackManager::getWayPointCount()
{
    return static_cast<unsigned int>(mWayPointList.size());
}

void TrackManager::clearWayPoints()
{
    mWayPointList.clear();
}

void TrackManager::resetValues()
{
    oldTrackTime = QDateTime::currentDateTime();
    oldAltitude = 0;
    maxAltitude = INT_MIN;
    minAltitude = INT_MAX;
    maxDistance = INT_MIN;
    maxVarioValue = INT_MIN;
    minVarioValue = INT_MAX;
    WPTCount = 0;

    clearIgcFiles();
    clearWayPoints();
    clearTrackPoints();
}

qreal TrackManager::getMaxAltitude() const
{
    return maxAltitude;
}

qreal TrackManager::getMinAltitude() const
{
    return minAltitude;
}

qreal TrackManager::getMaxDistance() const
{
    return  maxDistance;
}

qreal TrackManager::getMaxVario() const
{
    return maxVarioValue;
}

qreal TrackManager::getMinVario() const
{
    return minVarioValue;
}

QString TrackManager::getDuration() const
{
    QString result{};

    auto size =  mTrackPointList.size();
    if(size == 0) return result;

    auto beginTime =  mTrackPointList.at(0).get()->dateTime;
    auto endTime =  mTrackPointList.at(size -1).get()->dateTime;

    beginTime.setTimeSpec(Qt::UTC);
    endTime.setTimeSpec(Qt::UTC);

    QDateTime localBegin = beginTime.toLocalTime();
    QDateTime localEnd   = endTime.toLocalTime();

    auto diff = localBegin.msecsTo(localEnd);

    long long diffMinutes = diff / (60 * 1000) % 60;
    long long diffHours = diff / (60 * 60 * 1000);

    result.append(localBegin.time().toString("HH:mm:ss"));
    result.append(" - ");
    result.append(localEnd.time().toString("HH:mm:ss"));
    result.append("\n");
    result.append(QString::number(diffHours));
    result.append(".");
    result.append(QString::number(diffMinutes));

    return result;
}

void TrackManager::setVarioMin(const qreal &value)
{
    varioMin = value;
}

QList<QGeoCoordinate> TrackManager::getPath() const
{   
    QList<QGeoCoordinate> cList;

    for (auto tPoint : mTrackPointList)
    {
        QGeoCoordinate m_coord;
        m_coord.setLatitude(tPoint.get()->coord.latitude());
        m_coord.setLongitude(tPoint.get()->coord.longitude());
        cList.append(m_coord);
    }

    return cList;
}

std::vector<WayPoint> TrackManager::getWayPointList() const
{
    return mWayPointList;
}

void TrackManager::setVarioFactor(int value)
{
    varioFactor = value;
}

QDate TrackManager::getFlightDate(const QString &dateString)
{
    if (dateString.isEmpty()) return QDate{};

    auto dd = dateString.mid(0,2);
    auto mm = dateString.mid(2,2);
    auto yyyy = QString::number(QDate::currentDate().year()).mid(0,2) + dateString.mid(4,2);
    return QDate::fromString(dd + "-" + mm + "-" + yyyy, "dd-MM-yyyy");
}

bool TrackManager::writeWayPoints(const QString &fileName)
{
    ofstream wpFile;
    wpFile.open(fileName.toStdString(), ios::out);

    if (!wpFile) {
        return false;
    }

    string format_string = "$FormatGEO\n";
    wpFile << format_string;

    for (auto wPoint : mWayPointList)
    {
        QString wpLine{};
        QString tempStr{};

        tempStr = wPoint.name;
        tempStr = tempStr.leftJustified(10, ' ');
        wpLine.append(tempStr);

        tempStr = QString::fromLocal8Bit(DMS::DegreesMinutesSecondsLat(wPoint.coord.latitude()).c_str());
        tempStr = tempStr.leftJustified(tempStr.size() + 4, ' ');
        wpLine.append(tempStr);

        tempStr = QString::fromLocal8Bit(DMS::DegreesMinutesSecondsLon(wPoint.coord.longitude()).c_str());
        tempStr = tempStr.leftJustified(tempStr.size() + 4, ' ');
        wpLine.append(tempStr);

        tempStr = QString::number(wPoint.coord.altitude());
        tempStr = tempStr.leftJustified(tempStr.size() + 2, ' ');
        wpLine.append(tempStr);

        tempStr = QString::number(wPoint.vario, 'f', 2);      ;
        tempStr = tempStr + QString(" ms");
        wpLine.append(tempStr);

        if (&wPoint.coord != &mWayPointList.back().coord)
        {
            wpLine.append("\n");
        }

        wpFile << wpLine.toStdString();
    }

    wpFile.close();

    if (!wpFile) {
        return false;
    }
    return true;
}

bool TrackManager::addWayPointFile(const QString &fileName)
{
    ifstream wpFile;
    wpFile.open(fileName.toStdString(), ios::in);

    if(!wpFile.is_open())
        return false;

    QGeoCoordinate m_coord;

    std::string header;
    getline(wpFile, header);
    QString sHeader = QString::fromLocal8Bit(header.c_str());
    if(!sHeader.startsWith("$FormatGEO"))
    {
        QMessageBox::critical(
                    nullptr,
                    tr("Thermal Finder"),
                    tr("Error: Waypoint file not loaded.\n"
                       "Supported file is Geographical WPT file.\n$FormatGEO")
                    );
        return false;
    }

    std::string line;
    while (getline(wpFile, line))
    {
        QString sLine = QString::fromLocal8Bit(line.c_str());
        if(sLine.startsWith("$FormatGEO"))
            continue;

        QString lat = sLine.mid(10,17).replace(" ", "");
        auto decLat = DMS::DecimalDegrees(lat.toStdString());

        QString lon =  sLine.mid(27,17).replace(" ", "");
        auto decLon = DMS::DecimalDegrees(lon.toStdString());

        QString alt =  sLine.mid(44,6).replace(" ", "");
        auto altitude = alt.toDouble();

        QString name =  sLine.mid(0,10).replace(" ", "");

        m_coord.setLatitude(decLat);
        m_coord.setLongitude(decLon);
        m_coord.setAltitude(altitude);

        WayPoint p;
        p.name = name;
        p.coord = m_coord;
        p.vario = 0;
        p.radius = 1000;
        addWayPointToList(p);
    }

    wpFile.close();

    return true;
}

//Calculate midpoint from a series of latitude and longitude
QGeoCoordinate TrackManager::GetMidPoint(std::vector<WayPoint> data)
{
    if (!(data.size() > 0)){
        return QGeoCoordinate();
    }

    auto num_coords = data.size();

    auto X = 0.0;
    auto Y = 0.0;
    auto Z = 0.0;

    for(unsigned int i = 0; i < num_coords; i++)
    {
        auto lat = toRadians(data.at(i).coord.latitude());
        auto lon = toRadians(data.at(i).coord.longitude());

        auto a = cos(lat) * cos(lon);
        auto b = cos(lat) * sin(lon);
        auto c = sin(lat);

        X += a;
        Y += b;
        Z += c;
    }

    X /= num_coords;
    Y /= num_coords;
    Z /= num_coords;

    auto lon = atan2(Y, X);
    auto hyp = sqrt(X * X + Y * Y);
    auto lat = atan2(Z, hyp);

    auto newX = toDegrees(lat);
    auto newY = toDegrees(lon);

    return QGeoCoordinate(newX, newY);
}
