var remote = require('remote');
var path = require('path');
var assert = require('assert');
var debug = require('debug')('takeoff:workspace');

var xml2js = remote.require('xml2js');
var fs = remote.require('fs');

var ShapeFactory = require('../shapes/ShapeFactory');

function Project(info) {
    if (!(this instanceof Project)) {
        return new Project(info);
    }

    var self = this;
    info = info || {};

    self._version = info.version || 1.0;
    self._pages = info.pages || [];
};

// get a list of project pages
Project.prototype.get_pages = function() {
    var self = this;
    return self._pages;
};

Project.prototype.set_filename = function(filepath) {
    var self = this;
};

Project.prototype.save = function(cb) {
    /*
       var builder = new xml2js.Builder();
       var xml = builder.buildObject(result);
       console.log(xml);
    */
};

Project.load = function load_project(filepath, cb) {
    // load and parse file
    Project._load(filepath, function(err, info) {
        if (err) {
            return cb(err);
        }

        debug('loaded project info (%s pages)', info.pages.length);

        // assert that documents matches pages
        // load pdfs for file? maybe load these on demand?
        var prj = new Project(info);
        return cb(null, prj);
    });
};

// load and parse xml file into basic json
Project._load = function(filepath, cb) {
    var dirname = path.dirname(filepath);

    fs.readFile(filepath, 'utf-8', function(err, data) {
        if (err) {
            return cb(err);
        }

        xml2js.parseString(data, function(err, xml_obj) {
            if (err) {
                return cb(err);
            }

            var documents = [];
            var pages = [];

            var root = xml_obj.takeoff;

            root.documents.forEach(function(docs) {
                docs.document.forEach(function(document) {
                    documents.push({
                        ref: document.$.ref
                    });
                });
            });

            root.pages.forEach(function(pgs) {
                pgs.page.forEach(function(page) {
                    var shapes = page.shape.map(function(shape) {

                        // initialize shape from shape factory
                        return ShapeFactory.create(shape.$.type, {
                            color: shape.$.color,
                            points: shape.point.map(function(point) {
                                return {
                                    x: point.$.x,
                                    y: point.$.y
                                }
                            })
                        });
                    });

                    pages.push({
                        scale: page.$.scale,
                        rotation: page.$.rotation,
                        shapes: shapes
                    });
                });
            });

            assert(documents.length === pages.length);
            documents.forEach(function(document, idx) {
                pages[idx].file = path.resolve(dirname, document.ref);
                pages[idx].name = document.ref;
            });

            var res = {
                version: root.$.version,
                pages: pages
            };

            cb(null, res);
        });
    });
};

module.exports = Project;
