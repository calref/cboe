
exe/bin/%.nib: rsrc/menus/%.xib
	ibtool --compile $@ $<

game-menus: exe/bin/game.nib

pc-menus: exe/bin/pcedit.nib

scen-menus: exe/bin/scenedit.nib

