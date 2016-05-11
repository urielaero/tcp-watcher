var tcp = module.exports = {},
    net = require('net'),
    http = require('http'),
    url = require('url'),
    sh = require('./sh'),
    ids = {};

tcp.sensors = {};

var server = net.createServer(function(socket) {
    var ip = socket.remoteAddress;
    socket.on('data', function(data) {
        var data = data.toString(),
        info = data.split(',');
        var id = info[0],
        status = info[1] || '';
        status = status.trim();
        if (ids[id] && ids[id].socket && ids[id].status != status && id > 1) {
            changeStatusNotify(status, id);
        }
        ids[id] = {
            socket: socket,
            status: status
        };
	console.log("conectado id", id);
	console.log("recv", info);
    });
}).listen(1338, '0.0.0.0');
console.log('server tcp on: ', 1338);

http.createServer(function(req, res) {
    var query = url.parse(req.url, true).query;
    if(query.id && query.status) {
        if (query.status == 'read') {
            if (ids[query.id]) {
                res.write(ids[query.id].status);
            } else {
                res.write('0');
            }
        } else {
            setStatus(query.id, query.status);
            res.write("ok");
        }
    } else {
        res.write("failed");
    }
    res.end();
}).listen(1337);
console.log("http server on: ", 1337);

function setStatus(id, status) {
    console.log("set ", id, "to: ", status);
    if (ids[id] && ids[id].socket) {
        ids[id].socket.write("@"+status+"\r\n");
        ids[id].status = status;
    }
}

var lock = false,
active = "event_activate.sh",
unactive = "event_no.sh";
function changeStatusNotify(status, id) {
    if (!lock && id) {
        lock = true;
        var script = "/home/others/";
        console.log("run command", status);
        if(status == '0') {	
            script += unactive;
        } else {
            script += active;
        }

        var idName = '';
        if (id == 2) {
            idName = 'dos';
        } else {
            idName = 'tres';
        }

        sh.run('docker', ['exec', '-t', 'asterisk', 'bash', script, idName, id], function() {
            console.log("on exit");
            //lock = false;
        });

        setTimeout(function() {
            lock = false;
        }, 3000);
    }
}
