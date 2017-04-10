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
        if (!data || !data.name || typeof data.name != 'string' || data.name.length < 1 || data.name.length > 32 || data.name.match(/^[\w\s-_]+$/i) === null ||
            !data.ct || !data.ct.visible || !data.ct.invisible || typeof data.ct.visible != 'object' || typeof data.ct.invisible != 'object' ||
            !data.t || !data.t.visible || !data.t.invisible || typeof data.t.visible != 'object' || typeof data.t.invisible != 'object' ||
            data.ct.visible.length != 3 || data.ct.invisible.length != 3 ||
            !helper.IsValidRgbColor(data.ct.visible) || !helper.IsValidRgbColor(data.ct.invisible) ||
            data.t.visible.length != 3 || data.t.invisible.length != 3 ||
            !helper.IsValidRgbColor(data.t.visible) || !helper.IsValidRgbColor(data.t.invisible)
        ) return console.log('[build] received invalid data: %s', util.inspect(data));

	fs.readFile(__dirname + '/buildcount.txt', (err, data) => {
	    if (err) return;
            fs.writeFile(__dirname + '/buildcount.txt', parseInt(data.toString()) + 1, 'utf8', err => {
	        if (err) return;
	    });
            io.emit('buildcount', { count: parseInt(data) + 1 });
	});

        var buildname = sha256(data.name + '-' + moment().unix() + '-' + randomstring.generate());

        ncp(__dirname + '/../cheat', __dirname + '/cache/' + buildname, err => {
            if (err) return console.log('[build] %s (%s): error while copying source', data.name, buildname);
            console.log('[build] %s (%s): copied source', data.name, buildname)

            console.log('[build] %s (%s): replacing name and colors', data.name, buildname);

            fs.readFile(__dirname + '/cache/' + buildname + '/MetaInfo.h', (err, metainfo) => {
                if (err) return console.log('[build] %s (%s): failed to open MetaInfo.h', data.name, buildname);

                var final = metainfo.toString()
                    .split('{NAME}').join(data.name)
                    .split('{CTVIS}').join(vsprintf('%d, %d, %d', data.ct.visible))
                    .split('{CTINVIS}').join(vsprintf('%d, %d, %d', data.ct.invisible))
                    .split('{TVIS}').join(vsprintf('%d, %d, %d', data.t.visible))
                    .split('{TINVIS}').join(vsprintf('%d, %d, %d', data.t.invisible));

                fs.writeFile(__dirname + '/cache/' + buildname + '/MetaInfo.h', final, 'utf8', err => {
                    if (err) console.log('[build] %s (%s): failed to save MetaInfo.h', data.name, buildname);

                    console.log('[build] %s (%s): starting build', data.name, buildname);
                    var build = child.spawn('msbuild', [__dirname + '/cache/' + buildname + '/AYYWARE CSGO.vcxproj', '/p:Configuration=Release'])
                    build.on('close', code => {
                        console.log('[build] %s (%s): build finished', data.name, buildname);
                        var dllname = sha256(buildname) + '.dll';
                        fs.move(
                            __dirname + '/cache/' + buildname + '/Release/ayyware.dll',
                            __dirname + '/bin/' + dllname,
                            err => {
                                if (err) return console.log('[build] %s (%s): renaming DLL failed.', data.name, buildname);

                                console.log('[build] %s (%s): deleting source', data.name, buildname);

				try {
                                    fs.removeSync(__dirname + '/cache/' + buildname);
				} catch (e) {}

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
    });
    socket.on('buildcount', () => {
        fs.readFile(__dirname + '/buildcount.txt', (err, data) => {
            if (err) return;
            socket.emit('buildcount', { count: data.toString() });
        });
    });
});
