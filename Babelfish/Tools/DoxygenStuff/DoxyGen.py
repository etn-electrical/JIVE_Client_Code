# This is python Script to generate doxygen package 
import os
import sys
import pathlib

import os.path
import shutil
from configparser import ConfigParser

listdir = []
print("...................................start...........................................")
#to find current directory path
Direct = os.path.dirname(os.path.realpath(__file__))
Direct2 = Direct+'\..\..\Code'
print("Direct="+ Direct)
print("Direct2="+ Direct2)

Basedirname = os.path.basename(os.path.realpath(Direct+'\..\..'))

print("Basedirname="+ Basedirname)
#os.system("dir %s"%Direct2)

path = os.path.realpath(Direct2)
print("path="+ path)

name = "Doxygen_Generater.bat"
doxy = 'C:\\Program Files\\doxygen\\bin\\doxygen.exe'
BatchfilePAth = ".\Doxygen_Generater.bat"


#create output folder if doesnt exist
ArgPath = sys.argv[1]
print("ArgPath="+ArgPath)
#os.system("dir %s"%ArgPath)
# Check whether the specified path exists or not
isExist = os.path.exists(ArgPath)

if not isExist:
  
  # Create a new directory because it does not exist 
  os.makedirs(ArgPath)
  #print("Output directory is created!")

#move the html files from tools to the output diretory

htmlindexpath = Direct+ "\Babelfish_index.html"
htmltreepath = Direct+ "\Babelfish_Tree.html"
cssetnpath = Direct+ "\etn_doxygen.css"

#print("htmlindexpath ="+htmlindexpath)
#print("htmltreepath ="+htmltreepath)
#print("cssetnpath ="+cssetnpath)

shutil.copy(htmlindexpath, ArgPath)
shutil.copy(htmltreepath, ArgPath)
shutil.copy(cssetnpath, ArgPath)
print("files get moved to output folder")



#This code will travrese through all the foler in Babelfish\code
# This will check if .cpp or .h file is present in folder or not.
# if present than it will copy the batch file in each folder 
# if any folder not containg .cpp or .h file. 
#this folder get skipeed and doxygen document will not generate for this particular folder
for root, dirs, files in os.walk(path):
    if name in files:
        listdir.append(os.path.join(root, name))
        
    else:
        if '\ThirdParty' in root:
            #print("Doxygen is not cretaed for this folder")
            #This code will skip doxygen if also folder contain .cpp or .h file and still want to skip the folder
            continue
        else:   
            ext = [".cpp",".h"]
            for cppfilename in os.listdir(root):
                if cppfilename.endswith(tuple(ext)):
                    #print(" file name = "+cppfilename) 
                    shutil.copy(BatchfilePAth,root+'\\')
                    listdir.append(os.path.join(root, name)) 
                    break

for Count in range(len(listdir)):
    #print("file:"+listdir[Count]+"\n")
    modulepath = os.path.dirname(os.path.realpath(listdir[Count]))
    print("modulepath= :"+modulepath+"\n")
    modulename= os.path.basename(modulepath)
    print("ModuleNAme="+modulename+"\n")
    
    Count_folder =0
    tree_level_count=0
    modulepath1 = modulepath
    for tree_level_count in range(6):
        if( os.path.basename(modulepath1) == Basedirname):
            # print("found=")
            break
        else:
            #print("base="+os.path.basename(modulepath1))
            Count_folder =Count_folder+1
            modulepath1 = modulepath1 + "\.."
            modulepath1 = os.path.realpath(modulepath1)
            #print("modulepath1="+modulepath1)
            
            
    RelativePathvar = ""        
    #print(Count_folder)
    for Folder_level_count  in range(Count_folder):
        RelativePathvar = RelativePathvar + "..\\"
    # print("RelativePathvar="+RelativePathvar)




    
    # This will create the docs folder in Module folder 
    # create Configuration file with name ModuleName_doxy.
    # update this file with required configuration
    filepath = modulepath+'\\docs\\'+ modulename + '_Doxy'
    isExist = os.path.exists(filepath)
    if not isExist:
        # Create a new directory because it does not exist 
        os.system("mkdir %s\docs"%modulepath)
        print("Configuration file is created")
        #os.system("mkdir %s\docs"%modulepath)
        os.system('"' + doxy + '"' + ' -g ' +modulepath+ '\\docs\\' + modulename+'_doxy')
        filepath = modulepath+'\\docs\\'+ modulename + '_Doxy'
        print("path= "+filepath+"\n")
        file1 = open(filepath,"r+")
        defaultString = ['PROJECT_NAME           = "My Project"',
                        'PROJECT_LOGO           =', 
                        'OUTPUT_DIRECTORY       =',
                        'CREATE_SUBDIRS         = NO',
                        'JAVADOC_AUTOBRIEF      = NO',
                        'BUILTIN_STL_SUPPORT    = NO',
                        'EXTRACT_ALL            = NO',
                        'EXTRACT_PRIVATE        = NO',
                        'EXTRACT_STATIC         = NO',
                        'EXTRACT_LOCAL_METHODS  = NO',
                        'EXTRACT_ANON_NSPACES   = NO',
                        'HIDE_FRIEND_COMPOUNDS  = NO',
                        'HIDE_IN_BODY_DOCS      = NO',
                        'CASE_SENSE_NAMES       = NO',
                        'INLINE_INFO            = YES',
                        'SORT_MEMBER_DOCS       = YES',
                        'GENERATE_TESTLIST      = YES',
                        'WARN_NO_PARAMDOC       = NO',
                        'INPUT                  =',
                        'EXAMPLE_PATH           =',
                        'IMAGE_PATH             =',
                        'USE_MDFILE_AS_MAINPAGE =',
                        'SOURCE_BROWSER         = NO',
                        'HTML_TIMESTAMP         = NO',
                        'GENERATE_TREEVIEW      = NO',
                        'FULL_SIDEBAR           = NO',
                        'GENERATE_LATEX         = YES',
                        'HAVE_DOT               = NO',
                        'UML_LOOK               = NO',
                        'TEMPLATE_RELATIONS     = NO',
                        'CALL_GRAPH             = NO',
                        'CALLER_GRAPH           = NO',
                        'DOT_MULTI_TARGETS      = NO',
                        ]

        UpdatedString = ['PROJECT_NAME           = '+modulename+"\n",
                        'PROJECT_LOGO           ='+RelativePathvar+"Tools\DoxygenStuff\Eaton_Green_DIP_Logo.png"+" "+ "\\"+"\n",
                        'OUTPUT_DIRECTORY       ='+ "." +"\n",
                        'CREATE_SUBDIRS         = YES'+"\n",
                        'JAVADOC_AUTOBRIEF      = YES'+"\n",
                        'BUILTIN_STL_SUPPORT    = YES'+"\n",
                        'EXTRACT_ALL            = YES'+"\n",
                        'EXTRACT_PRIVATE        = YES'+"\n",
                        'EXTRACT_STATIC         = YES'+"\n",
                        'EXTRACT_LOCAL_METHODS  = YES'+"\n",
                        'EXTRACT_ANON_NSPACES   = YES'+"\n",
                        'HIDE_FRIEND_COMPOUNDS  = YES'+"\n",
                        'HIDE_IN_BODY_DOCS      = YES'+"\n",
                        'CASE_SENSE_NAMES       = YES'+"\n",
                        'INLINE_INFO            = NO'+"\n",
                        'SORT_MEMBER_DOCS       = NO'+"\n",
                        'GENERATE_TESTLIST      = NO'+"\n",
                        'WARN_NO_PARAMDOC       = YES'+"\n",
                        'INPUT                  = '+"." +" "+"\\"+"\n"+"documents"+" "+"\\"+"\n",
                        'EXAMPLE_PATH           = '+RelativePathvar+"Examples"+" "+"\n",
                        'IMAGE_PATH             = '+"Image"+"\n",
                        'USE_MDFILE_AS_MAINPAGE = ',
                        'SOURCE_BROWSER         = YES'+"\n",
                        'HTML_TIMESTAMP         = YES'+"\n",
                        'GENERATE_TREEVIEW      = YES'+"\n",
                        'FULL_SIDEBAR           = YES'+"\n",
                        'GENERATE_LATEX         = NO'+"\n",
                        'HAVE_DOT               = YES'+"\n",
                        'UML_LOOK               = YES'+"\n",
                        'TEMPLATE_RELATIONS     = YES'+"\n",
                        'CALL_GRAPH             = YES'+"\n",
                        'CALLER_GRAPH           = YES'+"\n",
                        'DOT_MULTI_TARGETS      = YES'+"\n",
                        ]
        y = len(defaultString)-1
        lines = file1.readlines()
        data = []
        i = 0
        for line in lines:   
            if defaultString[i] in line:
                #print("Found =")
                #line.replace(defaultString[i],UpdatedString[i])
                data.append(UpdatedString[i])
                if i < y:
                    i = i + 1          
            else:
                data.append(line)
                file1.close()
        file2 = open(filepath,"w+")
        file2.writelines(data)
        file2.close()
        print("Configuration File updated")
    else:
        print("Configuration File allready exist")
    
    # This  code will trigger the Doxygen_Generater.bat batch file
    # it will generate the doxygen document 
    print("trigger doxy generation batch file")
    os.system(modulepath+"\\"+'Doxygen_Generater.bat')

    
    # copy the generated doxygen document to the destination directory path 
    # this path is predefined in Doxygen_package_main.bat file
    # if user provide path as command line argument than this path will be used otherwise predefined path will be used

    source_dir = modulepath+'\\html'
    #print("==========================================")
    #print("modulepath="+modulepath)
    #print("modulename="+modulename)

    TreePath = modulepath.split('\Code')
    #print("TreePath[1]= "+TreePath[1])
    ArgPath = sys.argv[1]
    #print("ArgPath="+ArgPath)
    OuputFolderTreePath = ArgPath+ TreePath[1]
    #print("modulepath="+modulepath)
    #print("modulename="+modulename)
    #print("OuputFolderTreePath="+OuputFolderTreePath)
    #print("ArgPath="+ArgPath)
    
    isExist = os.path.exists(OuputFolderTreePath)
    if not isExist:
        # Create a new directory because it does not exist 
        os.makedirs(OuputFolderTreePath)
        #print("The new directory is created!")
    
    
    # This will copy the created file to the output destination folder
    #ArgPath = sys.argv[1]
    #print("ArgPath="+ArgPath)
    #print("htmlfileName="+htmlfileName)
    destination_dir = OuputFolderTreePath +'\\html' 
    print("moving doxyouput to output folder")
    #print("destination_dir="+destination_dir)
    isExist = os.path.exists(destination_dir)
    if not isExist:
        #print("new destination_dir directory is created!")
        #os.makedirs(destination_dir)
        shutil.move(source_dir,destination_dir)
        #shutil.rmtree(source_dir)
    else:
        shutil.rmtree(destination_dir)
        #os.makedirs(destination_dir)
        shutil.move(source_dir,destination_dir)
        #shutil.rmtree(source_dir)
        #print("Dirctory Exist")
print("Complete the python file")

     
