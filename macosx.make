
BOE_CONTENT='exe/Blades of Exile/Blades of Exile.app/Contents'
PC_CONTENT='exe/Blades of Exile/Scenario Editor/Blades of Exile Character Editor.app/Contents'
SCEN_CONTENT='exe/Blades of Exile/Scenario Editor/BoE Scenario Editor.app/Contents'

exe/bin/%.nib: rsrc/menus/%.xib
	ibtool --compile $@ $<

game-menus: exe/bin/game.nib

pc-menus: exe/bin/pcedit.nib

scen-menus: exe/bin/scenedit.nib

frameworks:
	rm -rf exe/fwk # Because otherwise cp complains "can't replace dir with non-dir"
	mkdir -p exe/fwk
	cp -Rfp /Library/Frameworks/sfml-{audio,graphics,system,window}.framework exe/fwk
	cp -Rfp /Library/Frameworks/{ogg,FLAC,freetype}.framework exe/fwk
	cp -Rfp /Library/Frameworks/vorbis{,enc,file}.framework exe/fwk
	cp -Rfp /usr/local/lib/libboost_{system,filesystem,thread}.dylib exe/fwk

game-bundle: game-menus frameworks src/BoE-Info.plist
	mkdir -p $(BOE_CONTENT)/MacOS
	mkdir -p $(BOE_CONTENT)/Contents
	mkdir -p $(BOE_CONTENT)/Resources
	rm -rf $(BOE_CONTENT)/Frameworks
	mkdir -p $(BOE_CONTENT)/Frameworks
	cp -fp exe/bin/boe $(BOE_CONTENT)/MacOS/'Blades of Exile'
	cp -fp src/BoE-Info.plist $(BOE_CONTENT)/Info.plist
	sed -Ei -e 's/\$$\{EXECUTABLE_NAME\}/Blades of Exile/' $(BOE_CONTENT)/Info.plist
	rm -f $(BOE_CONTENT)/PkgInfo
	echo 'APPLblx!' > $(BOE_CONTENT)/PkgInfo
	cp -fp rsrc/icons/mac/BoE.icns $(BOE_CONTENT)/Resources/
	cp -fp rsrc/icons/mac/boegraphics.icns $(BOE_CONTENT)/Resources/
	cp -fp rsrc/icons/mac/boeresources.icns $(BOE_CONTENT)/Resources/
	cp -fp rsrc/icons/mac/boesave.icns $(BOE_CONTENT)/Resources/
	cp -fp rsrc/icons/mac/boesounds.icns $(BOE_CONTENT)/Resources/
	cp -fp exe/bin/game.nib $(BOE_CONTENT)/Resources/
	cp -Rfp exe/fwk/ $(BOE_CONTENT)/Frameworks/

pc-bundle: pc-menus frameworks src/pcedit/BoECharEd-Info.plist
	mkdir -p $(PC_CONTENT)/MacOS
	mkdir -p $(PC_CONTENT)/Contents
	mkdir -p $(PC_CONTENT)/Resources
	rm -rf $(PC_CONTENT)/Frameworks
	mkdir -p $(PC_CONTENT)/Frameworks
	cp -fp exe/bin/pced $(PC_CONTENT)/MacOS/'Blades of Exile Character Editor'
	cp -fp src/pcedit/BoECharEd-Info.plist $(PC_CONTENT)/Info.plist
	sed -Ei -e 's/\$$\{EXECUTABLE_NAME\}/Blades of Exile Character Editor/' $(PC_CONTENT)/Info.plist
	rm -f $(PC_CONTENT)/PkgInfo
	echo 'APPLblxe' > $(PC_CONTENT)/PkgInfo
	cp -fp rsrc/icons/mac/BoECharEd.icns $(PC_CONTENT)/Resources/
	cp -fp exe/bin/pcedit.nib $(PC_CONTENT)/Resources/
	cp -Rfp exe/fwk/* $(PC_CONTENT)/Frameworks/

scen-bundle: scen-menus frameworks src/scenedit/BoEScenEd-Info.plist
	mkdir -p $(SCEN_CONTENT)/MacOS
	mkdir -p $(SCEN_CONTENT)/Contents
	mkdir -p $(SCEN_CONTENT)/Resources
	rm -rf $(SCEN_CONTENT)/Frameworks
	mkdir -p $(SCEN_CONTENT)/Frameworks
	cp -fp exe/bin/scened $(SCEN_CONTENT)/MacOS/'BoE Scenario Editor'
	cp -fp src/scenedit/BoEScenEd-Info.plist $(SCEN_CONTENT)/Info.plist
	sed -Ei -e 's/\$$\{EXECUTABLE_NAME\}/BoE Scenario Editor/' $(SCEN_CONTENT)/Info.plist
	rm -f $(Scen_CONTENT)/PkgInfo
	echo 'APPLBlEd' > $(SCEN_CONTENT)/PkgInfo
	cp -fp rsrc/icons/mac/BoEScenEd.icns $(SCEN_CONTENT)/Resources/
	cp -fp rsrc/icons/mac/boescenario.icns $(SCEN_CONTENT)/Resources/
	cp -fp exe/bin/scenedit.nib $(SCEN_CONTENT)/Resources/
	cp -Rfp exe/fwk/* $(SCEN_CONTENT)/Frameworks/
