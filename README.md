# UPSMonitor
Check the UPS used by ELIADE team.  First, this uses cgi to get the UPS status.  After implemented, using SNMP.  The database is influxdb.  Config file is JSON format.  

Requirement
nlohmann-json-dev for reading config file  
curl for post the UPS status to the database (influsdb)
