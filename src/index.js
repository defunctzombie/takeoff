var remote = require('remote');
var ipc = require('ipc');

var Menu = remote.require('menu');

var template = [
  {
    label: 'Takeoff',
    submenu: [
      {
        label: 'About Takeoff',
        selector: 'orderFrontStandardAboutPanel:'
      },
      {
        type: 'separator'
      },
      {
        label: 'Services',
        submenu: []
      },
      {
        type: 'separator'
      },
      {
        label: 'Hide Electron',
        accelerator: 'CmdOrCtrl+H',
        selector: 'hide:'
      },
      {
        label: 'Hide Others',
        accelerator: 'CmdOrCtrl+Shift+H',
        selector: 'hideOtherApplications:'
      },
      {
        label: 'Show All',
        selector: 'unhideAllApplications:'
      },
      {
        type: 'separator'
      },
      {
        label: 'Quit',
        accelerator: 'CmdOrCtrl+Q',
        selector: 'terminate:'
      },
    ]
  },
  {
    label: 'File',
    submenu: [
      {
        label: 'Open Project',
        accelerator: 'CmdOrCtrl+O',
        selector: 'undo:'
      },
      {
        label: 'Save',
        accelerator: 'CmdOrCtrl+S',
        selector: 'redo:'
      },
      {
        label: 'Save As',
        accelerator: 'Shift+CmdOrCtrl+S',
        selector: 'redo:'
      }
    ]
  },
  {
    label: 'Tools',
    submenu: [
      {
        label: 'Measure Scale',
      },
    ]
  },
  /*
  {
    label: 'View',
    submenu: [
      {
        label: 'Reload',
        accelerator: 'CmdOrCtrl+R',
        click: function() { remote.getCurrentWindow().reload(); }
      },
      {
        label: 'Toggle DevTools',
        accelerator: 'Alt+CmdOrCtrl+I',
        click: function() { remote.getCurrentWindow().toggleDevTools(); }
      },
    ]
  },
  */
  {
    label: 'Window',
    submenu: [
      {
        label: 'Minimize',
        accelerator: 'CmdOrCtrl+M',
        selector: 'performMiniaturize:'
      },
      {
        label: 'Close',
        accelerator: 'CmdOrCtrl+W',
        selector: 'performClose:'
      },
      {
        type: 'separator'
      },
      {
        label: 'Bring All to Front',
        selector: 'arrangeInFront:'
      }
    ]
  },
  /*
  {
    label: 'Help',
    submenu: []
  }
  */
];

var menu = Menu.buildFromTemplate(template);

Menu.setApplicationMenu(menu);

ipc.on('open file', function(message) {
  console.log('OPEN FILE', message);
});
