var page=1;
var current_dir="Mydrive";
var dir_id="0";
$(document).ready(function(){
                var column = "child";
                var title = document.getElementById("dir_name");
                var path = document.getElementById("dir_config");
                var dirname;
                console.log("current id:"+dir_id);
                    
                title.innerHTML = "Mydrive";
                dirname = "Mydrive";
                current_dir = "/";
                dir_id = "0";
                var path_link = document.createElement("span");
                path_link.setAttribute("class","path_link");
                path_link.setAttribute("value",0);
                path_link.innerHTML = dirname;
                path.appendChild(path_link);
                /*
                createFileBlock("LIST","POST",current_dir,"normal",page,function(result){
                    page = result;
                });
                */
                /*
                createFileBlock("LIST","POST",dir_id,"normal",page,function(result){
                    page = result;
                });
                */
                createFileBlock("LIST","POST",dir_id,"test",page,column,function(result){
                    page = result;
                });
                
});


//block default mouse right
function blockMenu(e){
        console.log("body click:"+e.which);
        if(e.which==3){
            e.preventDefault();//取消DOM預設事件
            e.stopPropagation(); // Stops some browsers from redirecting.
        }
        else{
            $(".blocks").remove();
        }
}
document.oncontextmenu = blockMenu;
var body = document.querySelectorAll('body').item(0);
body.addEventListener('mousedown',blockMenu, false);

//web command: add dir
$("#add_dir").click(function(){
    var check = document.getElementsByClassName("files dir");
    var len =document.getElementsByClassName("files dir").length;
    if(len){
        if(check[len-1].tagName=="INPUT"){
            check[len-1].remove();
        }
    }

    var dir = document.createElement("input");
    dir.setAttribute("class","files dir");
    dir.setAttribute("type","textarea");
    document.getElementById("file_block").appendChild(dir);
    
    $("input.files.dir").focusout(function(){
        var name = $(this).val();
        console.log("dir name:"+name);
        if(!name){
            $(this).remove();
            return ;
        }
        
        sendCommand("CDIR",name,"","","filename",dir_id,"","POST",function(result){
            console.log("result:"+result);
            var input= document.getElementsByClassName("files dir");
            var len =document.getElementsByClassName("files dir").length;
            var id = result.match(/id:[0-9]+/);
            id = id[0].replace("id:","");
            input[len-1].remove();
            var dir = document.createElement("div");
            dir.setAttribute("class","files dir");
            dir.setAttribute("value",id);
            dir.innerHTML = name;
            dir.addEventListener('dblclick',intodir);
            document.getElementById("file_block").appendChild(dir);
            dir.addEventListener('mousedown',control,false);
        });
        
    });
});
$("#upload").click(function(){
    var file = document.getElementsByName('filename')[0];
    upload(file,file.files[0].name,0,1,"single");
});
function sendCommand(command,filename,id,newfilename,par,path,newpath,method,callback){
    var formData = new FormData();
    formData.append(par,filename);
    formData.append('command',command);
    formData.append('newfilename',newfilename);
    formData.append('path',path);
    formData.append('newpath',newpath);
    formData.append('getid',id);
        $.ajax({
            'url':'/pei/odb.cgi',
            'data':formData,
            'processData': false,  // tell jQuery not to process the data
            'contentType': false,  // tell jQuery not to set contentType
            'type':method,
            'success':function(data){
                callback(data);
            },
            'error':function(xhr,ajaxOptions, thrownError){
                        console.log(xhr.status);
                        console.log(thrownError);
            }
        });
}

function createFileBlock(command,method,sfilename,show,page,column,callback){
        $.ajax({
            'url':'/pei/odb.cgi',
            'data': 'command='+command+'&search='+sfilename+'&page='+page+'&column='+column,
            'type': method,
            'success':function(data){

                console.log(data);
                var arr = data.split("</br>");
                var filename="";

                var detail=[];
                var key=[];
                var offset=[];
                var dates = [];
                var filenames = [];
                var types = [];
                
                var file_array =new Array();
                
                for(i=0;i<arr.length-1;i++){
                        if(arr[i]=="none"){continue;}
                        var data = arr[i].split(",");
                        var id = data[0];
                        var filename = data[1];
                        var type = data[2].match(/@type:.*/);
                        type = type[0].replace("@type:","");
                        var date = data[2].match(/@ctime:.*/);
                        date = date[0].replace("@ctime:","");
                        file_array.push({id:id,name:filename,type:type,date:date});
                }
                //sort file by__(default is date)
                file_array.sort(sortFunction);
                file_array = JSON.stringify(file_array);
                file_array = JSON.parse(file_array);
                
                if(show=="test"){
                    $(".files").remove();
                    //console.log(arr);
                    if(arr==""){
                        //errMsg("404");
                        return;
                    }
                    if(arr.length==1){
                        if(show=="down"){
                            page = page-1;
                            callback(page);
                            return ;
                        }
                        else if(show=="up"){
                            page = page+1;
                            callback(page);
                            return;
                        }
                    }

                    for(i=0;i<file_array.length;i++){
                        newfile(file_array[i].id,file_array[i].name,file_array[i].type);
                    }
                    $(".nothing").remove();
                    $(".file").remove();
                }
                else if(show=="move"){
                    var block = document.createElement("span");
                    block.setAttribute("class","move_block");
                    var check = document.createElement("span");
                    check.setAttribute("class","move_block_check");
                    check.innerHTML = "move";
                    for(i=0;i<file_array.length;i++){
                        console.log("move list:["+file_array[i].id+"]"+file_array[i].name);
                        var list = document.createElement("div");
                        list.setAttribute("class","move_list");
                        list.setAttribute("value",file_array[i].id);
                        list.innerHTML = file_array[i].name;
                        block.appendChild(list);

                    }
                        var file_block = document.getElementById("file_block");
                        file_block.appendChild(block);
                        file_block.appendChild(check);

                    return 0;
                }
                else{
                    console.log(data);
                    return -1;
                }


            },
            'error':function(xhr,ajaxOptions, thrownError){
            console.log(xhr.status);
            console.log(thrownError);
            }
        });
}
function errMsg(num){
    document.getElementById("file_block").innerHTML  = "<span class='error'>"+num+"</span>";
}
function sortFunction(a,b){
    var dateA = new Date(a.date).getTime();
    var dateB = new Date(b.date).getTime();
    return dateA > dateB ? 1 : -1;
}
//show on website
function newfile(id,filename,type){
        var div = document.createElement("div");
        //console.log("id:"+id+",type:"+type);
        switch(type){
            case "dir":
                    type = "files dir";
                    break;
            case "application/pdf":
                    type = "files pdf";
                    break;
            case "application/vnd.ms-powerpoint":
            case "application/vnd.openxmlformats-officedocument.presentationml.presentation":
                    type = "files ppt";
                    break;
            case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
                    type = "files xls";
                    break;
            case "application/msword":
            case "application/vnd.openxmlformats-officedocument.wordprocessingml.document":
                    type = "files doc";
                    break;
            case "application/zip":
            case "application/x-gzip":
                    type = "files zip";
                    break;
            case "application/json":
                    type = "files json";
                    break;
            case "image/png":
            case "image/jpeg":
                    type = "files image";
                    break;
            case "audio/mp3":
                    type = "files audio";
                    break;
            default:
                    type = "files default";
        }

        div.setAttribute("class",type);
        div.setAttribute("value",id);
        document.getElementById("file_block").appendChild(div);
        div.innerHTML = filename;
        if(type=="files dir"){
                div.addEventListener('dblclick',intodir);
        }

        div.addEventListener('mousedown',control,false);
}
function intodir(e){
        var column = "child";
        console.log("dbclick:"+$(this).text());
        console.log("dir id:"+$(this).attr("value"));
        dir_id = $(this).attr("value");
        var title = document.getElementById("dir_name");
        var path = document.getElementById("dir_config");
        
        var path_link = document.createElement("span");
        path_link.setAttribute("class","path_link");
        path_link.setAttribute("value",dir_id);
        path_link.innerHTML = " > " + $(this).text();
        title.innerHTML = $(this).text();
        path.appendChild(path_link);

        $(".path_link").click(jumpdir);

        $(".files").remove();
        current_dir = $(this).text();//if dir exist
        document.getElementsByName("path")[0].value = current_dir;
        /*
        createFileBlock("LIST","POST",$(this).text(),"normal",page,function(result){
                page = result;
        });
        */
        /*
        createFileBlock("LIST","POST",dir_id,"normal",page,function(result){
                page = result;
        });
        */
        
        createFileBlock("LIST","POST",dir_id,"test",page,column,function(result){
                page = result;
        });
        
        
        
        
}
function jumpdir(e){
        var column = "child";
        $(this).nextAll().remove();
        var gopath = $(this).text().replace(" > ","");
        dir_id = $(this).attr("value");
        var title = document.getElementById("dir_name");
        title.innerHTML = gopath;
        current_dir = gopath;
        
        gopath = gopath.replace("Mydrive","/");

        console.log("jump to :"+gopath);
        console.log("jump to id:"+dir_id);
        /*
        createFileBlock("LIST","POST",dir_id,"normal",page,function(result){
            page = result;
        });
        */
        createFileBlock("LIST","POST",dir_id,"test",page,column,function(result){
                page = result;
        });
        
        
}
//control file
function control(e){
        e.preventDefault();//取消DOM預設事件
        e.stopPropagation(); // Stops some browsers from redirecting.
            $(".blocks").remove();
            switch(e.which){
                case 1://left mouse
                    console.log("1.GET:"+$(this).text());

                    break;
                case 2://middle mouse
                    console.log("2");
                    break;
                case 3://right mouse
                    console.log("3."+$(this).text()+" cor:("+e.pageX+","+e.pageY+")"+" type:"+$(this).attr("class"));
                    var blocks = document.createElement("div");
                    var filename = $(this).text();
                    var type = $(this).attr("class");
                    var id = $(this).attr("value");
                    blocks.setAttribute("class","blocks");
                    blocks.setAttribute("value",filename);

                    this.appendChild(blocks);
                    blocks.setAttribute("style","left:"+e.pageX+"px;top:"+e.pageY+"px");
                    var commands = ['Download','Rename','Delete','Move to'];
                    for(i=0;i<commands.length;i++){
                        if(type=="files dir"){type = "dir"; continue;}//dir will not have "download" command
                        var t = (i*5);
                        var control_block = document.createElement("div");
                        control_block.setAttribute("class","control_block");
                        control_block.setAttribute("value",id);
                        control_block.innerHTML = commands[i];
                        blocks.appendChild(control_block);
                        control_block.setAttribute("style","top:"+t+"px");
                    
                        control_block.addEventListener('mousedown',excute_control,false);
                    }

                    break;
                default:
                    console.log("0");
            }
   //return false;         
}
function control_out(e){
        $(".blocks").remove();
}
function excute_control(e){
        e.preventDefault();//取消DOM預設事件
        e.stopPropagation(); // Stops some browsers from redirecting.
        console.log("excute_control:"+$(this).text());
        console.log("object id:"+$(this).attr("value"));
        var id = $(this).attr("value");
        var filename = $(this).closest('.blocks').attr("value");

        switch($(this).text()){
                case 'Download'://download
                        if($(this).closest('.files').attr("class")!="files dir"){
                            console.log("not dir:"+$(this).closest('.files').attr("class"));
                            download(id,filename);
                        }
                        $(".control_block").remove();
                        break;
                case 'Rename'://rename
                        console.log("dir/file:"+$(this).parents("div.files").attr("class"));
                        var type = $(this).parents("div.files").attr("class");
                        var rename_block = document.createElement("div");
                        var file_block = document.getElementById("file_block");
                        rename_block.setAttribute("class","rename_block");
                        rename_block.setAttribute("value",type);
                        file_block.appendChild(rename_block);

                        var input = document.createElement("input");
                        input.setAttribute("id","newname_block");
                        input.setAttribute("type","text");
                        input.setAttribute("placeholder",filename);
                        var input_id = document.createElement("input");
                        input_id.setAttribute("id","getid");
                        input_id.setAttribute("type","hidden");
                        input_id.setAttribute("value",id);
                        var submit = document.createElement("input");
                        submit.setAttribute("id","rename");
                        submit.setAttribute("type","button");
                        submit.setAttribute("value","ok");
                
                        rename_block.appendChild(input);
                        rename_block.appendChild(input_id);
                        rename_block.appendChild(submit);
                        
                        submit.addEventListener('click',rename,false);
                        //rename(filename);
                        $(".control_block").remove();
                        break;
                case 'Delete'://delete
                    var command="";
                    if($(this).closest('.files').attr("class")!="files dir"){
                        command="DEL";
                    }
                    else{
                        command="DELD";
                    }
                    $(this).closest('.files').remove();
                    sendCommand(command,filename,id,"","filename",dir_id,"","POST",function(result){
                                    console.log(result);               
                    });
                    $(".control_block").remove();
                    break;
                case 'Move to':
                    var column="@dchild";
                    console.log("move to");
                    createFileBlock("LIST","POST",dir_id,"move",page,column,function(result){
                        page = result;
                    });
                    
                    $(".control_block").remove();

        }
}

//odb command
function download(id,name){
    $("#get_filename").val(name);
    $("#get_fileid").val(id);

    document.getElementById('get_form').submit();
}
function rename(e){

    var newfilename = $("input[id='newname_block']").val();
    var filename = $("input[id='newname_block']").attr("placeholder");
    var type = $(".rename_block").attr("value");
    var id = $("input[id='getid']").val();
    $(".rename_block").remove();

    console.log("type:"+type+" new:"+newfilename+" old:"+filename+" id:"+id);
    
    if(filename!=newfilename&&newfilename!=""){
        console.log("type:"+type+" new:"+newfilename+" old:"+filename+" id:"+id);
        if(type=="files dir"){
            console.log("rename dir");
            sendCommand("RENAMED",filename,id,newfilename,"filename","","","POST",function(result){
                console.log(result);
                $(".dir[value='"+id+"']").text(newfilename);
            });
        }
        else{
            console.log("rename file");
            sendCommand("RENAME",filename,id,newfilename,"filename","","","POST",function(result){
                console.log(result);               
                $("div[value='"+id+"']").text(newfilename);
            });
        }
    }
    else{
        console.log("nothing change");
    }
    
}

