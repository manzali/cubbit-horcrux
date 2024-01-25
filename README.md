# cpp-project-template

For VS2017 (default msbuild in path)  
cd build  
cmake -G "Visual Studio 15 2017" -S .. -B .  
msbuild .\MyProject.sln /property:Configuration=Release /p:Platform=x64  


For VS2022 (default compiler for cmake)
cd build  
cmake -S .. -B .  
&"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\msbuild.exe" .\MyProject.sln /property:Configuration=Release /p:Platform=x64  
