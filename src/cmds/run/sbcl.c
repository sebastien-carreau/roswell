#include <stdio.h>
#include "util.h"
#include "opt.h"

char** cmd_run_sbcl(int argc,char** argv,struct sub_command* cmd)
{
  char** arg=NULL;
  char* home=homedir();
  char* arch=uname_m();
  char* os=uname();
  char* impl=(char*)cmd->name;
  char* version=(char*)cmd->short_name;
  int offset=7; /*[binpath for sbcl] --noinform --core param
                  --no-sysinit --no-userinit [terminating NULL] that total 7 are default. */
  int i;
  char* impl_path= cat(home,"impls",SLASH,arch,SLASH,os,SLASH,impl,SLASH,version,NULL);
  char* help=get_opt("help");
  char* script=get_opt("script");
  char* image=get_opt("image");
  char* program=get_opt("program");
  char* dynamic_space_size=get_opt("dynamic-space-size");
  char* dynamic_stack_size=get_opt("dynamic-stack-size");
  char* sbcl_version=get_opt("version");
  int paramc=0;
  char *bin= cat(impl_path,SLASH,"bin",SLASH,"sbcl",
#ifdef _WIN32
           ".exe",
#endif
           NULL);
  s(arch),s(os);
  if(help) {
    offset++;
  }
  if(dynamic_space_size)
    offset+=2;
  if(dynamic_stack_size)
    offset+=2;
  if(sbcl_version)
    offset+=1;
  if(script)
    offset+=1;
  if(program||script)
    offset+=4;

  arg=alloc(sizeof(char*)*(offset+argc));
  arg[paramc++]=bin;
  /* runtime options from here */
  arg[paramc++]=q("--core");
  if(!image) {
    arg[paramc++]=cat(
#ifdef _WIN32
               //"\"",
#endif
               impl_path,SLASH,"lib",SLASH,"sbcl",SLASH,"sbcl.core",
#ifdef _WIN32
               //"\"",
#endif
               NULL);
  }else {
    arg[paramc++]=q(image);
  }
  if(help) {
    arg[paramc++]=q("--help");
  }
  arg[paramc++]=q("--noinform");

  if(dynamic_space_size) {
    arg[paramc++]=q("--dynamic-space-size");
    arg[paramc++]=q(dynamic_space_size);
  }
  if(dynamic_stack_size) {
    arg[paramc++]=q("--dynamic-stack-size");
    arg[paramc++]=q(dynamic_stack_size);
  }
  if(sbcl_version) {
    arg[paramc++]=q("--version");
  }
  /* runtime options end here */
  for(i=1;i<argc;++i) {
    arg[paramc++]=argv[i];
  }
  arg[paramc++]=q("--no-sysinit");
  arg[paramc++]=q("--no-userinit");
  if(script)
    arg[paramc++]=q("--disable-debugger");

  if(program || script) {
    char* lisp_path=lispdir();
    arg[paramc++]=q("--load");
    arg[paramc++]=s_cat2(lisp_path,q("init.lisp"));
    arg[paramc++]=q("--eval");
    arg[paramc++]=cat("(ros:run '(",program?program:"",script?"(:script ":"",script?script:"",script?")":"",script?"(:quit ())":"","))",NULL);
  }

  s(impl_path);

  arg[paramc]=NULL;
  if(verbose>0) {
    fprintf(stderr,"args=");
    for(i=0;arg[i]!=NULL;++i)
      fprintf(stderr,"%s ",arg[i]);
    fprintf(stderr,"\nhelp=%s ",help?"t":"nil");
    fprintf(stderr,"script=%s\n",script?script:"nil");
  }
  s(image),s(help),s(script),s(dynamic_space_size),s(dynamic_stack_size);
  return arg;
}