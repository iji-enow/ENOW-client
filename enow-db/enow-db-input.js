var program = require('commander');
var mongo = require('mongodb');
var Grid = require('gridfs');
var assert = require('assert');
var MongoClient = mongo.MongoClient;
var ObjectId = mongo.ObjectID;
var Binary = mongo.Binary;


program.version('0.0.1')
//.option('-i --_id [id_number]', 'document_id you want to make in mongodb')
.option('-d --document [name]', 'document name you want to make in mongodb')
.option('-f --file [filename]', 'sourece file you want to insert to mongodb ex)\"./helloworld.txt\"')
.parse(process.argv);


program.on('--help', function(){
    console.log('  Examples:');
    console.log('');
    console.log('    $ node enow-db-input.js --help');
    console.log('    $ node enow-db-input.js -h');
    console.log('');
});

if(!program.file)
{
        console.log("error : you must type input file");
        return -1;
}
if(!program.document)
{
        console.log("error : you must type document_name");
        return -1;
}
//if(program._id)
//    console.log("document_id is %j", program._id);



MongoClient.connect('mongodb://localhost/test',function(err,db) {
    console.log('start...');
    var gfs = Grid(db, mongo);
    // file you want to insert to mongodb
    var source = program.file;

    gfs.fromFile({filename: program.document}, source, function (err, file) {
        console.log('saved %s to GridFS file %s', source, file._id);
        var insertDocument = function(db, callback){
            //log파일로 기록함.
            db.collection('logs').insertOne({
                "_id" : file._id,
                "filename" : file.filename,
                "contentType" : file.contentType,
                "length" : file.length,
                "chunkSize" : file.chunkSize,
                "uploadDate" : file.uploadDate,
                "aliases" : file.aliases,
                "metadata" : file.metadata,
                "md5" : file.md5,
                "log" : "[id : " + file._id+"] [filename : "+ file.filename+"] is inserted in test db at "+ file.uploadDate
            },function(err, result){
                assert.equal(err, null);
                console.log("Inserted a document into logs collection.");
                callback();
            });
        };
        assert.equal(null,err);
        insertDocument(db, function(){
            db.close();
        });

    });
});
