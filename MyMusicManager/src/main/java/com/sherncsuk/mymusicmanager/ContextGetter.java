package com.sherncsuk.mymusicmanager;

import android.app.Application;
import android.content.Context;

/**
 * Author: Ethan Shernan
 * Date: 11/19/13
 * Version: 1.0
 */
public class ContextGetter extends Application {
    private static Context context;

    public void onCreate(){
        super.onCreate();
        ContextGetter.context = getApplicationContext();
    }

    public static Context getAppContext() {
        return ContextGetter.context;
    }
}
