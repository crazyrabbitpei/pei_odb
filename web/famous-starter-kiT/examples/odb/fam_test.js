define(function(require, exports, module) {
    var Engine           = require("famous/core/Engine");
    var Surface          = require("famous/core/Surface");
    var ImageSurface = require("famous/surfaces/ImageSurface");
    var InputSurface = require("famous/surfaces/InputSurface");
    var Modifier         = require("famous/core/Modifier");
    var Transform        = require("famous/core/Transform");
    var Transitionable   = require("famous/transitions/Transitionable");
    var TransitionableTransform = require("famous/transitions/TransitionableTransform");
    var Easing           = require("famous/transitions/Easing");
    var ContainerSurface = require("famous/surfaces/ContainerSurface");
    var Transform = require('famous/core/Transform');
    var StateModifier = require('famous/modifiers/StateModifier');
    var RenderNode = require('famous/core/RenderNode');
    var ContainerSurface = require("famous/surfaces/ContainerSurface");


    var mainContext = Engine.createContext();
    $(document).ready(function(){
        $.ajax({
            'url':'http://140.123.101.181:3636/pei/odb.cgi',
            'data': $('form').serialize(),
            'type': 'POST',
            'success':function(data){
                console.log("arr:"+data);
                var arr = data.split("</br>");
                var filename="";

                for(i=0;i<arr.length-1;i++){
                    NameBlock(mainContext,i,arr[i],function(mainContext,surface,originModifier,deleteModifier,editModifier,desc1,desc2,container,num){
                        var node = new RenderNode(originModifier);
                        node.add(surface);
                        node.add(deleteModifier).add(desc1);
                        node.add(editModifier).add(desc2);
                        mainContext.add(node);
                        /*
                        mainContext.add(originModifier).add(surface);
                        mainContext.add(descModifier).add(desc);
                        */
                        /*
                        var containModifier = new StateModifier({
                        });
                        container.add(surface);
                        container.add(deleteModifier).add(desc1);
                        mainContext.add(containModifier).add(container);
                        */
                        desc1.on('click',function(){
                            del(surface.getContent());
                        });
                        desc2.on('click',function(){

                            var input = new InputSurface({
                               size: [100, 50],
                                placeholder: 'Type text here',
                                value: surface.getContent(),
                                name:'rename_input',
                                type: 'text',
                                properties:{
                                    color:'black'
                                }
                            });
                            var stateModifier = new StateModifier({
                                    transform: Transform.translate(50, 150, 0)
                            });
                            node.add(stateModifier).add(input);
                            mainContext.add(node);
                            input.on('mouseout',function(){
                                stateModifier.setOpacity(0);
                                stateModifier.setTransform(Transform.behind);
                                var newfilename = $("input[name='rename_input']").val()
                                $("input[name='rename_input']").remove()
                                rename(surface.getContent(),newfilename);
                            });
                        });
                        surface.on('click', function() {
                            surface.setProperties({
                                    backgroundColor: '#878785'
                            });
                            list(surface.getContent());
                        });
                        surface.on('mouseout', function() {
                            surface.setProperties({
                                    backgroundColor: 'cornflowerblue',
                                    boxShadow:''
                            });
                            originModifier.setTransform(Transform.scale(1,1), {curve: 'easeOut',duration: 100  });
                            desc1.setProperties({
                               marginTop:'110px',
                               backgroundColor:'white'
                            });
                            deleteModifier.setOpacity(0,{curve: 'easeOut',duration: 100  });
                            deleteModifier.setTransform(Transform.behind);
                        });
                        desc1.on('mouseout', function() {
                            surface.setProperties({
                                    backgroundColor: 'cornflowerblue',
                                    boxShadow:''
                            });
                            originModifier.setTransform(Transform.scale(1,1), {curve: 'easeOut',duration: 100  });
                            desc1.setProperties({
                               marginTop:'110px',
                               backgroundColor:'white'
                            });
                            deleteModifier.setOpacity(0,{curve: 'easeOut',duration: 100  });
                            deleteModifier.setTransform(Transform.behind);
                        });
                        desc2.on('mouseout', function() {
                            surface.setProperties({
                                    backgroundColor: 'cornflowerblue',
                                    boxShadow:''
                            });
                            originModifier.setTransform(Transform.scale(1,1), {curve: 'easeOut',duration: 100  });
                            desc2.setProperties({
                               marginTop:'110px',
                               backgroundColor:'white'
                            });
                            editModifier.setOpacity(0.3,{curve: 'easeOut',duration: 100  });
                            editModifier.setTransform(Transform.behind);
                        });

                        surface.on('mouseover', function() {
                            surface.setProperties({
                                boxShadow:'5px 5px 5px rgba(0, 0, 0, 0.5)',
                                backgroundColor: 'cadetblue'
                            });
                            //originModifier.setTransform(Transform.scale(1.05,1.05), {curve: 'easeOut',duration: 100  });
                            desc1.setProperties({
                               marginTop:'110px',
                            });
                            deleteModifier.setOpacity(0.8,{curve: 'easeOut',duration: 500  });
                            deleteModifier.setTransform(Transform.inFront,{curve: 'easeOut',duration: 1000  });
                        });
                        desc1.on('mouseover', function() {
                            surface.setProperties({
                                boxShadow:'5px 5px 5px rgba(0, 0, 0, 0.5)',
                                backgroundColor: 'cadetblue'
                            });
                            //originModifier.setTransform(Transform.scale(1.05,1.05), {curve: 'easeOut',duration: 100  });
                            desc1.setProperties({
                               marginTop:'110px',
                            });
                            deleteModifier.setOpacity(0.8,{curve: 'easeOut',duration: 500  });
                            deleteModifier.setTransform(Transform.inFront,{curve: 'easeOut',duration: 1000  });
                        });
                        desc2.on('mouseover', function() {
                            surface.setProperties({
                                boxShadow:'5px 5px 5px rgba(0, 0, 0, 0.5)',
                                backgroundColor: 'cadetblue'
                            });
                            desc2.setProperties({
                               marginTop:'110px',
                            });
                            editModifier.setOpacity(0.8,{curve: 'easeOut',duration: 500  });
                            editModifier.setTransform(Transform.inFront,{curve: 'easeOut',duration: 1000  });
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

function NameBlock(mainContext,num,filename,add){
    var firstSurface = new Surface({
        content: filename,
        size: [100, 50],
        properties: {
            backgroundColor: 'cornflowerblue',
            textAlign: 'center',
            overflow: 'hidden',
            textOverflow: 'ellipsis',
            padding: '5px',
            border: '2px solid rgb(210, 208, 203)',
            marginTop:'150px',
            marginLeft: '50px'
        }
    });
    var desc1 = new ImageSurface({
        size: [25, 25],
        properties: {
            //background: 'rgba(0, 0, 0, 0.8)',
            //backgroundColor:'black',
            padding: '0px',
            marginTop:'110px',
            marginLeft: '5px',
            //borderRadius: '8px 8px 0px 0px'
        }
    });
    desc1.setContent('./image/close.png');
    var desc2 = new ImageSurface({
        size: [25, 25],
        properties: {
            //background: 'rgba(0, 0, 0, 0.8)',
            //backgroundColor:'black',
            padding: '0px',
            marginTop:'110px',
            marginLeft: '15px',
            //borderRadius: '8px 8px 0px 0px'
        }
    });
    desc2.setContent('./image/pencile.png');
    var originModifier = new StateModifier({
        origin: [num*1.1+0.5, 0.5],
        align: [0.8, 0.1]
    });

    var editModifier = new StateModifier({
        origin: [i*4.4+0.1, 0.5],
        align: [0.85, 0.1],
        opacity:0.3
    });

    var deleteModifier = new StateModifier({
        origin: [num*4.4+0.1, 0.5],
        align: [0.8, 0.1],
        opacity:0,
        transform:Transform.behind,
    });
    var container = new ContainerSurface({
    })
    //container.add(firstSurface);
    //container.add(desc);
    add(mainContext,firstSurface,originModifier,deleteModifier,editModifier,desc1,desc2,container,num);
}

function list(filename){
    $("#post_filename").val(filename);
    document.getElementById('post_form').submit();
}
function del(filename){
    $("#del_filename").val(filename);
    document.getElementById('del_form').submit();
}
function rename(filename,newfilename){
    $("#o_filename").val(filename);
    $("#re_filename").val(newfilename);
    console.log(filename);
    console.log(newfilename);
    $("input[name='rename_input']").val(newfilename);
    if(filename!=newfilename){
        document.getElementById('re_form').submit();
    }
}
});


