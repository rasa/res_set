all:
	MSBuild.exe /nologo res_set.sln /p:Configuration=Debug
	MSBuild.exe /nologo res_set.sln /p:Configuration=Release

clean:
	MSBuild.exe /nologo res_set.sln /p:Configuration=Debug   /t:clean
	MSBuild.exe /nologo res_set.sln /p:Configuration=Release /t:clean

upgrade:
	devenv res_set.sln /upgrade
	
.PHONY:	all clean upgrade


