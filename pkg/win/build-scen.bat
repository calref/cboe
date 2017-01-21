
set Zip="C:\Program Files\7-Zip\7z.exe"

@echo Packing scenario %2...
if exist %2 del /F %2
if exist %2 exit 1
move %1 scenario
%Zip% a -ttar -so temp.tar scenario | %Zip% a -tgzip -si %2
move scenario %1