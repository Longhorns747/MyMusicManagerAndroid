package com.sherncsuk.mymusicmanager;

import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.View;
import android.widget.TextView;

import com.sherncsuk.mymusicmanager.DataStructures.Filestate;
import com.sherncsuk.mymusicmanager.DataStructures.Message;
import com.sherncsuk.mymusicmanager.DataStructures.MusicFile;
import com.sherncsuk.mymusicmanager.Utils.NetworkingUtil;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class MainActivity extends Activity {

    //Network Connection Stuffz
    private Socket sock;
    private OutputStream out;
    private DataInputStream inputStream;
    private ByteArrayOutputStream byteStream;
    private boolean connected = false;
    private Context currentContext;

    //Utilities
    private NetworkingUtil networkingUtil;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        new NetworkConnection(this).execute();

        networkingUtil = new NetworkingUtil();
        currentContext = getApplicationContext();
    }

    @Override
    protected void onStop() {
        leave(null);
        super.onStop();
    }

    @Override
    protected void onRestart() {
        new NetworkConnection(this).execute();
        super.onRestart();
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
        if(connected){
            sendInitialMessage(Message.MessageType.LIST);
            networkingUtil.recieveFilenames(this, sock);
        }
    }

    /**
     * Calculates the diff from files on the server to the Android directory
     * @param v
     */

    public void diff(View v){
        if(connected){
            sendInitialMessage(Message.MessageType.DIFF);
            Filestate currentState = updateFiles(currentContext);
            networkingUtil.sendIDs(currentState, sock);
            networkingUtil.recieveFilenames(this, sock);
        }
    }

    /**
     * Pulls diff'd files from the server and saves them to the Android directory
     * @param v
     */

    @TargetApi(Build.VERSION_CODES.FROYO)
    public void pull(View v){
        if(connected){
            sendInitialMessage(Message.MessageType.PULL);
            File currDirectory = currentContext.getExternalFilesDir(null);
            Filestate currState = updateFiles(currentContext);
            networkingUtil.sendIDs(currState, sock);
            networkingUtil.receiveMusicFiles(this, currDirectory, sock);
        }
    }

    /**
     * Pulls most popular songs from the server up to a certain file cap
     * @param v
     */

    public void cap(View v){
        if(connected)
            sendInitialMessage(Message.MessageType.CAP);
    }

    /**
     * Leaves the session
     * @param v
     */

    public void leave(View v){
        if(connected){
            sendInitialMessage(Message.MessageType.LEAVE);

            try{
                sock.close();
                inputStream.close();
                out.close();
            } catch (IOException e){
                e.printStackTrace();
            }

            connected = false;
            updateConnectedLabel(connected);
        }
    }

    /**
     * Reconnects to the session
     * @param v
     */

    public void reconnect(View v){
        if(!connected){
            new NetworkConnection(this).execute();
        }
    }

    /**
     * Sends the initial control message to the server
     * @param type
     */

    public void sendInitialMessage(Message.MessageType type){
        Message initialMessage = new Message(0, type,
                Message.MessageType.LAST_MESSAGE.getVal(), 0, 0);
        NetworkingUtil.sendMessage(sock, initialMessage);
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
                sock = new Socket("130.207.114.21", 2223);
            } catch (UnknownHostException e) {
                connected = false;
                e.printStackTrace();
                return null;
            } catch (IOException e) {
                connected = false;
                e.printStackTrace();
                return null;
            }

            connected = true;
            return null;
        }

        @Override
        protected void onPostExecute(String str) {
            if(connected){
                try {
                    out = sock.getOutputStream();
                    inputStream = new DataInputStream(sock.getInputStream());
                    byteStream = new ByteArrayOutputStream();
                } catch (IOException e){
                    e.printStackTrace();
                }
            }
            else{
                AlertDialog.Builder builder = new AlertDialog.Builder(activity);
                builder.setMessage("Couldn't connect to the server :(\nSlap Ethan!");
                (builder.create()).show();
            }

            updateConnectedLabel(connected);
        }
    }

    /**
     * A helper method to scan the directory with our music files and update the filestate
     * @param currContext
     * @return currentFileState
     */

    @TargetApi(Build.VERSION_CODES.FROYO)
    public Filestate updateFiles(Context currContext){
        String state = Environment.getExternalStorageState();

        //Check to make sure the external storage is writable
        if (!Environment.MEDIA_MOUNTED.equals(state)) {
            // We can read and write the media
            return null;
        }

        File currDirectory = currContext.getExternalFilesDir(null);
        File[] fileList = currDirectory.listFiles();
        ArrayList<MusicFile> musicFiles = new ArrayList<MusicFile>();

        for(File file: fileList){
            String extension = "";

            int i = file.getName().lastIndexOf('.');
            if (i > 0) {
                extension = file.getName().substring(i + 1);
            }

            if(extension.equals("mp3")){
                musicFiles.add(new MusicFile(file.getName(), file));
            }
        }

        return new Filestate(musicFiles.size(), musicFiles.toArray(new MusicFile[musicFiles.size()]));
    }

    /**
     * A helper method to update the connected state label
     * @param connected
     */

    public void updateConnectedLabel(boolean connected){
        if(connected)
            ((TextView)findViewById(R.id.connectedState)).setText("Connected!");
        else
            ((TextView)findViewById(R.id.connectedState)).setText("Not Connected :(");
    }
}
