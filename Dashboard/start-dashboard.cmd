@echo off
rem AXION MU ilerleme panosu - dis erisimle baslatir (http://45.87.120.29:8096/)
rem Yonetici onayli calistirma gerekmez; URL ACL + firewall kurali bir kez eklendi.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0server.ps1" -Published 1
