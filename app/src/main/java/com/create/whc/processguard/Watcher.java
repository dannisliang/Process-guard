package com.create.whc.processguard;

public class Watcher {

    static {
        System.loadLibrary("native-lib");
    }

    public native void creatWatcher(String userId);

    public native void connectMonitor();

}
