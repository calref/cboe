
all: Install-OBoE.exe Makefile.mak

Install-OBoE.exe: ..\..\..\pkg\win\main.nsi ..\..\..\pkg\win\data.nsi
	makensis /DMSVC /DRELEASE_DIR=..\..\src\BoE.vsproj\Release ..\..\..\pkg\win\main.nsi

..\..\..\pkg\win\data.nsi: ..\..\..\pkg\win\gen-data.py
	python ..\..\..\pkg\win\gen-data.py ..\Release > ..\..\..\pkg\win\data.nsi

clean:
	del ..\..\..\pkg\win\data.nsi
	del Install-OBoE.exe

rebuild: clean all
