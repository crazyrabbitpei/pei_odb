var page=1;
var current_dir;
$(document).ready(function(){
                var title = document.getElementById("dir_name");
                var path = document.getElementById("dir_config");
                  
                var url  = location.href;
                var dirname = url.split("path=")[1];
                console.log(dirname);
                if(!dirname){
                    title.innerHTML = "Mydrive";
                    dirname = "Mydrive";
                    current_dir = "/";
                    var path_link = document.createElement("span");
                    path_link.setAttribute("class","path_link");
                    path_link.innerHTML = dirname;
                    path.appendChild(path_link);
                }
                else{
                    title.innerHTML = dirname;
                    current_dir = dirname;
                    document.getElementsByName("path")[0].value = current_dir;
                }
                
                createFileBlock("LIST","POST",current_dir,"normal",page,function(result){
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
        
        sendCommand("CDIR",name,"filename",current_dir,"POST",function(result){
            console.log("result:"+result);
            var input= document.getElementsByClassName("files dir");
            var len =document.getElementsByClassName("files dir").length;
            input[len-1].remove();
            var dir = document.createElement("div");
            dir.setAttribute("class","files dir");
            dir.innerHTML = name;
            dir.addEventListener('dblclick',intodir);
            document.getElementById("file_block").appendChild(dir);
            
        });
        
    });
});
$("#upload").click(function(){
    var file = document.getElementsByName('filename')[0];
    upload(file,file.files[0].name,0,1,"single");
});

function sendCommand(command,filename,par,path,method,callback){
    var formData = new FormData();
    formData.append(par,filename);
    formData.append('command',command);
    formData.append('path',path);
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

function createFileBlock(command,method,sfilename,test,page,callback){
        $.ajax({
            'url':'/pei/odb.cgi',
            'data': 'command='+command+'&search='+sfilename+'&page='+page,
            'type': method,
            'success':function(data){
                $(".files").remove();
                if(test!="demo"){
                    var arr = data.split("</br>");
                    console.log(arr);
                    if(arr==""){
                        //errMsg("404");
                    }
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

                var detail=[];
                var key=[];
                var offset=[];
                var dates = [];
                var filenames = [];
                var types = [];
                
                var file_array =new Array();
                for(i=0;i<arr.length-1;i++){
                     var data = arr[i].split(",");
                     key.push(data[0]);
                     offset.push(data[1]);
                     detail.push(data[2]);
                     var filename = data[2].match(/@name:.*/);
                     filename = filename[0].replace("@name:","");
                     var type = data[2].match(/@type:.*/);
                     type = type[0].replace("@type:","");
                    var date = data[2].match(/@ctime:.*/);
                    date = date[0].replace("@ctime:","");
                    file_array.push({name:filename,type:type,date:date});
                }
                //sort file by__(default is date)
                file_array.sort(sortFunction);
                file_array = JSON.stringify(file_array);
                file_array = JSON.parse(file_array);
                for(i=0;i<file_array.length;i++){
                        newfile(file_array[i].name,file_array[i].type);
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
function newfile(filename,type){
        var div = document.createElement("div");
        console.log("type:"+type);
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
        document.getElementById("file_block").appendChild(div);
        div.innerHTML = filename;
        if(type=="files dir"){
                div.addEventListener('dblclick',intodir);
        }

        div.addEventListener('mousedown',control,false);
}
function intodir(e){
        console.log("dbclick:"+$(this).text());
        var title = document.getElementById("dir_name");
        var path = document.getElementById("dir_config");
        
        var path_link = document.createElement("span");
        path_link.setAttribute("class","path_link");
        path_link.innerHTML = " > " + $(this).text();
        title.innerHTML = $(this).text();
        path.appendChild(path_link);

        $(".path_link").click(jumpdir);

        $(".files").remove();
        current_dir = $(this).text();//if dir exist
        document.getElementsByName("path")[0].value = current_dir;
        createFileBlock("LIST","POST",$(this).text(),"normal",page,function(result){
                page = result;
        });
        
}
function jumpdir(e){
        $(this).nextAll().remove();
        var gopath = $(this).text().replace(" > ","");
        
        var title = document.getElementById("dir_name");
        title.innerHTML = gopath;
        current_dir = gopath;
        
        gopath = gopath.replace("Mydrive","/");

        console.log("jump to :"+gopath);
        createFileBlock("LIST","POST",gopath,"normal",page,function(result){
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
                    blocks.setAttribute("class","blocks");
                    this.appendChild(blocks);
                    blocks.setAttribute("style","left:"+e.pageX+"px;top:"+e.pageY+"px");
                    var commands = ['Download','Rename','Delete'];
                    for(i=0;i<3;i++){
                        if(type=="files dir"){type = "dir"; continue;}//dir will not have "download" command
                        var t = (i*5);
                        var control_block = document.createElement("div");
                        control_block.setAttribute("class","control_block");
                        control_block.setAttribute("value",filename);
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
        console.log("filename:"+$(this).attr("value"));
        var filename = $(this).attr("value");

        switch($(this).text()){
                case 'Download'://download
                        if($(this).closest('.files').attr("class")!="files dir"){
                            console.log("not dir:"+$(this).closest('.files').attr("class"));
                            list(filename);
                        }
                        break;
                case 'Rename'://rename
                        //rename(filename);
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
                    sendCommand(command,filename,"filename","","POST",function(result){
                                    console.log(result);               
                    });
                    break;
        }
}

//odb command
function list(filename){
    $("#post_filename").val(filename);
    document.getElementById('get_form').submit();
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

