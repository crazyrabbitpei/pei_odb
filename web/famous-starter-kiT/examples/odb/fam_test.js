define(function(require, exports, module) {
    var Engine           = require("famous/core/Engine");
    var Surface          = require("famous/core/Surface");
    var Modifier         = require("famous/core/Modifier");
    var Transform        = require("famous/core/Transform");
    var Transitionable   = require("famous/transitions/Transitionable");
    var Easing           = require("famous/transitions/Easing");
    var ContainerSurface = require("famous/surfaces/ContainerSurface");
    var Transform = require('famous/core/Transform');
    var StateModifier = require('famous/modifiers/StateModifier');


    var mainContext = Engine.createContext();
    $(document).ready(function(){
        $.ajax({
            'url':'http://140.123.101.181:3636/pei/odb.cgi',
            'data': $('form').serialize(),
            'type': 'POST',
            'success':function(data){
                var arr = data.split("</br>");
                var filename="";
                for(i=0;i<arr.length-1;i++){
                    NameBlock(i,arr[i],function(surface,originModifier){
                        mainContext.add(originModifier).add(surface);
                        surface.on('click', function() {
                            surface.setProperties({
                                    backgroundColor: '#878785'
                            });
                            list(surface.getContent());

                        //console.log(surface.getContent());
                        });

                    });
                }
            },
            'error':function(xhr,ajaxOptions, thrownError){
            console.log(xhr.status);
            console.log(thrownError);
            }
        });
    });
function NameBlock(num,filename,add){
    var firstSurface = new Surface({
        content: filename,
        size: [100, 50],
        properties: {
            backgroundColor: 'rgb(240, 238, 233)',
            textAlign: 'center',
            overflow: 'hidden',
            textOverflow: 'ellipsis'
            //padding: '5px',
            //border: '2px solid rgb(210, 208, 203)',
            //marginTop:'150px',
            //marginLeft: '50px'
        }
    });
    var originModifier = new StateModifier({
        origin: [num*1.1+0.5, 0.5],
        align: [0.5, 0.5]
            });
    add(firstSurface,originModifier);
}

function list(filename){
    $("#filename").val(filename);
    document.getElementById('post_form').submit();
}

});


