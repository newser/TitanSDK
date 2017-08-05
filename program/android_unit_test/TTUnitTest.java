package com.titansdk;

public class TTUnitTest {
    static {
        System.loadLibrary("platform");
    }

    public static native String runUT(String name);
}

