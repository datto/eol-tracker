/* This file is part of eol-tracker.
 * 
 * Copyright © 2020 Datto, Inc.
 * Author: Dakota Williams <drwilliams@datto.com>
 * 
 * Licensed under the GNU Lesser General Public License Version 3
 * Fedora-License-Identifier: LGPLv3+
 * SPDX-2.0-License-Identifier: LGPL-3.0+
 * SPDX-3.0-License-Identifier: LGPL-3.0-or-later
 * 
 * eol-tracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * eol-tracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with eol-tracker.  If not, see <https://www.gnu.org/licenses/>.
 */

import Timeline from 'vis-timeline';
import DataSet from 'vis-data';

function versionCompare(v1, v2, options) {
    var lexicographical = options && options.lexicographical,
        zeroExtend = options && options.zeroExtend,
        v1parts = v1.split('.'),
        v2parts = v2.split('.');

    function isValidPart(x) {
        return (lexicographical ? /^\d+[A-Za-z]*$/ : /^\d+$/).test(x);
    }

    if (!v1parts.every(isValidPart) || !v2parts.every(isValidPart)) {
        return NaN;
    }

    if (zeroExtend) {
        while (v1parts.length < v2parts.length) v1parts.push("0");
        while (v2parts.length < v1parts.length) v2parts.push("0");
    }

    if (!lexicographical) {
        v1parts = v1parts.map(Number);
        v2parts = v2parts.map(Number);
    }

    for (var i = 0; i < v1parts.length; ++i) {
        if (v2parts.length == i) {
            return 1;
        }

        if (v1parts[i] == v2parts[i]) {
            continue;
        }
        else if (v1parts[i] > v2parts[i]) {
            return 1;
        }
        else {
            return -1;
        }
    }

    if (v1parts.length != v2parts.length) {
        return -1;
    }

    return 0;
};

function genTimelineOptions(windowWidthYears, monthScale, select, sortFunc) {
    var startDate = new Date();
    startDate.setFullYear(startDate.getFullYear() - windowWidthYears/2);
    var endDate = new Date();
    endDate.setFullYear(endDate.getFullYear() + windowWidthYears/2);
    return {
        orientation: 'top',
        order: sortFunc,
        /*
        stack: false,
        stackSubgroups: false,
        ,
        */
        selectable: select,
        zoomable: false,
        timeAxis: {
            scale: 'month',
            step: monthScale
        },
        start: startDate,
        end: endDate,
    }
}

export function updateDepTimeline(target) {
    var xmlreq = new XMLHttpRequest();
    xmlreq.onload = function () {
        if (this.status == 200) {
            var info = JSON.parse(this.responseText);
            // get os names for subgroups
            var oses = [];
            info.forEach(function (element) {
                var os = element.os;
                var osObj = oses.find(function (e) {
                    return e === os;
                });
                if (osObj === undefined) {
                    oses.push(os);
                }
            });

            // get project names for groups
            var groupObjs = new DataSet();
            var groupCount = 1;
            info.forEach(function (element) {
                var group = element.name;
                var groupObj = groupObjs.get().find(function (e) {
                    return e.content === group;
                });
                if (groupObj === undefined) {
                    var parent = {
                        id: groupCount,
                        content: group,
                        //name: '',
                        nestedGroups: [groupCount + 1],
                    };

                    /*
                    oses.forEach(function (os) {
                        groupObjs.push({
                            id: groupCount,
                            content: os,
                            name: group,
                        });
                        parent.nestedGroups.push(groupCount);
                        groupCount = groupCount + 1;
                    });
                    */

                    groupObjs.add([parent, {
                        id: groupCount + 1,
                        content: '',
                    }]);

                    groupCount = groupCount + 2;
                }
            });

            // put all the packages into groups
            var stuff = new DataSet();
            var count = 1;
            info.forEach(function (element) {
                var gid = groupObjs.get().find(function (e) {
                    //return e.content === element.os && e.name === element.name;
                    return e.content === element.name;
                }).id + 1;

                var descr = element.found + " " + element.version + " " + element.os + " " + element.os_version;
                var obj = {
                    dataObj:    element,
                    id:         count,
                    content:    descr,
                    start:      element.start,
                    end:        element.end,
                    group:      gid,
                    className:  element.os,
                    title:      descr,
                };
                stuff.add(obj);
                count = count + 1;
            });
            var container = document.getElementById('visualization');
            var timeline = new Timeline(container, stuff, groupObjs, genTimelineOptions(4, 3, false, (a, b) => {
                //return a.dataObj.version < b.dataObj.version;
                var result = versionCompare(a.dataObj.version, b.dataObj.version, { zeroExtend: true });
                if (result === 0) {
                    return a.content > b.content;
                }
                return result;
            }));
            document.getElementById('loading-spinner').style.display = 'none';
        }
    };
    xmlreq.open("GET", "/data?collection=" + target, true);
    xmlreq.send();
}

function clamp(x) {
    if (x > 1.0) {
        return 1.0;
    }
    if (x < 0.0) {
        return 0.0;
    }
    return x;
}

function colorScale(sev, max, lightness) {
    var hue=((1-clamp(sev/max))*120).toString(10);
    return "hsl(" + hue + ",100%," + lightness + "%)";
}

var supportClasses = {
    0: "fullSupport",
    1: "noupdatesSupport",
    2: "deadSupport"
}

export function updateIssueTimeline(collection, timelineObj) {
    var xmlreq = new XMLHttpRequest();
    xmlreq.onload = function () {
        if (this.status == 200) {
            var info = JSON.parse(this.responseText);

            var max = 0;
            info.forEach(function (element) {
                if (element.difficulty > max) {
                    max = element.difficulty;
                }
            });

            var issueSet = new DataSet();
            var count = 1;
            info.forEach(function (element) {
                var obj = {
                    dataObj:    element,
                    id:         count,
                    content:    element.name + ' ' + element.version + (element.difficulty != 0 ? ' <i class="em-svg em-warning"></i>' : ''),
                    start:      element.start,
                    end:        element.end,
                    className:  supportClasses[element.supportStatus],
                    //style:      "background-color: " + colorScale(element.difficulty, max, 75) + "; border-color: " + colorScale(element.difficulty, max, 25),
                };
                issueSet.add(obj);
                count = count + 1;
            });

            var container = document.getElementById('visualization');
            timelineObj.timeline = new Timeline(container, issueSet, genTimelineOptions(4, 3, true, (a, b) => {
                return a.dataObj.end > b.dataObj.end;
            }));
            timelineObj.timeline.on('select', (properties) => {
                var item = issueSet.get(properties.items[0]).dataObj;
                var issueReq = new XMLHttpRequest();
                issueReq.onload = function () {
                    if (this.status == 200) {
                        var issueInfo = JSON.parse(this.responseText);

                        var infoContent = document.getElementById("info-content-title");
                        infoContent.innerHTML = item.name + " " + item.version;
                        infoContent = document.getElementById("info-content-birth");
                        infoContent.innerHTML = item.start;
                        infoContent = document.getElementById("info-content-death");
                        infoContent.innerHTML = item.end;

                        infoContent = document.getElementById("info-content-issues");
                        infoContent.innerHTML = "";
                        issueInfo.forEach(function (element) {
                            var draft = '<li class="list-group-item">';
                            draft += '<div class="split-box"><span>' + element.name + '</span><span>' + element.difficulty + '</span></div>';
                            draft += '<div>' + element.description + '</div>';
                            draft += '</li>\n';
                            infoContent.innerHTML += draft;
                        });

                        var panel = document.getElementById("info");
                        container.classList.remove("col-md-12");
                        container.classList.add("col-md-9");
                        panel.classList.add("col-md-3");

                        panel.style.display = "block";
                    }
                }
                issueReq.open("GET", "/issueData?os=" + item.id + "&collection=" + collection, true);
                issueReq.send();
            });
            document.getElementById('loading-spinner').style.display = 'none';
        }
    };
    xmlreq.open("GET", "/osData?collection=" + collection, true);
    xmlreq.send();
}

export function closeInfo(timelineObj) {
    var container = document.getElementById('visualization');
    var panel = document.getElementById("info");

    panel.style.display = "none";
    panel.classList.remove("col-md-3");
    container.classList.remove("col-md-9");
    container.classList.add("col-md-12");

    timelineObj.timeline.setSelection([]);
}

// settings callbacks
$(document).ready(function () {
    var focusoutFunc = function () {
        $(this).hide();
        $(this).parent().find('.edit').show();
    };
    $('input.editing[type=text]').focusout(focusoutFunc).keydown(function (event) {
        event.stopImmediatePropagation();
        var keyscan = event.keyCode || event.which;
        if (keyscan == 13) {
            $(this).blur();
            $(this).parent().submit();
        }
    })
    $('select.editing').focusout(focusoutFunc).change(function (event) {
        event.stopImmediatePropagation();
        $(this).blur();
        $(this).parent().submit();
    });
    $('.edit').click(function (event) {
        event.preventDefault();
        $(this).hide();
        $(this).parent().find('.editing').val($(this).text()).show().focus();
    });

    $('.edit-form').on('submit', function (event) {
        event.preventDefault();
        var form = $(this);
        var xmlreq = new XMLHttpRequest();
        xmlreq.onload = function () {
            if (this.status != 200) {
                alert("Could not do the thing error: " + this.status + ", " + this.responseText);
            } else {
                var obj = JSON.parse(this.response);
                for (const [key, val] of Object.entries(obj)) {
                    form.find('.edit.' + key).text(val);
                }
            }
        };
        xmlreq.open("POST", window.location.href + "/edit", true);
        xmlreq.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xmlreq.send(form.serialize());
    });

    $('.remove-form').on('submit', function (event) {
        event.preventDefault();
        var form = $(this);
        var xmlreq = new XMLHttpRequest();
        xmlreq.onload = function () {
            if (this.status != 200) {
                alert("Could not do the thing error: " + this.status + ", " + this.responseText);
            } else {
                form.closest('tr').remove();
            }
        };
        xmlreq.open("POST", window.location.href + "/remove", true);
        xmlreq.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xmlreq.send(form.serialize());
    });

    $('.add-form').on('submit', function (event) {
        event.preventDefault();
        var form = $(this);
        form.closest('.modal').modal('hide');
        var xmlreq = new XMLHttpRequest();
        xmlreq.onload = function () {
            if (this.status != 200) {
                alert("Could not do the thing error: " + this.status + ", " + this.responseText);
            } else {
                location.reload();
            }
        };
        xmlreq.open("POST", window.location.href + "/add", true);
        xmlreq.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xmlreq.send(form.serialize());
    });
});
