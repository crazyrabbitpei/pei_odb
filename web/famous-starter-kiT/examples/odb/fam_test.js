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
    var Scrollview = require("famous/views/Scrollview");


    var mainContext = Engine.createContext();
    var scrollview = new Scrollview();
    var page=1;
    var layout = new HeaderFooterLayout({
                headerSize: 120,
                //footerSize: 50,
                contentSize:500
        });
    /*
    layout.header.add(new Surface({
                size: [undefined, 100],
                content: "Header",
                classes: ["red-bg"],
                properties: {
                    lineHeight: "100px",
                    textAlign: "center",
                    backgroundColor:"blue"
                }
    }));
    */
    layout.content.add(new Surface({
                size: [undefined, undefined],
                content:"",
                properties: {
                    //lineHeight: window.innerHeight - 150 + 'px',
                    backgroundColor:'gainsboro',
                    textAlign: "center"
                }
    }));
    
    layout.footer.add(new Surface({
                size: [undefined, 50],
                content: "2015 Apr. Data Engineering Midterm Report",
                classes: ["red-bg"],
                properties: {
                    lineHeight: "50px",
                    textAlign: "center",
                    backgroundColor:"grey"
                    }
    
    }));
    
    layoutm = new StateModifier({
        
    });
    mainContext.add(layout);
    
    value = '/pei/odb.cgi';
    var uploadSurface = new Surface({
        size:[undefined,undefined],
        content:'<FORM id="form1" METHOD="POST" ACTION="'+value+'" enctype="multipart/form-data" style=" margin-top: 50px; width: 50%; "" ">'+
                '<input type="hidden" name="command" value="PUT" />'+
                '<INPUT TYPE="file" NAME="filename" VALUE="">'+
                //'Detail<input type="radio" name="command" value="DETAIL" /></br>'+
                '<INPUT TYPE="submit" VALUE="Upload"></br>'+
                '</FORM>'
                ,
        properties: {
            backgroundColor: 'black'
        }
    });
    var upload_input = new InputSurface({
        size: [300, 30],
        placeholder: 'Search',
        name:'filename',
        type: 'file',
        properties:{
            color:""
        }
    });
    
    var uploadModifier = new StateModifier({
        transform:Transform.translate(850,0,0)
    });
    layout.header.add(uploadSurface);
    //layout.header.add(uploadModifier).add(upload_input);

    uploadModifier.setTransform(
      Transform.translate(100, 100, 0),
        { duration : 1000, curve: Easing.inOutBack  }
        );
    

    var search_input = new InputSurface({
        size: [300, 30],
        placeholder: 'Search',
        name:'search_input',
        type: 'textarea',
        properties:{
            color:'black'
        }
    });

    var searchModifier = new StateModifier({
        transform:Transform.translate(580,0,0)
    });
    layout.header.add(searchModifier).add(search_input);//commit for hidden
    
    searchModifier.setTransform(
      Transform.translate(580, 50, 0),
        { duration : 1000, curve: Easing.inOutBack  }
        );
    
    search_input.on('keyup',function(e){
        page=1;
        word = $("input[name='search_input']").val();
        if(word!=''){
            if(createFileBlock("FIND","POST","4","4",word,"normal",page)==0){
                $(".nothing").remove();
            }
        }
        else{
            createFileBlock("LIST","POST","4","4","","normal",page);
        }
    })
    /*
    Engine.on('keydown', function(e) {
            if(e.which == 13) {
                word = $("input[name='search_input']").val();
                createFileBlock("FIND","POST","4","4",word,"normal");
            }    
    });
    */

    Engine.on('keyup', function(e) {
        //TODO : show pagenation
            if(e.which == 40) {
                page=page+1;
                word = $("input[name='search_input']").val();
                if(word!=""){
                    createFileBlock("FIND","POST","4","4",word,"down",page,function(result){
                        page = result;
                    });
                }
                else{ 
                    createFileBlock("LIST","POST","4","4","","down",page,function(result){
                        page = result;
                    });
                }
            }    
            if(e.which == 38) {
                page=page-1;
                word = $("input[name='search_input']").val();
                if(word!=""){
                    createFileBlock("FIND","POST","4","4",word,"up",page,function(result){
                            page = result;
                    });
                }
                else{ 
                    createFileBlock("LIST","POST","4","4","","up",page,function(result){
                            page = result;
                    });
                }
            }    
    });
    $(document).ready(function(){
            createFileBlock("LIST","POST","4","4","","normal",page,function(result){
                page = result;
            });
    });

    function createFileBlock(command,method,row,column,sfilename,test,page,callback){
        $.ajax({
            'url':value,
            'data': 'command='+command+'&search='+sfilename+'&page='+page,
            'type': method,
            'success':function(data){
                if(test!="demo"){
                    var arr = data.split("</br>");
                    //console.log(arr);
                    if(arr.length==1){
                        if(test=="down"){
                            page = page-1;
                            callback(page);
                            return ;
                        }
                        else if(test=="up"){
                            page = page+1;
                            callback(page);
                            return;
                        }
                    }
                    $(".nothing").remove();
                    $(".file").remove();
                    if(arr[0]=="-1,-1,-1,-1"){
                        layout.content.add(new Surface({
                            size: [undefined, undefined],
                            content:"<section class='nothing'>"+
                                    "<h1>Not Found.</h1></br>"+
                                    "</section>"
                            ,
                            properties: {
                                backgroundColor:'gainsboro',
                                textAlign: "center"
                            }
                        }));
                        mainContext.add(layout);
                        return -1;
                    }
                }
                else if(test=="demo"){
                    var arr=[];
                    for(i=0;i<=33;i++){
                        arr.push(i+","+i+","+i+","+i);
                    }
                }
                var filename="";
                var container = new ContainerSurface();
                var grid = new GridLayout({
                            dimensions: [row, column]
                });
                var surfaces = [];

                var filenames=[];
                var sizes=[];
                var dates=[];
                var types=[];

                grid.sequenceFrom(surfaces);

                var filestatem = new StateModifier({
                        //origin: [0.1, 0.5],
                        //align: [0.1, 0.5],
                        transform:Transform.translate(0,0,0),
                        opacity:0.3
                });
                var filestate = new Surface({
                    size:[400,200],
                    classes:['state'],
                    properties:{
                        textAlign:"left",
                        padding:'20px',
                        backgroundColor:'black',
                        borderRadius:'50px',
                        visibility:'hidden',
                        overflow:'hidden',
                        textOverflow:'ellipsis'
                    }
                });
                layout.content.add(filestatem).add(filestate);
                for(i=0;i<arr.length-1;i++){
                     var filename = arr[i].split(",");
                     sizes.push(filename[0]);
                     filenames.push(filename[1]);
                     dates.push(filename[2]);
                     types.push(filename[3]);
                     NameBlock(container,mainContext,i,filename[1],function(container,mainContext,surface,originModifier,deleteModifier,editModifier,desc1,desc2,input,container,num){
                        
                        var inputModifier = new StateModifier({
                            align: [0.5, 1.7]
                        });
                        input.setValue(surface.getContent());
                        var node = new RenderNode(originModifier);
                        node.add(surface);
                        node.add(inputModifier).add(input);
                        node.add(deleteModifier).add(desc1);
                        node.add(editModifier).add(desc2);
                        surfaces.push(node);
                                
                        desc1.on('click',function(){
                            del(surface.getContent());
                        });
                        desc2.on('click',function(){
                                input.setProperties({
                                    visibility:'visible'
                                });
                            $("input[name='rename_input']").mouseout(function(){
                                var newfilename = $(this).val();
                                input.setProperties({
                                    visibility:'hidden'
                                });
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
                            filestate.setProperties({
                                visibility:'hidden',
                                overflow:'hidden',
                                textOverflow:'ellipsis'
                            });
                            deleteModifier.setOpacity(0,{curve: 'easeOut',duration: 100  });
                            filestatem.setOpacity(0);
                            filestatem.setTransform(Transform.translate(0,0,0));
                        });
                        desc1.on('mouseout', function() {
                            surface.setProperties({
                                    backgroundColor: 'cornflowerblue',
                                    boxShadow:''
                            });
                            originModifier.setTransform(Transform.scale(1,1), {curve: 'easeOut',duration: 100  });
                            deleteModifier.setOpacity(0,{curve: 'easeOut',duration: 100  });
                        });
                        desc2.on('mouseout', function() {
                            surface.setProperties({
                                    backgroundColor: 'cornflowerblue',
                                    boxShadow:''
                            });
                            originModifier.setTransform(Transform.scale(1,1), {curve: 'easeOut',duration: 100  });
                            editModifier.setOpacity(0.3,{curve: 'easeOut',duration: 100  });
                        });

                        surface.on('mouseover', function() {
                            surface.setProperties({
                                boxShadow:'5px 5px 5px rgba(0, 0, 0, 0.5)',
                                backgroundColor: 'cadetblue'
                            });

                            filestate.setProperties({
                                visibility:'visible',
                                color:'white',
                                overflow:'hidden',
                                textOverflow:'ellipsis'

                                
                            });
                            filestate.setContent(
                                                '<div style="width: 100%; overflow: hidden; text-overflow:ellipsis"><h3>'+filenames[num]+'</h3></div><hr>'+
                                                'Size : '+sizes[num]+' KB</br>'+
                                                'Upload date : '+dates[num]+'</br>'+
                                                'Type : '+types[num]+'</br>'
                                                );

                            deleteModifier.setOpacity(0.8,{curve: 'easeOut',duration: 500  });
                            deleteModifier.setTransform(Transform.inFront,{curve: 'easeOut',duration: 1000  });
                            filestatem.setOpacity(0.5,{curve: 'easeOut',duration: 1000  });
                            filestatem.setTransform(Transform.translate(50,70),{curve: 'easeOut',duration: 1000  });

                        });
                        desc1.on('mouseover', function() {
                            surface.setProperties({
                                boxShadow:'5px 5px 5px rgba(0, 0, 0, 0.5)',
                                backgroundColor: 'cadetblue'
                            });

                            deleteModifier.setOpacity(0.8,{curve: 'easeOut',duration: 500  });
                            deleteModifier.setTransform(Transform.inFront,{curve: 'easeOut',duration: 1000  });
                        });
                        desc2.on('mouseover', function() {
                            surface.setProperties({
                                boxShadow:'5px 5px 5px rgba(0, 0, 0, 0.5)',
                                backgroundColor: 'cadetblue'
                            });
                            editModifier.setOpacity(0.8,{curve: 'easeOut',duration: 500  });
                            editModifier.setTransform(Transform.inFront,{curve: 'easeOut',duration: 1000  });
                        });

                    });
                }

                var stateModifier = new StateModifier({
                        size: [500, 150],//when 250:3, 130:when 1~2
                        //origin: [0.5, 1],
                        //align: [0.5, 0.1],
                        transform:Transform.translate(180,-130,0),
                        opacity:0

                });
                layout.content.add(stateModifier).add(grid);
                stateModifier.setTransform(
                    Transform.translate(180, -120, 0),
                    { duration : 1000, curve: Easing.inOutBack  }
                );
                stateModifier.setOpacity(1,{curve: 'easeOut',duration: 500  });



            },
            'error':function(xhr,ajaxOptions, thrownError){
            console.log(xhr.status);
            console.log(thrownError);
            }
        });
    return 0;        
    }


    function search(word,callback){

    }
function NameBlock(container,mainContext,num,filename,add){
    var topl;
    var top2;
    if(num>3){
        var count = Math.floor(num/4);
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
        classes:["file"],
        properties: {
            backgroundColor: 'cornflowerblue',
            textAlign: 'center',
            overflow: 'hidden',
            textOverflow: 'ellipsis',
            padding: '5px',
            border: '2px solid rgb(210, 208, 203)',
            marginTop:topl+'px',
            marginLeft: '300px'
        }
    });
    var input = new InputSurface({
            classes:['file'],
            size: [100, 60],
            value: "",
            name:'rename_input',
            type: 'text',
            properties:{
                color:'black',
                visibility: 'hidden',
                marginTop:topl3+'px',
                marginLeft: '250px'
            }
    });

    var desc1 = new ImageSurface({
        size: [35, 35],
        classes:['file'],
        content:'./image/close.png',
        properties: {
            padding: '5px',
            marginTop:topl2+'px',
            marginLeft: '300px',
        }
    });
    var desc2 = new ImageSurface({
        classes:['file'],
        size: [35, 35],
        content:'./image/pencile.png',
        properties: {
            padding: '5px',
            marginTop:topl2+'px',
            marginLeft: '370px',
        }
    });
    var originModifier = new StateModifier({
        align: [0.1, 0.2],
    });

    var editModifier = new StateModifier({
        align: [0.1, 0.2],
        opacity:0.3,
        size: [600, 150]
    });

    var deleteModifier = new StateModifier({
        align: [0.1, 0.2],
        opacity:0,
        size: [600, 150],
        transform:Transform.behind
    });

    var container = new ContainerSurface({
    })
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

    $("input[name='rename_input']").val(newfilename);
    if(filename!=newfilename){
        document.getElementById('re_form').submit();
    }
}

});


