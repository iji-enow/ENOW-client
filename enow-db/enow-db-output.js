var program = require('commander');
var mongo = require('mongodb');
var Grid = require('gridfs');
var MongoClient = mongo.MongoClient;
var ObjectId = mongo.ObjectID;
var Binary = mongo.Binary;


program.version('0.0.1')
.option('-i --_id [id_number]', 'document_id you want to download')
.option('-f --file [filename]', 'sourece file you want to download from mongodb ex) \"./helloworld.txt\"')
.option('-s --show', 'show list of logs')
.parse(process.argv);

program.on('--help', function()
{
    console.log('  Examples:');
    console.log('');
    console.log('    $ node enow-db-output.js --help');
    console.log('    $ node enow-db-output.js -h');
    console.log('');
});





MongoClient.connect('mongodb://localhost/test',function(err,db)
{

    console.log('start...');
    var gfs = Grid(db, mongo);
    //filename of destination
    var target = program.file;

    if(!program.show)
    {
        // error when user doesn't type filename
        if(!program.file)
        {
                console.log("error : type filename you want to download");
                return -1;
        }
        // download file from mongodb
        gfs.toFile({_id: program._id}, target, function (err)
        {
            if(err)
            {
                console.log("no id exists");
            }
            console.log('wrote file %s to %s', program._id, target);
            db.close();
        });
    }
    else
    {
        // show logs
        var collection = db.collection('logs');
        collection.find({}, {"_id":0, "log":1}).limit(50).toArray(function(err,result){
            if(err)
            {
                console.log(err);
            }
            else if(result.length)
            {
                console.log("found : ", result);
            }
            else
            {
                console.log("no log(s) found");
            }
            db.close();
        });
    }

});
