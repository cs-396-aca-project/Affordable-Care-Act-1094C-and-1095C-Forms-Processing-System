NET STOP wampapache
.\bin\apache\apache2.2.11\bin\httpd.exe -k uninstall -n wampapache
NET STOP wampmysqld 
.\bin\mysql\mysql5.1.36\bin\mysqld.exe --remove wampmysqld
wampmanager.exe -quit -id={wampserver}