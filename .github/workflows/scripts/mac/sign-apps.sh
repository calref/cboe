#! /bin/bash

# CODE-SIGNING STEP
# Original Source: https://federicoterzi.com/blog/automatic-code-signing-and-notarization-for-macos-apps-using-github-actions/
# Modified by NQNStudios

# Turn our base64-encoded certificate back to a regular .p12 file

echo $PROD_MACOS_CERTIFICATE | base64 --decode > certificate.p12

# We need to create a new keychain, otherwise using the certificate will prompt
# with a UI dialog asking for the certificate password, which we can't
# use in a headless CI environment

security create-keychain -p "$PROD_MACOS_CI_KEYCHAIN_PWD" build.keychain
security default-keychain -s build.keychain
security unlock-keychain -p "$PROD_MACOS_CI_KEYCHAIN_PWD" build.keychain
security import certificate.p12 -k build.keychain -P "$PROD_MACOS_CERTIFICATE_PWD" -T /usr/bin/codesign
security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k "$PROD_MACOS_CI_KEYCHAIN_PWD" build.keychain

sign() {
    if [ "$SIGN" = "no" ];
    then
        return
    fi
    APP_PATH="build/Blades of Exile/$1.app"

    # We finally codesign our app bundle, specifying the Hardened runtime option

    /usr/bin/codesign --force -s "$PROD_MACOS_CERTIFICATE_NAME" --options runtime "$APP_PATH"/Contents/Frameworks/*.dylib -v
    (cd "$APP_PATH" && frameworks=Contents/Frameworks/*.framework/Versions/A/* && \
        for framework in $frameworks; do
            if [ -f "$framework" ]; then
                /usr/bin/codesign --force -s "$PROD_MACOS_CERTIFICATE_NAME" --options runtime "$framework" -v
            fi
        done)
    /usr/bin/codesign --force -s "$PROD_MACOS_CERTIFICATE_NAME" --options runtime "$APP_PATH"/Contents/Frameworks/*.framework/Versions/A/Resources/Info.plist -v
    /usr/bin/codesign --force -s "$PROD_MACOS_CERTIFICATE_NAME" --options runtime "$APP_PATH/Contents/Info.plist" -v

    /usr/bin/codesign --force -s "$PROD_MACOS_CERTIFICATE_NAME" --options runtime "$APP_PATH" -v

    # NOTARIZATION STEP
    if [ "$NOTARIZE" = "no" ];
    then
        return
    fi

    # (same source)

    # Store the notarization credentials so that we can prevent a UI password dialog
    # from blocking the CI

    echo "Create keychain profile"
    xcrun notarytool store-credentials "notarytool-profile" --apple-id "$PROD_MACOS_NOTARIZATION_APPLE_ID" --team-id "$PROD_MACOS_NOTARIZATION_TEAM_ID" --password "$PROD_MACOS_NOTARIZATION_PWD"

    # We can't notarize an app bundle directly, but we need to compress it as an archive.
    # Therefore, we create a zip file containing our app bundle, so that we can send it to the
    # notarization service

    echo "Creating temp notarization archive"
    ditto -c -k --keepParent "$APP_PATH" "notarization.zip"

    # Here we send the notarization request to the Apple's Notarization service, waiting for the result.
    # This typically takes a few seconds inside a CI environment, but it might take more depending on the App
    # characteristics. Visit the Notarization docs for more information and strategies on how to optimize it if
    # you're curious

    echo "Notarize app"
    xcrun notarytool submit "notarization.zip" --keychain-profile "notarytool-profile" --wait

    # Finally, we need to "attach the staple" to our executable, which will allow our app to be
    # validated by macOS even when an internet connection is not available.
    echo "Attach staple"
    xcrun stapler staple "$APP_PATH" || exit 1
}

sign "Blades of Exile"
sign "BoE Scenario Editor"
sign "BoE Character Editor"