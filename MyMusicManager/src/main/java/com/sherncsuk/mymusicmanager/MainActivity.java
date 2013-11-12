package com.sherncsuk.mymusicmanager;

import android.app.Activity;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    /**
     * A method that lists the files currently on a server
     * @param v
     */

    public void list(View v){

    }

    /**
     * Calculates the diff from files on the server to the Android directory
     * @param v
     */

    public void diff(View v){

    }

    /**
     * Pulls diff'd files from the server and saves them to the Android directory
     * @param v
     */

    public void pull(View v){

    }

    /**
     * Pulls most popular songs from the server up to a certain file cap
     * @param v
     */

    public void cap(View v){

    }

    /**
     * Leaves the session
     * @param v
     */

    public void leave(View v){

    }
}
