# ENOW - db

ENOW database with MongoDB


## Settings

###Windows
1. Download MongoDB  [link](https://www.mongodb.com/download-center?jmp=nav#community)

2. type ```$mkdir C:\data\db``` in cmd.

3. type ```$mkdir C:\mongodb\log``` in cmd.

4. make mongodb.cfg file and type below. put it in C:\mongodb
   ```
    ##Which IP address(es) mongod should bind to.
    bind_ip = 127.0.0.1

    ##Which port mongod should bind to.
    port = 27017

    ##I set this to true, so that only critical events and errors are logged.
    quiet = true

    ##store data here
    dbpath=C:\data\db

    ##The path to the log file to which mongod should write its log messages.
    logpath=C:\mongodb\log\mongo.log

    ##I set this to true so that the log is not overwritten upon restart of mongod.
    logappend = true

    ##log read and write operations
    diaglog=3

    ##It ensures write durability and data consistency much as any journaling scheme would be expected to do.
    ##Only set this to false if you don’t really care about your data (or more so, the loss of it).
    journal = true
   ```

5. start MongoDB server with command ```$mongod.exe --config d:\mongodb\mongo.config``` in cmd.

6. start MongoDB with ```$mongo``` command.
