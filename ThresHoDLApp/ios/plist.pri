darwin {
    INFOPLIST = \
       "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" \
       "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" \
       "<plist version=\"1.0\">" \
       "<dict>" \
        "    <key>CFBundleIconFile</key>" \
        "    <string></string>" \
        "    <key>CFBundlePackageType</key>" \
        "    <string>APPL</string>" \
        "    <key>CFBundleGetInfoString</key>" \
        "    <string>Created by ThresHodl</string>" \
        "    <key>CFBundleSignature</key>" \
        "    <string>????</string>" \
        "    <key>CFBundleExecutable</key>" \
        "    <string>$$TARGET</string>" \
        "    <key>CFBundleIdentifier</key>" \
        "    <string>com.threeb.$${LITERAL_DOLLAR}{PRODUCT_NAME:rfc1034identifier}</string>" \
        "    <key>CFBundleDisplayName</key>" \
        "    <string>threshodl</string>" \
        "    <key>CFBundleName</key>" \
        "    <string>$${LITERAL_DOLLAR}{PRODUCT_NAME}</string>" \
        "    <key>CFBundleShortVersionString</key>" \
        "    <string>1.0</string>" \
        "    <key>CFBundleVersion</key>" \
        "    <string>1.0</string>" \
        "    <key>LSRequiresIPhoneOS</key>" \
        "    <true/>" \
        "    <key>UILaunchStoryboardName</key>" \
        "    <string>LaunchScreen</string>" \
        "    <key>UISupportedInterfaceOrientations</key>" \
        "    <array>" \
        "            <string>UIInterfaceOrientationPortrait</string>" \
        "    </array>" \
        "    <key>NSCameraUsageDescription</key>" \
        "    <string>QR Scanner needs camera access</string>" \
        "    <key>NSMicrophoneUsageDescription</key>" \
        "    <string>QR Scanner needs camera access</string>" \
        "    <key>LSApplicationQueriesSchemes</key>" \
        "    <array>" \
        "    <string>https</string>" \
        "    </array>" \
        "    <key>CFBundleAllowMixedLocalizations</key>" \
        "    <true/>" \
        "    <key>CFBundleDocumentTypes</key>" \
        "        <array>" \
        "            <dict>" \
        "                <key>CFBundleTypeExtensions</key>" \
        "                <array>" \
        "                    <string>\"*\"</string>" \
        "                </array>" \
        "                <key>CFBundleTypeName</key>" \
        "                <string>Unknown</string>" \
        "                <key>LSItemContentTypes</key>" \
        "                <array>" \
        "                    <string>public.data</string>" \
        "                </array>" \
        "            </dict>" \
        "        </array>" \
        "    <key>CFBundleIcons</key>" \
        "    <dict>" \
        "        <key>CFBundlePrimaryIcon</key>" \
        "        <dict>" \
        "            <key>CFBundleIconFiles</key>" \
        "            <array>" \
        "                <string>Icon-App-29x29@1x.png</string>" \
        "                <string>Icon-App-29x29@2x.png</string>" \
        "                <string>Icon-App-40x40@2x.png</string>" \
        "                <string>Icon-App-57x57@1x.png</string>" \
        "                <string>Icon-App-57x57@2x.png</string>" \
        "                <string>Icon-App-60x60@2x.png</string>" \
        "            </array>" \
        "        </dict>" \
        "    </dict>" \
        "</dict>" \
        "</plist>"
    write_file($$OUT_PWD/Info.plist, INFOPLIST)|error()
    QMAKE_INFO_PLIST = $$OUT_PWD/Info.plist
}
