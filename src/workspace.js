var path = require('path');
var remote = require('remote');

var remote = require('remote');
var ipc = require('ipc');
var path = require('path');
var debug = require('debug')('takeoff:workspace');
var assert = require('assert');
var domify = require('domify');

var fs = remote.require('fs');

var Project = require('./models/Project');

var ZoomInTool = require('./tools/ZoomIn');
var ZoomOutTool = require('./tools/ZoomOut');
var ZoomWindowTool = require('./tools/ZoomWindow');
var PointerTool = require('./tools/Pointer');
var AreaTool = require('./tools/Area');

// local dommit used due to browser field usage with dommit
var dommit = require('../lib/dommit');

function Workspace() {
    if (!(this instanceof Workspace)) {
        return new Workspace();
    }

    var self = this;
    self._project = undefined;

    var tmpl = fs.readFileSync(__dirname + '/tmpl/workspace.html', 'utf8');
    self.view = dommit(tmpl, {}, {
        bindings: {},
        delegate: self
    });

    self._active_tool = undefined;
}

// set the project for the workspace
Workspace.prototype.project = function(prj) {
    var self = this;

    // TODO if we have project ask to save?

    self._prj = prj;
    var pages = prj.get_pages();

    self.view.set('pages', pages);
    if (pages.length > 0) {
        self._show_page(pages[0]);
    }
};

Workspace.prototype.init = function() {
    var self = this;

    // setup tools
    self._add_tool(ZoomInTool);
    self._add_tool(ZoomOutTool);
    self._add_tool(ZoomWindowTool);
    //self._add_tool(ZoomFitTool);
    self._add_separator();
    //add_tool(RotateLeftTool);
    //add_tool(RotateRightTool);
    self._add_separator();
    self._add_tool(PointerTool);
    self._add_tool(AreaTool);
    //add_tool(LineTool);
    //add_tool(CountTool);
    //add_tool(CheckTool);
    self._add_separator();
    // TODO group tool (icon-group)
    // TODO color picker (icon-color-picker)

};

Workspace.prototype._add_tool = function() {
    var self = this;

    var tmpl = '<button class="button icon-{{class}} {{active ? \'active\' : \'\'}}" alt="{{tooltip}}" on-click="clicked"></button>';
    var tool_view = dommit(tmpl, {
        class: Tool.class,
        tooltip: Tool.tip
    }, {
        delegate: {
            clicked: function() {
                if (active_view) {
                    active_view.set('active', false);
                }

                active_view = tool_view;
                active_view.set('active', true);
                console.log('active:', Tool.tip);
                self._active_tool = new Tool(canvas_params);

                // TODO remove listener on tool change
                self._active_tool.on('shape', function(shape) {
                    // TODO add to current page
                    //shapes.push(shape);
                });
            }
        }
    });

    tool_view.appendTo(view.el.querySelector('.tools'));
};

Workspace.prototype._add_separator = function() {
    var self = this;
    self.view.el.querySelector('.tools')
        .appendChild(domify('<div class="toolbar-separator"></div>'));
};

// initialize the drawing space
Workspace.prototype._init_drawing = function() {
};

// sets the active page for the workspace
Workspace.prototype._show_page = function(page) {
    var self = this;

    self._active_page = page;

    // load PDF
    // load shapes

    debug('loading page %s', page.file);
    PDFJS.getDocument(page.file)
    .then(function(pdf) {
        // TODO what if PDF has more than one page?
        return pdf.getPage(1);
    })
    .then(function(page) {
        self._render_page(page);
    }).catch(function(err) {
        throw err;
        //console.error(err);
    });
};

Workspace.prototype.on_change_page = function(ev, model) {
    var page_id = ev.target.value;
    var pages = view.get('pages');

    var found = undefined;
    pages.forEach(function(page) {
        if (page.file === page_id) {
            found = page;
        }
    });

    if (!found) {
        return;
    }

    self._show_page(found);
};

Workspace.prototype.appendTo = function(el) {
    var self = this;
    self.view.appendTo(el);
};

var canvas_params = {
    zoom_factor: 1.0,
    off: {
        x: 0,
        y: 0
    }
};

Workspace.prototype._render_page = function(page) {
    var self = this;

    var view = self.view;

    var drawing = view.el.querySelector('#drawing');
    var canvas = view.el.querySelector('#canvas-pdf');
    var shape_canvas = view.el.querySelector('#canvas-shapes');
    var context = canvas.getContext('2d');
    var shapes_ctx = shape_canvas.getContext('2d');

    var scaling_factor = 0;

    var dim = page.getViewport(1.0);

    var width_in = dim.width / 72;
    var height_in = dim.height / 72;

    console.log('dimensions (in): %s x %s', width_in, height_in);

    // 72 DPI
    console.log(dim);

    console.log(canvas.clientWidth, canvas.clientHeight);

    var scale_w = canvas.clientWidth / dim.width;
    console.log(scale_w);
    //var scale_h = canvas.clientHeight / dim.height;

    //scale = 0.25;
    //var viewport = page.getViewport(scale);
    var off = canvas_params.off;

    var rendering = false;
    var viewport = undefined;

    function render() {
        if (rendering) {
            return;
        }

        rendering = true;

        var scale = scale_w * canvas_params.zoom_factor;
        viewport = new PDFJS.PageViewport(page.view, scale, page.rotate, 0, 0);

        // multiply value by scaling factor to turn inches -> pixel location
        // used when drawing shapes
        scaling_factor = viewport.width / width_in;

        console.log('scale', scale);
        console.log('viewport', viewport);

        canvas.style.left = off.x + 'px';
        canvas.style.top = off.y + 'px';
        canvas.width = viewport.width;
        canvas.height = viewport.height;

        shape_canvas.style.left = off.x + 'px';
        shape_canvas.style.top = off.y + 'px';
        shape_canvas.width = viewport.width;
        shape_canvas.height = viewport.height;

        var renderContext = {
            canvasContext: context,
            viewport: viewport
        };

        if (self._active_tool && self._active_tool.render) {
            var ctx = shapes_ctx;
            ctx.clearRect(0, 0, shape_canvas.width, shape_canvas.height);
            self._active_tool.render(shapes_ctx);
        }
        window.requestAnimationFrame(function() {
            redraw_shapes(shapes, scaling_factor, shapes_ctx);
        });

        var render_task = page.render(renderContext);
        render_task.then(function() {
            rendering = false;
        }).catch(function(err) {
            console.error(err);
        });
    }

    render();

    drawing.addEventListener('dblclick', function(ev) {
        //ev.x, ev.y
        //turn into locations on canvas
        console.log('dbclick', ev);
    });

    window.addEventListener('keyup', function(ev) {
        if (ev.which === 27 /* escape */) {
            // finish current shape (tool done)
            if (self._active_tool && self._active_tool.done) {
                self._active_tool.done();
            }
        }
    });

    // TODO click and drag needs to edit active shape

    drawing.addEventListener('click', function(ev) {
        //ev.x, ev.y
        //turn into locations on canvas
        console.log('click', ev);

        var x = ev.offsetX;
        var y = ev.offsetY;

        //viewport may be bigger now
        //viewport.width is still same amount of width_in tho
        var x_in = width_in / viewport.width * x;
        var y_in = height_in / viewport.height * y;

        console.log('(in) %s %s', x_in, y_in);

        // TODO convert x,y into page space
        // when rendering tho shapes draw as if on page space
        // so we might need to reverse transform

        if (self._active_tool) {
            var point = {
                x: x_in,
                y: y_in
            };

            self._active_tool.add_point(point);
            // dirty, render
            render();
        }

        // try to select a shape point that is closest
        shapes.forEach(function(shape) {
            return;
            shape.nearest_point({
                x: x_in,
                y: y_in
            });
        })
    });

    // if using select tool, then we want to select shapes
    shape_canvas.addEventListener('click', function(ev) {
        window.requestAnimationFrame(function() {
            redraw_shapes(shapes, scaling_factor, shapes_ctx);
        });
    });

    var start_pos = undefined;
    var start_off = undefined;
    drawing.addEventListener('mousedown', function(ev) {
        start_off = {
            x: off.x,
            y: off.y
        };
        start_pos = {
            x: ev.clientX,
            y: ev.clientY
        };
    });

    drawing.addEventListener('mouseup', function(ev) {
        start_pos = undefined;
    });

    drawing.addEventListener('blur', function(ev) {
        start_pos = undefined;
    });

    drawing.addEventListener('scroll', function(ev) {
        start_pos = undefined;
    });

    drawing.addEventListener('mousemove', function(ev) {
        if (!start_pos) {
            return;
        }

        //console.log(canvas.style.left, canvas.style.top);

        var pos = {
            x: ev.clientX,
            y: ev.clientY
        };

        var diff = {
            x: pos.x - start_pos.x,
            y: pos.y - start_pos.y
        };

        off.x = start_off.x + diff.x;
        off.y = start_off.y + diff.y;
        canvas.style.left = off.x + 'px';
        canvas.style.top = off.y + 'px';
        shape_canvas.style.left = off.x + 'px';
        shape_canvas.style.top = off.y + 'px';
    });
};

function redraw_shapes(shapes, scaling_factor, ctx) {
    shapes.forEach(function(shape) {
        // TODO when drawing each shape, need to turn inches coordinates
        // into screen pixel coordinates
        // using ctx.scale is not good
        // since all the lines, dots, etc will scale as well
        shape.render(shapes_ctx, scaling_factor);
    });
}

//var dialog = remote.require('dialog');
//console.log(dialog.showOpenDialog({ properties: [ 'openFile' ]}));

// rotation
// 0 = no rotation
// 1 = 90 (clockwise)
// 2 = 180
// 3 = 270


// takeoff version=1.0
// documents
//    document ref=./relative/path/to/file
//
// pages
//    page scale="" rotation = ""
//      shape type color sibling
//         point
//         point
//      shape type color sibling
//         point
//    page
//
//    1" = <scale> feet

var workspace = Workspace();
workspace.appendTo(document.body);

ipc.on('open file', function(file) {
    debug('open file', file);

    /*
    if (active_project && active_project.is_dirty()) {
        // ask before loading new project?
    }
    */

    Project.load(file, function(err, project) {
        if (err) {
            return; // TODO
        }

        workspace.project(project);
    });
});

