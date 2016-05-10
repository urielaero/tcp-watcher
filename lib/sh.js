var spawn = require('child_process').spawn;

var sh = module.exports = {};

sh.run = function(command, ars, done) {
    ars = ars || [];
    var sp = spawn(command, ars);

    sp.stdout.on('data', pr);
    sp.stderr.on('data', pr);

    sp.on('exit', function(code) {
        done();
    });
};

function pr(data) {
    console.log(data.toString());
}
