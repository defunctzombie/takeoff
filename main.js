require('localenv');
require('crash-reporter').start();

var app = require('app');
var path = require('path');
var BrowserWindow = require('browser-window');
var Menu = require('menu');
var EventEmitter = require('events').EventEmitter;

var cwd = process.cwd();

// any command line args are files to try and open
var args = process.argv.slice(2);
var files_to_open = args;

// global reference to avoid getting gc'd until closed
var workspaces = [];

app.on('window-all-closed', function() {
    // On OS X it is common for applications and their menu bar
    // to stay active until the user quits explicitly with Cmd + Q
    if (process.platform != 'darwin') {
        app.quit();
    }
});

// electron has loaded, windows can be created
app.on('ready', function() {
    setup_app_menu();

    if (files_to_open.length === 0) {
        var workspace = Workspace();

        var idx = workspaces.push(workspace) - 1;
        // remove reference to the workspace so it can be GC'd
        workspace.on('closed', function() {
            workspaces.splice(idx, 1);
        });

        return;
    }

    // load each project in a workspace
    files_to_open.forEach(function(file) {
        file = path.resolve(cwd, file);

        var workspace = Workspace();
        workspace.open(file);

        var idx = workspaces.push(workspace) - 1;
        // remove reference to the workspace so it can be GC'd
        workspace.on('closed', function() {
            workspaces.splice(idx, 1);
        });
    });

});

function Workspace() {
    if (!(this instanceof Workspace)) {
        return new Workspace();
    }

    var self = this;

    // must be required after app is ready
    var atom_screen = require('screen');
    var size = atom_screen.getPrimaryDisplay().workAreaSize;

    // this window doesn't do anything other than bootstrap IPC for the app
    // it will launch the other windows to do actual work
    var win = self._window = new BrowserWindow({
        width: size.width,
        height: size.height,
        title: 'controller'
    });

    win.loadUrl('file://' + __dirname + '/views/workspace.html');

    // TODO only in development
    win.openDevTools();

    win.on('closed', function() {
        self._window = null;
        self.emit('closed');
    });
};

Workspace.prototype.__proto__ = EventEmitter.prototype;

Workspace.prototype.open = function(file) {
    var self = this;

    self._window.webContents.on('did-finish-load', function() {
        self._window.webContents.send('open file', file);
    });
};

function setup_app_menu() {
    var template = [{
        label: 'Takeoff',
        submenu: [{
            label: 'About Takeoff',
            selector: 'orderFrontStandardAboutPanel:'
        }, {
            type: 'separator'
        }, {
            label: 'Services',
            submenu: []
        }, {
            type: 'separator'
        }, {
            label: 'Hide Electron',
            accelerator: 'CmdOrCtrl+H',
            selector: 'hide:'
        }, {
            label: 'Hide Others',
            accelerator: 'CmdOrCtrl+Shift+H',
            selector: 'hideOtherApplications:'
        }, {
            label: 'Show All',
            selector: 'unhideAllApplications:'
        }, {
            type: 'separator'
        }, {
            label: 'Quit',
            accelerator: 'CmdOrCtrl+Q',
            selector: 'terminate:'
        }]
    }, {
        label: 'File',
        submenu: [{
            label: 'Open Project',
            accelerator: 'CmdOrCtrl+O',
            selector: 'undo:'
        }, {
            label: 'Save',
            accelerator: 'CmdOrCtrl+S',
            selector: 'redo:'
        }, {
            label: 'Save As',
            accelerator: 'Shift+CmdOrCtrl+S',
            selector: 'redo:'
        }]
    }, {
        label: 'Tools',
        submenu: [{
            label: 'Measure Scale',
        }]
    }, {
        label: 'Debug',
        submenu: [ {
            label: 'Reload',
            accelerator: 'CmdOrCtrl+R',
            click: function() { BrowserWindow.getFocusedWindow().reload(); }
        }, {
            label: 'Toggle DevTools',
            accelerator: 'Alt+CmdOrCtrl+I',
            click: function() { BrowserWindow.getFocusedWindow().toggleDevTools(); }
        }]
    }, {
        label: 'Window',
        submenu: [ {
            label: 'Minimize',
            accelerator: 'CmdOrCtrl+M',
            selector: 'performMiniaturize:'
        }, {
            label: 'Close',
            accelerator: 'CmdOrCtrl+W',
            selector: 'performClose:'
        }, {
            type: 'separator'
        }, {
            label: 'Bring All to Front',
            selector: 'arrangeInFront:'
        }]
    }];

    var menu = Menu.buildFromTemplate(template);
    Menu.setApplicationMenu(menu);
}

