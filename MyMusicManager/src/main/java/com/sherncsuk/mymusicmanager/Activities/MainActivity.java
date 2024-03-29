package com.sherncsuk.mymusicmanager.Activities;

import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.sherncsuk.mymusicmanager.DataStructures.Filestate;
import com.sherncsuk.mymusicmanager.DataStructures.Message;
import com.sherncsuk.mymusicmanager.R;
import com.sherncsuk.mymusicmanager.Utils.FileUtil;
import com.sherncsuk.mymusicmanager.Utils.NetworkingUtil;

import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

public class MainActivity extends Activity {

    //Network Connection Stuffz
    private Socket sock;
    private OutputStream out;
    private DataInputStream inputStream;
    private boolean connected = false;
    private Context currentContext;

    //Utilities
    private NetworkingUtil networkingUtil;
    private FileUtil fileUtil;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        networkingUtil = new NetworkingUtil();
        networkingUtil.connect(this);
        fileUtil = new FileUtil();
        currentContext = getApplicationContext();
    }

    @Override
    protected void onStop() {
        leave(null);
        super.onStop();
    }

    @Override
    protected void onRestart() {
        networkingUtil.connect(this);
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
            networkingUtil.sendInitialMessage(sock, Message.MessageType.LIST);
            networkingUtil.recieveFilenames(this, sock);
        }
    }

    /**
     * Calculates the diff from files on the server to the Android directory
     * @param v
     */

    public void diff(View v){
        if(connected){
            networkingUtil.sendInitialMessage(sock, Message.MessageType.DIFF);
            Filestate currentState = fileUtil.updateFiles(currentContext);
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
            networkingUtil.sendInitialMessage(sock, Message.MessageType.PULL);
            File currDirectory = currentContext.getExternalFilesDir(Environment.DIRECTORY_MUSIC);
            Filestate currState = fileUtil.updateFiles(currentContext);
            networkingUtil.sendIDs(currState, sock);
            networkingUtil.receiveMusicFiles(this, currDirectory, sock, Message.MessageType.PULL);
        }
    }

    /**
     * Pulls most popular songs from the server up to a certain file cap
     * @param v
     */

    @TargetApi(Build.VERSION_CODES.FROYO)
    public void cap(View v){
        if(connected){
            AlertDialog.Builder alert = new AlertDialog.Builder(this);

            alert.setTitle("Cap");
            alert.setMessage("Enter Cap in MB");

            // Set an EditText view to get user input
            final EditText input = new EditText(this);
            alert.setView(input);

            alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                    int numBytes = (int)(Double.parseDouble(input.getText().toString()) * 1048576);
                    networkingUtil.sendInitialMessage(sock, Message.MessageType.CAP, numBytes);
                    File currDirectory = currentContext.getExternalFilesDir(Environment.DIRECTORY_MUSIC);
                    Filestate currState = fileUtil.updateFiles(currentContext);
                    networkingUtil.sendIDs(currState, sock);
                    networkingUtil.receiveMusicFiles(MainActivity.this, currDirectory, sock, Message.MessageType.CAP);
                }
            });

            alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                    // Canceled.
                }
            });

            alert.show();
        }
    }

    /**
     * Leaves the session
     * @param v
     */

    public void leave(View v){
        if(connected){
            networkingUtil.sendInitialMessage(sock, Message.MessageType.LEAVE);

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
            networkingUtil.connect(this);
        }
    }

    /**
     * A helper method to update the connected state label
     * @param connected
     */

    public void updateConnectedLabel(boolean connected){
        if(connected)
            ((TextView)findViewById(R.id.connectedState)).setText(R.string.connected);
        else
            ((TextView)findViewById(R.id.connectedState)).setText(R.string.not_connected);
    }

    /**
     * Standard getters and setters
     */

    public Socket getSock() {
        return sock;
    }

    public void setSock(Socket sock) {
        this.sock = sock;
    }

    public void setOut(OutputStream out) {
        this.out = out;
    }

    public void setInputStream(DataInputStream inputStream) {
        this.inputStream = inputStream;
    }

    public void setConnected(boolean connected) {
        this.connected = connected;
    }

    public boolean getConnected() {
        return connected;
    }
}
