import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.3

import com.threeb.threshodl.PlatformInformation 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Threshodl")

    flags: Qt.MaximizeUsingFullscreenGeometryHint

    PlatformInformation { id: platformInformation }

    property real topAreaCorrectionHeight: (platformInformation.getTopAreaCorrection())
    property real bottomAreaCorrectionHeight: (platformInformation.getBottomAreaCorrection())
    property string titleName: "Threshodl Beta"

    property int buttonFontSize: 13

    function loadFirstPage() {
//        return Qt.resolvedUrl("startPage.qml")
        return Qt.resolvedUrl("dashboardPage.qml")
    }

    StackView {
        id: ourStackView
        initialItem: loadFirstPage()
        anchors.fill: parent
    }
}
