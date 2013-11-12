package com.sherncsuk.mymusicmanager;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

public class MainActivity extends Activity {

    //Network Connection Stuffz
    private Socket sock;
    private OutputStream out;
    private DataInputStream inputStream;
    private ByteArrayOutputStream byteStream;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        new NetworkConnection(this).execute();
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

    /**
     * A private inner class to help set up the connection to the server on a different thread
     */

    private class NetworkConnection extends AsyncTask<String, String, String> {
        private MainActivity activity;

        public NetworkConnection(MainActivity activity){
            this.activity = activity;
        }

        @Override
        protected String doInBackground(String[] strings) {
            try {
                sock = new Socket("130.207.114.21", 2222);
            } catch (UnknownHostException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(String str) {
            try {
                out = sock.getOutputStream();
                inputStream = new DataInputStream(sock.getInputStream());
                byteStream = new ByteArrayOutputStream();
            } catch (IOException e){
                e.printStackTrace();
            }
        }
    }
}
