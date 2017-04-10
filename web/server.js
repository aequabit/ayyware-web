const config = require('./config'),
    helper = require('./helper'),
    express = require('express'),
    app = express(),
    util = require('util'),
    fs = require('fs-extra'),
    randomstring = require('randomstring'),
    moment = require('moment'),
    ncp = require('ncp'),
    sha256 = require('sha256'),
    sprintf = require('sprintf-js').sprintf,
    vsprintf = require('sprintf-js').vsprintf,
    child = require('child_process');

/**
 * Express setup.
 */
app.use('/static', express.static(__dirname + '/static'));
//app.use('/bin', express.static(__dirname + '/bin'));

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/templates/index.html');
});

app.get('/bin/:name', (req, res) => {
    if (!fs.existsSync(__dirname + '/bin/' + req.params.name)) {
        return res.redirect('/');
    }
    res.sendFile(__dirname + '/bin/' + req.params.name);
    console.log('[bin] %s requested %s', (req.headers['x-forwarded-for'] || req.connection.remoteAddress).replace('::ffff:', ''), req.params.name);
});

var server = app.listen(config.http.port, () => {
    console.log('[http] listening on %d', config.http.port);
});

/**
 * Socket.io setup.
 */
const io = require('socket.io').listen(server);

io.on('connection', socket => {
    //console.log('[socket] client connected');
    socket.on('build', data => {
        if (!data || !data.name || typeof data.name != 'string' || data.name.match(/^[a-z0-9]+$/i) === null ||
            !data.ct || !data.ct.visible || !data.ct.invisible || typeof data.ct.visible != 'object' || typeof data.ct.invisible != 'object' ||
            !data.t || !data.t.visible || !data.t.invisible || typeof data.t.visible != 'object' || typeof data.t.invisible != 'object' ||
            data.ct.visible.length != 3 || data.ct.invisible.length != 3 ||
            !helper.IsValidRgbColor(data.ct.visible) || !helper.IsValidRgbColor(data.ct.invisible) ||
            data.t.visible.length != 3 || data.t.invisible.length != 3 ||
            !helper.IsValidRgbColor(data.t.visible) || !helper.IsValidRgbColor(data.t.invisible)
        ) return console.log('[build] received invalid data: %s', util.inspect(data));

        var buildname = data.name + '-' + moment().unix() + '-' + randomstring.generate();

        ncp(__dirname + '/../cheat', __dirname + '/cache/' + buildname, err => {
            if (err) return console.log('[build] %s: error while copying source', buildname);
            console.log('[build] %s: copied source', buildname)

            console.log('[build] %s: replacing name and colors', buildname);

            fs.readFile(__dirname + '/cache/' + buildname + '/MetaInfo.h', (err, metainfo) => {
                if (err) return console.log('[build] %s: failed to open MetaInfo.h', buildname);

                var final = metainfo.toString()
                    .split('{NAME}').join(data.name)
                    .split('{CTVIS}').join(vsprintf('%d, %d, %d', data.ct.visible))
                    .split('{CTINVIS}').join(vsprintf('%d, %d, %d', data.ct.invisible))
                    .split('{TVIS}').join(vsprintf('%d, %d, %d', data.t.visible))
                    .split('{TINVIS}').join(vsprintf('%d, %d, %d', data.t.invisible));

                fs.writeFile(__dirname + '/cache/' + buildname + '/MetaInfo.h', final, 'utf8', err => {
                    if (err) console.log('[build] %s: failed to save MetaInfo.h', buildname);

                    console.log('[build] %s: starting build', buildname);
                    var build = child.spawn('msbuild', [__dirname + '/cache/' + buildname + '/AYYWARE CSGO.vcxproj', '/p:Configuration=Release'])
                    build.on('close', code => {
                        console.log('[build] %s: build finished', buildname);
                        var dllname = sha256(buildname) + '.dll';
                        fs.move(
                            __dirname + '/cache/' + buildname + '/Release/ayyware.dll',
                            __dirname + '/bin/' + dllname,
                            err => {
                                if (err) return console.log('[build] %s: renaming DLL failed.');

                                console.log('[build] deleting source');
                                fs.removeSync(__dirname + '/cache/' + buildname);

                                socket.emit('build', {
                                    url: '/bin/' + dllname
                                });
                                setTimeout(() => {
                                    fs.unlinkSync(__dirname + '/bin/' + dllname);
                                }, 15000);
                            });
                    });
                });
            })
        });

        //child.spawn('msbuild', [])
    });
});
