import QtQuick 2.12
import QtPositioning 5.12
import QtLocation 5.12
import QtQuick.Controls 2.5
import QtQuick.Controls 2.0 as C2
import "../helper.js" as Helper

Item  {

    id: mapView
    objectName: "mapView"
    visible: true

    Plugin {
        id: esriPlugin
        name: "esri"
    }

    Map
    {
        id: map
        anchors.fill: parent
        objectName: "map"
        plugin: esriPlugin
        zoomLevel: (maximumZoomLevel - minimumZoomLevel)/2
        center: QtPositioning.coordinate(51.476852,-0.000500)// Greenwich, London, UK
        property var  circles: []
        property var  lines: []
        property var  markers: []

        property variant scaleLengths: [5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000]
        activeMapType: supportedMapTypes[0]

        onSupportedMapTypesChanged: {
            for (var i = 0; i < map.supportedMapTypes.length; i++) {
                cbItems.append({"key": supportedMapTypes[i].name, "value": i})
            }
        }

        function calculateScale()
        {
            var coord1, coord2, dist, text, f
            f = 0
            coord1 = map.toCoordinate(Qt.point(0,scale.y))
            coord2 = map.toCoordinate(Qt.point(0+scaleImage.sourceSize.width,scale.y))
            dist = Math.round(coord1.distanceTo(coord2))

            if (dist === 0) {
                // not visible
            } else {
                for (var i = 0; i < scaleLengths.length-1; i++) {
                    if (dist < (scaleLengths[i] + scaleLengths[i+1]) / 2 ) {
                        f = scaleLengths[i] / dist
                        dist = scaleLengths[i]
                        break;
                    }
                }
                if (f === 0) {
                    f = dist / scaleLengths[i]
                    dist = scaleLengths[i]
                }
            }

            text = Helper.formatDistance(dist)
            scaleImage.width = (scaleImage.sourceSize.width * f) - 2 * scaleImageLeft.sourceSize.width
            scaleText.text = text
        }


        gesture.flickDeceleration: 3000
        gesture.enabled: true

        onCenterChanged:{
            scaleTimer.restart()
        }

        onZoomLevelChanged:{
            scaleTimer.restart()
        }

        onWidthChanged:{
            scaleTimer.restart()
        }

        onHeightChanged:{
            scaleTimer.restart()
        }

        Keys.onPressed: {
            if (event.key === Qt.Key_Plus) {
                map.zoomLevel++
            } else if (event.key === Qt.Key_Minus) {
                map.zoomLevel--
            }
        }

        Timer {
            id: scaleTimer
            interval: 100
            running: false
            repeat: false
            onTriggered: {
                map.calculateScale()
            }
        }

        Item {
            id: scale
            visible: scaleText.text != "0 m"
            z: map.z + 3
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 20
            height: scaleText.height * 2
            width: scaleImage.width

            Image {
                id: scaleImageLeft
                source: "qrc:///icons/scale_end.png"
                anchors.bottom: parent.bottom
                anchors.right: scaleImage.left
            }
            Image {
                id: scaleImage
                source: "qrc:///icons/scale.png"
                anchors.bottom: parent.bottom
                anchors.right: scaleImageRight.left
            }
            Image {
                id: scaleImageRight
                source: "qrc:///icons/scale_end.png"
                anchors.bottom: parent.bottom
                anchors.right: parent.right
            }
            Label {
                id: scaleText
                font.bold: true
                color: "#004EAE"
                anchors.centerIn: parent
                text: "0 m"
            }
            Component.onCompleted: {
                map.calculateScale();
            }
        }

        MapQuickItem {
            id: marker
            anchorPoint.x: image.width/2
            anchorPoint.y: image.height/2
            coordinate: map.center

            sourceItem: Image {
                id: image
                //source: "http://maps.gstatic.com/mapfiles/ridefinder-images/mm_20_red.png"
                source: "qrc:///icons/marker.png"
            }
        }

        Column {
            id: infoText
            y: 10
            x : mapView.width - 180
            spacing: 2
            Text {id: positionText; text: "Zoom:\t" + map.zoomLevel.toFixed(1); color: "blue"; font.bold: true}
            Text {color: positionText.color; font.bold: true; width: mapView.width / 2; elide: Text.ElideRight; text: "Lat:\t" + map.center.latitude.toFixed(6) }
            Text {color: positionText.color; font.bold: true; width: mapView.width / 2; elide: Text.ElideRight; text: "Lon:\t" + map.center.longitude.toFixed(6) }
            Text {color: positionText.color; font.bold: true; width: mapView.width / 2; elide: Text.ElideRight; text: "Tilt:\t" + map.tilt.toFixed(0) }
        }
    }

    GroupBox
    {
        id: mapType
        title:"Map Types"
        font.bold: true
        y: 10
        ComboBox{
            font.bold: false
            textRole: "key"
            currentIndex : 0
            model: ListModel {
                id: cbItems
            }
            onCurrentIndexChanged: map.activeMapType = map.supportedMapTypes[currentIndex]
        }
    }

    //sliders bearing and tilt
    property color labelBackground : "transparent"
    property real fontSize : 14

    Rectangle {
        id: sliderContainer
        height: parent.height - mapType.height - 20
        width: sliderRow.width + 10
        visible: true
        y: mapType.height + 20
        color: Qt.rgba( 0, 191 / 255.0, 255 / 255.0, 0.07)

        property var labelBorderColor: "transparent"
        property var slidersHeight : sliderContainer.height
                                     - rowSliderValues.height
                                     - rowSliderLabels.height
                                     - sliderColumn.spacing * 2
                                     - sliderColumn.topPadding
                                     - sliderColumn.bottomPadding

        Column {
            id: sliderColumn
            spacing: 10
            topPadding: 16
            bottomPadding: 48
            anchors.centerIn: parent

            // the sliders value labels
            Row {
                id: rowSliderValues
                spacing: sliderRow.spacing
                width: sliderRow.width
                height: 32
                property real entryWidth: tiltSlider.width

                Rectangle{
                    color: labelBackground
                    height: parent.height
                    width: parent.entryWidth
                    border.color: sliderContainer.labelBorderColor
                    Label {
                        id: labelBearingValue
                        text: bearingSlider.value.toFixed(2)
                        color: "blue"
                        font.bold: true
                        font.pixelSize: fontSize
                        rotation: -90
                        anchors.centerIn: parent
                    }
                }
                Rectangle{
                    color: labelBackground
                    height: parent.height
                    width: parent.entryWidth
                    border.color: sliderContainer.labelBorderColor
                    Label {
                        id: labelTiltValue
                        text: tiltSlider.value.toFixed(2)
                        color: "blue"
                        font.bold: true
                        font.pixelSize: fontSize
                        rotation: -90
                        anchors.centerIn: parent
                    }
                }
            } // rowSliderValues

            // The sliders row
            Row {
                spacing: -10
                id: sliderRow
                height: sliderContainer.slidersHeight


                C2.Slider {
                    id: bearingSlider
                    height: parent.height
                    from: 0
                    to: 360
                    orientation : Qt.Vertical
                    value: map.bearing
                    onValueChanged: {
                        map.bearing = value;
                    }
                }
                C2.Slider {
                    id: tiltSlider
                    height: parent.height
                    orientation : Qt.Vertical
                    from: map.minimumTilt;
                    to: map.maximumTilt
                    value: map.tilt
                    onValueChanged: {
                        map.tilt = value;
                    }
                }
            } // Row sliders

            // The labels row
            Row {
                id: rowSliderLabels
                spacing: sliderRow.spacing
                width: sliderRow.width
                property real entryWidth: bearingSlider.width
                property real entryHeight: 64

                Rectangle{
                    color: labelBackground
                    height: parent.entryHeight
                    width: parent.entryWidth
                    border.color: sliderContainer.labelBorderColor
                    Label {
                        id: labelBearing
                        text: "Bearing"
                        color: "blue"
                        font.pixelSize: fontSize
                        font.bold: true
                        rotation: -90
                        anchors.centerIn: parent
                    }
                }
                Rectangle{
                    color: labelBackground
                    height: parent.entryHeight
                    width: parent.entryWidth
                    border.color: sliderContainer.labelBorderColor
                    Label {
                        id: labelTilt
                        text: "Tilt"
                        color: "blue"
                        font.pixelSize: fontSize
                        font.bold: true
                        rotation: -90
                        anchors.centerIn: parent
                    }
                }
            } // rowSliderLabels
        } // Column
    } // sliderContainer  

    Component {
        id: mapCircle
        MapCircle {
            border.color: "white"
            opacity: 0.5
        }
    }

    Component {
        id: mapLine
        MapPolyline {
            line.width: 3
            line.color: 'red'
        }
    }

    Component {
        id: mapMarker
        MapQuickItem {
            id: marker
            anchorPoint.x: sourceItem.width/2
            anchorPoint.y: sourceItem.height/2
            sourceItem: Text{
                color:"#242424"
                font.bold: true
                font.pixelSize: 14
                styleColor: "#ECECEC"
                style: Text.Outline
            }
        }
    }

    Connections{
        target: pathController
        onGeopathChanged:
        {
            addPolyLine()
        }
    }

    Component.onCompleted:{

        for (line in map.lines)
            line.path = loadPath()
    }

    function loadPath(){
        var lines = []
        for(var i=0; i < pathController.geopath.size(); i++){
            lines[i] = pathController.geopath.coordinateAt(i)
        }
        return lines;
    }

    function randomBetween(min, max) {
        return Math.random() * (max - min) + min;
    }

    function addPolyLine()
    {
        var polyLine = mapLine.createObject(map, {})
        if(polyLine == null) {
            console.log("error creating polyLine")
        }

        polyLine.line.color = Qt.rgba(Math.random(),Math.random(),Math.random(),1);
        polyLine.path = loadPath()

        map.lines.push(polyLine);
        map.addMapItem(polyLine)
    }

    function addWayPoint(lat, lon, radius, text)
    {
        var circle = mapCircle.createObject(map, {"center.latitude" : lat, "center.longitude": lon})
        if(circle == null) {
            console.log("error creating WayPoint")
            return false
        }
        circle.radius = radius
        circle.border.width = 1
        circle.color = "red"
        circle.border.color = "white"

        var marker = mapMarker.createObject(map,{})
        marker.coordinate = QtPositioning.coordinate(lat, lon)
        marker.sourceItem.text = text
        map.markers.push(marker);
        map.addMapItem(marker)

        map.circles.push(circle);
        map.addMapItem(circle)
    }

    function setRotation(angle)
    {
        map.bearing = angle
        map.update()
    }

    function setTilt(angle)
    {
        map.tilt = angle
        map.update()
    }

    function setZoom(zoom)
    {
        map.zoomLevel = zoom
        map.update()
    }

    function setCenter(lat,lng)
    {
        map.center = QtPositioning.coordinate(lat, lng)
        map.addMapItem(marker)
        map.update()
    }

    function removeAllWaypoints() {
        // Remove all wpt.
        while (map.circles.length > 0) {
            map.removeMapItem(map.circles[0]);
            map.circles[0].destroy();
            map.circles.shift();
        }

        // Remove all markers.
        while (map.markers.length > 0) {
            map.removeMapItem(map.markers[0]);
            map.markers[0].destroy();
            map.markers.shift();
        }
    }

    function removeAllLines() {
        // Remove all lines.
        while (map.lines.length > 0) {
            map.removeMapItem(map.lines[0]);
            map.lines[0].destroy();
            map.lines.shift();
        }
    }

    function clearMap()
    {
        //map.clearMapItems()
        //map.addMapItem(marker)
        removeAllWaypoints()
        removeAllLines()
        pathController.clear()
        map.update()
    }
}
