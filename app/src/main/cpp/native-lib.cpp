#include <jni.h>
#include <string>
#include "native_lib.h"
#include "../../../../../../environment/android/sdk/ndk-bundle/sources/cxx-stl/llvm-libc++/include/cstring"

#include <unistd.h>
#include <android/log.h>
#include <sys/inotify.h>
#include <stdlib.h>

int m_child;
const char *PATH="/data/data/com.create.whc.processguard/my.sock";
const char *userId;

void child_do_work() {
    //开启socket
    if (child_creat_channel()){
        child_listen_msg();
    }

}
/**
 * 服务端读取信息
 */
void child_listen_msg() {
    fd_set rfds;
    struct timeval timeout={3,0};
    while (1){
        //清空内容
        FD_ZERO(&rfds);
        FD_SET(m_child,&rfds);
        int r=select(m_child+1,&rfds,NULL,NULL,&timeout);
        LOGE("读取消息前 %d",r);
        if (r>0){
            //缓冲区
            char pkg[256]={0};
            //保证所读到的信息是指定apk客户端
            if (FD_ISSET(m_child,&rfds)){
                //阻塞式函数 只读状态，客服端未发送任何消息
                int result=read(m_child,&pkg, sizeof(pkg));
                //开启服务
                execlp("am","am","startservice","--user",userId,
                       "com.create.whc.processguard/com.create.whc.processguard.ProcessService",(char*)NULL);
                break;
            }
        }

    }

}

/**
 * 创建服务端的socket
 * @return
 */
int child_creat_channel() {
    //三个参数：协议域 socket类型
    int listenfd=socket(AF_LOCAL,SOCK_STREAM,0);
    unlink(PATH);
    //addr --> 内存区域
    struct sockaddr_un addr;
    //清空先前创建的
    unlink(PATH);
    //清空内存
    memset(&addr,0, sizeof(sockaddr_un));
    addr.sun_family=AF_LOCAL;

    int connfd=0;
    strcpy(addr.sun_path,PATH);
    if (bind(listenfd,(const sockaddr *)&addr, sizeof(sockaddr_un))<0){
        LOGE("绑定错误");
        return 0;
    }
    //第二个参数：最大监听数
    listen(listenfd,5);
    //保证 宿主进程链接成功
    while (1){
        // 返回客户端的地址 阻塞式函数
        if ((connfd=accept(listenfd,NULL,NULL))<0){
            if (errno==EINTR){
                continue;
            }else{
                LOGE("读取错误！");
                return 0;
            }
        }
        m_child=connfd;
        LOGE("apk 父进程链接上了 %d",m_child);
        break;
    }
    return 1;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_create_whc_processguard_Watcher_creatWatcher(JNIEnv *env, jobject instance,
                                                      jstring userId_) {
    userId = env->GetStringUTFChars(userId_, 0);

    //开双进程
    pid_t pid=fork();
    if(pid<0){

    } else if (pid==0){
        //子进程 守护
        child_do_work();

    } else if (pid>0){
        //父进程
    }

    env->ReleaseStringUTFChars(userId_, userId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_create_whc_processguard_Watcher_connectMonitor(JNIEnv *env, jobject instance) {

    //客服端
    int socked;
    //addr --> 内存区域
    struct sockaddr_un addr;
    while (1){
        LOGE("父进程  父进程开始链接");
        socked=socket(AF_LOCAL,SOCK_STREAM,0);
        if (socked<0){
            LOGE("链接失败");
            return;
        }
        //清空内存
        memset(&addr,0, sizeof(sockaddr_un));
        addr.sun_family=AF_LOCAL;
        strcpy(addr.sun_path,PATH);

        if (connect(socked,(const sockaddr *)&addr, sizeof(sockaddr_un))<0){
            LOGE("链接失败");
            close(socked);
            sleep(1);
            // 再来下一次尝试链接
            continue;
        }
        LOGE("链接成功");
        break;
    }

}