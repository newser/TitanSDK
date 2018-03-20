package com.titansdk.titansdkunittest;

public class TTUnitTestJNI {
    static {
        System.loadLibrary("platform");
    }

    public static native void init();
    public static native void exit();

    public static native String runUT(String name);
}
