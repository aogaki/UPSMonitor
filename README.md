# UPSMonitor
Checking the UPS used by ELIADE team.  First, this uses UPS server cgi to get the UPS status.  After implemented, developer plan to use SNMP (NYI).  The database is influxdb.  Config file is JSON format (settings.json).  

Requirement (listing as package names in Ubuntu20.04)   
**nlohmann-json3-dev** for reading config file.  
**libcurl4-openssl-dev** for fetching data from UPS.  
[influxdb-cpp](https://github.com/orca-zhang/influxdb-cpp) post the UPS status to the database (influsdb).  
And some small tools (e.g. cmake, git)

## Installation (How to make this as a deamon in Ubuntu20.04)  
1. Compile and build UPS_monitor  
1. Edit daemon.sh.  Most probably you need to change the time interval to check UPSs  
1. Making link ups-monitor.service to /etc/systemc/system  
    - **sudo ln -fs /home/eliade/UPSMonitor/ups-monitor.service /etc/systemd/system/**  
1. Register this service
    - **sudo systemctl enable ups-monitor.service**  
1. Start the service  
    - **sudo systemctl start ups-monitor.service**  
1. Cheking the service  
    - **sudo systemctl status ups-monitor**  

If not working, ask Soiciro.  