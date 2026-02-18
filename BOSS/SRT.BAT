@echo off
cls
echo Sorting \boss\file1\files.bbs
cd\boss\file1
sort <files.bbs >files.bak
erase files.bbs
copy files.bak files.bbs
echo Sorting \boss\file2\files.bbs
cd\boss\file2
sort <files.bbs >files.bak
erase files.bbs
copy files.bak files.bbs
echo Sorting \boss\file3\files.bbs
cd\boss\file3
sort <files.bbs >files.bak
erase files.bbs
copy files.bak files.bbs
echo Sorting \boss\file4\files.bbs
cd\boss\file4
sort <files.bbs >files.bak
erase files.bbs
copy files.bak files.bbs
echo Sorting \boss\file5\files.bbs
cd\boss\file5
sort <files.bbs >files.bak
erase files.bbs
copy files.bak files.bbs
echo Sorting \boss\file6\files.bbs
cd\boss\file6
sort <files.bbs >files.bak
copy files.bak files.bbs
cd\boss
echo Sort Complete
