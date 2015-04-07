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
    var HeaderFooterLayout = require("famous/views/HeaderFooterLayout");
    var GridLayout = require("famous/views/GridLayout");


    var mainContext = Engine.createContext();

    var layout = new HeaderFooterLayout({
                headerSize: 100,
                footerSize: 50
        });
    layout.header.add(new Surface({
                size: [undefined, 100],
                content: "Header",
                classes: ["red-bg"],
                properties: {
                    lineHeight: "100px",
                    textAlign: "center"
                }
    }));
    
    layout.content.add(new Surface({
                size: [undefined, undefined],
                properties: {
                    //lineHeight: window.innerHeight - 150 + 'px',
                    //backgroundColor:'darkcyan',
                    //textAlign: "center"
                }
    }));
    layout.footer.add(new Surface({
                size: [undefined, 50],
                content: "Footer",
                classes: ["red-bg"],
                properties: {
                    lineHeight: "50px",
                    textAlign: "center"
                    }
    
    }));
    mainContext.add(layout);



    $(document).ready(function(){
        $.ajax({
            'url':'http://140.123.101.181:3636/pei/odb.cgi',
            'data': $('form').serialize(),
            'type': 'POST',
            'success':function(data){
                console.log("arr:"+data);
                //var arr = data.split("</br>");
                
                var arr=[];
                for(i=0;i<=11;i++){
                    arr.push(i);
                }
                
                var filename="";
                var container = new ContainerSurface();
                var row = 4,column=4;
                var grid = new GridLayout({
                            dimensions: [row, column]
                });
                var grid2= new GridLayout({
                            dimensions: [row, column]
                });
                var grid3 = new GridLayout({
                            dimensions: [row, column]
                });
                var surfaces = [];
                var desc1s = [];
                var desc2s = [];
                var delm=[];
                var editm=[];
                var originm=[];


                /*
                var deleteModifier = new StateModifier({
                        size: [600, 150],
                        origin: [0.5, 0.5],
                        align: [0.5, 0.2],
                        opacity:0.3
                        //transform:Transform.behind
                });
                var editModifier = new StateModifier({
                        size: [600, 150],
                        origin: [0.5, 0.5],
                        align: [0.5, 0.2],
                        opacity:0.3
                });
                */
                for(i=0;i<arr.length-1;i++){
                    NameBlock(container,mainContext,i,arr[i],function(container,mainContext,surface,originModifier,deleteModifier,editModifier,desc1,desc2,input,container,num){
                        
                        var stateModifier = new StateModifier({
                            align: [0.5, 1.7]
                        });
                        input.setValue(surface.getContent());
                        var node = new RenderNode(originModifier);
                        node.add(surface);
                        node.add(stateModifier).add(input);
                        node.add(deleteModifier).add(desc1);
                        node.add(editModifier).add(desc2);
                        surfaces.push(node);
                        //mainContext.add(node);
                                
                        /*
                        surfaces.push(surface);
                        desc1s.push(desc1);
                        desc2s.push(desc2);
                        delm.push(deleteModifier);
                        editm.push(editModifier);
                        originm.push(originModifier);
                        */
                        /*
                        mainContext.add(originModifier).add(surface);
                        mainContext.add(descModifier).add(desc);
                        */
                        /*
                        var containModifier = new StateModifier({
                        });
                        */
                        /*
                        container.add(surface);
                        container.add(deleteModifier).add(desc1);
                        mainContext.add(container);
                        */
                        desc1.on('click',function(){
                            del(surface.getContent());
                        });
                        desc2.on('click',function(){
                                input.setProperties({
                                    visibility:'visible'
                                });
                            /*
                            var input = new InputSurface({
                               size: [100, 60],
                                //placeholder: 'Type text here',
                                value: surface.getContent(),
                                name:'rename_input',
                                type: 'text',
                                properties:{
                                    color:'black'
                                }
                            });
                            */
                            input.on('mouseout',function(){
                                input.setProperties({
                                    visibility:'hidden'
                                });
                                console.log("hidden")
                                var newfilename = $("input[name='rename_input']").val()
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
                               //marginTop:'120px',
                            });
                            deleteModifier.setOpacity(0,{curve: 'easeOut',duration: 100  });
                            //deleteModifier.setTransform(Transform.behind);
                        });
                        desc1.on('mouseout', function() {
                            surface.setProperties({
                                    backgroundColor: 'cornflowerblue',
                                    boxShadow:''
                            });
                            originModifier.setTransform(Transform.scale(1,1), {curve: 'easeOut',duration: 100  });
                            desc1.setProperties({
                               //marginTop:'120px',
                            });
                            deleteModifier.setOpacity(0,{curve: 'easeOut',duration: 100  });
                            //deleteModifier.setTransform(Transform.behind);
                        });
                        desc2.on('mouseout', function() {
                            surface.setProperties({
                                    backgroundColor: 'cornflowerblue',
                                    boxShadow:''
                            });
                            originModifier.setTransform(Transform.scale(1,1), {curve: 'easeOut',duration: 100  });
                            desc2.setProperties({
                               //marginTop:'120px',
                            });
                            editModifier.setOpacity(0.3,{curve: 'easeOut',duration: 100  });
                            //editModifier.setTransform(Transform.behind);
                        });

                        surface.on('mouseover', function() {
                            surface.setProperties({
                                boxShadow:'5px 5px 5px rgba(0, 0, 0, 0.5)',
                                backgroundColor: 'cadetblue'
                            });
                            desc1.setProperties({
                               //marginTop:'120px',
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
                               //marginTop:'120px',
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
                              // marginTop:'120px',
                            });
                            editModifier.setOpacity(0.8,{curve: 'easeOut',duration: 500  });
                            editModifier.setTransform(Transform.inFront,{curve: 'easeOut',duration: 1000  });
                        });

                    });
                }
                grid.sequenceFrom(surfaces);
                //grid2.sequenceFrom(desc1s);
                //grid3.sequenceFrom(desc2s);
                
                var stateModifier = new StateModifier({
                        size: [500, 150],//when 250:3, 130:when 1~2
                        origin: [0.5, 1],
                        align: [0.5, 0.1],

                });
                //mainContext.add(stateModifier).add(grid);
                layout.content.add(stateModifier).add(grid);
                //mainContext.add(deleteModifier).add(grid2);
                //mainContext.add(editModifier).add(grid3);

            },
            'error':function(xhr,ajaxOptions, thrownError){
            console.log(xhr.status);
            console.log(thrownError);
            }
        });
    });

function NameBlock(container,mainContext,num,filename,add){
    var topl;
    var top2;
    if(num>3){
        var count = Math.floor(num/4);
        console.log("count:"+count);
        if(count==0){
            //count = (num+1)4;
        }
        topl=150+count*50;
        topl2 = topl-30;
        topl3 = topl-56;
    }
    else{
        topl=150;
        topl2=120;
        topl3=94;
    }
    var firstSurface = new Surface({
        content: filename,
        size: [100,60],
        properties: {
            backgroundColor: 'cornflowerblue',
            textAlign: 'center',
            overflow: 'hidden',
            textOverflow: 'ellipsis',
            padding: '5px',
            border: '2px solid rgb(210, 208, 203)',
            marginTop:topl+'px',
            marginLeft: '0px'
        }
    });
    var input = new InputSurface({
            size: [100, 60],
            //placeholder: 'Type text here',
            //value: surface.getContent(),
            value: "",
            name:'rename_input',
            type: 'text',
            properties:{
                color:'black',
                visibility: 'hidden',
                marginTop:topl3+'px'
            }
    });

    var desc1 = new ImageSurface({
        size: [35, 35],
        content:'./image/close.png',
        properties: {
            padding: '5px',
            marginTop:topl2+'px',
            marginLeft: '0px',
        }
    });
    var desc2 = new ImageSurface({
        size: [35, 35],
        content:'./image/pencile.png',
        properties: {
            padding: '5px',
            marginTop:topl2+'px',
            marginLeft: '70px',
        }
    });
    var originModifier = new StateModifier({
        /*
        origin: [num*1.1+0.5, 0.5],
        align: [0.8, 0.1]
        */
        //size: [600, 150],
        //origin: [0.5, 0.5],
        align: [0.5, 0.2],
        //transform: Transform.translate(50, 0, 0)
    });

    var editModifier = new StateModifier({
        //origin: [i*4.4+0.5, 0.5],
        align: [0.5, 0.2],
        opacity:0.3,
        size: [600, 150]
        /*
        origin: [0.5, 0.5],
        align: [0.5, 0.2],
        opacity:0.3
        */
    });

    var deleteModifier = new StateModifier({
        //origin: [num*4.4+0.1, 0.5],
        align: [0.5, 0.2],
        opacity:0,
        size: [600, 150],
        transform:Transform.behind
        /*
        origin: [0.5, 0.5],
        align: [0.5, 0.2],
        opacity:0.3,
        */

    });

    var container = new ContainerSurface({
    })
    //container.add(firstSurface);
    //container.add(desc);
    add(container,mainContext,firstSurface,originModifier,deleteModifier,editModifier,desc1,desc2,input,container,num);
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


