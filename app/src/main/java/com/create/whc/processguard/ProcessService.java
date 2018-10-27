package com.create.whc.processguard;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import android.support.annotation.Nullable;
import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

public class ProcessService extends Service {

    private final String TAG="ProcessService";
    private int i=0;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Watcher watcher=new Watcher();
        watcher.creatWatcher(String.valueOf(Process.myUid()));
        watcher.connectMonitor();
        Timer timer=new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.i(TAG,"服务开启中"+i);
                i++;
            }
        },0,3*1000);
    }
}
