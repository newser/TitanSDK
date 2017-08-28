package com.titansdk.titansdkunittest;

public class TTUnitTestJNI {
    static {
        System.loadLibrary("platform");
    }

    public static native String runUT(String name);
}
