import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.3

import com.threeb.threshodl.PlatformInformation 1.0
import com.threeb.threshodl.UserAccount 1.0
import com.threeb.threshodl.ThreshodlTools 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Threshodl")

    flags: Qt.MaximizeUsingFullscreenGeometryHint

    PlatformInformation { id: platformInformation }
    UserAccount { id: userAccount }
    ThreshodlTools { id: threshodlTools }

    property real topAreaCorrectionHeight: (platformInformation.getTopAreaCorrection())
    property real bottomAreaCorrectionHeight: (platformInformation.getBottomAreaCorrection())
    property string titleName: "Threshodl Beta"

    property int buttonFontSize: 13

    LockScreen {
        id: loginLockScreen
        allowBack: false
        visible: false
        setupPasscode: false

        onPasscodeComplete: {
            ourStackView.replace(Qt.resolvedUrl("dashboardPage.qml"))
        }
    }

    StackView {
        id: ourStackView
        initialItem: {

//            ourStackView.push(Qt.resolvedUrl("settingsPage.qml"))
//            return

            if (userAccount.exists()) {
                if (userAccount.checkPasscode("")) {
                    ourStackView.replace(Qt.resolvedUrl("dashboardPage.qml"))
                }
                loginLockScreen
            } else {
                Qt.resolvedUrl("startPage.qml")
            }
        }
        anchors.fill: parent
    }
}
