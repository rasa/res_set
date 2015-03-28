all:
	VCBuild.exe /nologo res_set.vcproj /rebuild

clean:
	VCBuild.exe /nologo res_set.vcproj /clean
	
upgrade:
	devenv res_set.sln /upgrade

.PHONY:	all clean upgrade

