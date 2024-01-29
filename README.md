# cubbit-horcrux

Compilation example for VS (from project root directory):  
cmake -S . -B build  
cd build  
msbuild .\CubbitHorcrux.sln /property:Configuration=Release /p:Platform=x64  

Server execution example:  
.\horcrux_server.exe  

Client execution example:  
.\horcrux.exe save -n 3 C:\Temp\tree.jpg  
.\horcrux.exe load f1263439-30c3-4442-aa96-b4a0c622f3be C:\Temp\tree2.jpg  
