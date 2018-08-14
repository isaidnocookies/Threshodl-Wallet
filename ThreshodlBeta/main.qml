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

    DashboardPage {
        id: dashboard
        visible: false
    }

    LockScreen {
        id: loginLockScreen
        allowBack: false
        visible: false
        setupPasscode: false

        onPasscodeComplete: {
            ourStackView.replace(dashboard)
        }
    }

    StackView {
        id: ourStackView
        initialItem: {
            if (userAccount.exists()) {
                if (userAccount.checkPasscode("")) {
                    dashboard
                }
                loginLockScreen
            } else {
                Qt.resolvedUrl("startPage.qml")
            }
        }
        anchors.fill: parent
    }
}
