
all: Release\Install-OBoE.exe

Release\Install-OBoE.exe: main.nsi data.nsi
	makensis main.nsi

data.nsi: gen-data.py
	python gen-data.py > data.nsi

clean:
	del data.nsi
	del Release\Install-OBoE.exe

rebuild: clean all
