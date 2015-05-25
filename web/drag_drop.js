
var fileUpload = document.querySelectorAll('body').item(0);
fileUpload.addEventListener('drop', uploadDrop , false);
fileUpload.addEventListener('dragover', handleDragOver, false);
//fileUpload.addEventListener('dragenter', handleDragEnter, false);
//fileUpload.addEventListener('dragleave', handleDragLeave, false);

function uploadDrop(e) {
    e.stopPropagation(); // Stops some browsers from redirecting.
    e.preventDefault();
    if(e.dataTransfer.types.indexOf('Files') == -1){
        message("not Files");
        return;
    }
    var files = e.dataTransfer.files;
    upload(files,files[0].name,0,files.length,"muti");
}
function handleDragOver(e) {
      e.stopPropagation(); // Stops some browsers from redirecting.
      e.preventDefault();
}
function handleDragEnter(e) {
      e.stopPropagation(); // Stops some browsers from redirecting.
        e.preventDefault();
}
function handleDragLeave(e) {
      e.stopPropagation(); // Stops some browsers from redirecting.
        e.preventDefault();
}
function upload(files,name,cnt,len,type){ 
    // Read the File objects in this FileList.
                var formData = new FormData();
                var filename;
                console.log(type);
                if(type=="muti"){
                  console.log("files:"+name);
                  formData.append('filename',files[cnt]);
                }
                else{
                  console.log("files:"+name);
                  formData.append('filename',files.files[0]);
                }
                  formData.append('command',"PUT");
                  if(current_dir=="Mydrive"){
                          current_dir = "/";
                  }
                  formData.append('path',current_dir);
                  $.ajax({
                    'url':'/pei/odb.cgi',
                    'data':formData,
                    'processData': false,  // tell jQuery not to process the data
                    'contentType': false,  // tell jQuery not to set contentType
                    'type':'POST',
                    'success':function(data){
                        //console.log(data);
                        var div = document.getElementById("upload_status");
                        div.innerHTML+=data;
                        var contenttype = data.match(/The alleged content type of the file was:.*/);
                        contenttype = contenttype[0].replace("The alleged content type of the file was:","");
                        contenttype = contenttype.replace("<p>","");
                        cintenttype = contenttype.replace(" ","");
                        
                        newfile(name,type);
                        
                        cnt++;
                        if(cnt==len){return;}
                        upload(files,files[cnt].name,cnt,len,type);
                    },
                    'error':function(xhr,ajaxOptions, thrownError){
                        console.log(xhr.status);
                        console.log(thrownError);
                    }
                  })

    
}
