package com.sherncsuk.mymusicmanager.Utils;

import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.os.AsyncTask;
import android.os.Build;

import com.sherncsuk.mymusicmanager.DataStructures.Filestate;
import com.sherncsuk.mymusicmanager.DataStructures.Message;
import com.sherncsuk.mymusicmanager.DataStructures.MusicFile;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Author: Ethan Shernan
 * Date: 11/14/13
 * Version: 1.0
 */
public class NetworkingUtil {

    /**
     * A static helper method to send a message
     * @param sock
     * @param msg
     */

    public static void sendMessage(Socket sock, Message msg){
        try{
            OutputStream outStream = sock.getOutputStream();
            DataOutputStream out = new DataOutputStream(outStream);
            out.writeInt(msg.getType().getVal());
            out.writeInt(msg.getNumBytes());
            out.writeInt(msg.isLastMessage());
            out.writeInt(msg.getFilenameLength());
            out.writeInt(msg.getMaxBytes());
            out.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * A static helper to receive a message
     * @param sock
     * @return
     */

    @TargetApi(Build.VERSION_CODES.GINGERBREAD)
    public static Message receiveMessage(Socket sock){
        Message res = new Message();

        try {
            DataInputStream inputStream = new DataInputStream(sock.getInputStream());
            ByteArrayOutputStream byteStream = new ByteArrayOutputStream();

            int field = 0;
            while(field < Message.NUM_MESSAGE_FIELDS){
                for(int i = 0; i < 4; i++){
                    byte retData = inputStream.readByte();
                    byteStream.write(retData);
                }

                byte result[] = Arrays.copyOf(byteStream.toByteArray(), 4);
                byteStream.reset();

                int val = byteArrayToInt(result);

                switch(field){
                    case 0:
                        res.setType(Message.MessageType.getType(val));
                        break;
                    case 1:
                        res.setNumBytes(val);
                        break;
                    case 2:
                        res.setLastMessage(val);
                        break;
                    case 3:
                        res.setFilenameLength(val);
                        break;
                    case 4:
                        res.setMaxBytes(val);
                        break;
                }

                field++;
            }
        } catch (IOException e){
            e.printStackTrace();
        }

        return res;
    }

    /**
     * A method that recieves the filenames of all music files on the server
     * @param activity
     * @param sock
     */

    public void recieveFilenames(Activity activity, Socket sock){
        new FilenameReceiver(activity).execute(sock);
    }

    private class FilenameReceiver extends AsyncTask<Socket, String, String[]> {
        Activity activity;

        public FilenameReceiver(Activity activity){
            this.activity = activity;
        }

        /**
         * Recieves filenames on a seperate thread from the UI
         * @param sock
         * @return
         */

        @TargetApi(Build.VERSION_CODES.GINGERBREAD)
        @Override
        protected String[] doInBackground(Socket... sock) {
            ArrayList<String> res = new ArrayList<String>();
            Message currMessage = receiveMessage(sock[0]);
            DataInputStream inputStream;
            ByteArrayOutputStream byteStream;

            try {
                inputStream = new DataInputStream(sock[0].getInputStream());
                byteStream = new ByteArrayOutputStream();

                //Keep receiving messages until we hit the last message
                while(currMessage.isLastMessage() == Message.MessageType.NOT_LAST.getVal()){

                    int i = 0;
                    //Keep receiving bytes until we get the last byte
                    while(i < currMessage.getFilenameLength()){
                        int retData = inputStream.readUnsignedByte();
                        byteStream.write(retData);
                        i++;
                    }

                    byte result[] = Arrays.copyOf(byteStream.toByteArray(), currMessage.getFilenameLength());
                    byteStream.reset();

                    res.add(new String(result));

                    currMessage = receiveMessage(sock[0]);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            return res.toArray(new String[res.size()]);
        }

        /**
         * Displays the received filenames in a dialog box
         * @param filenames
         */

        @Override
        protected void onPostExecute(String[] filenames) {
            AlertDialog.Builder builder = new AlertDialog.Builder(activity);
            StringBuilder stringBuilder = new StringBuilder();

            for(int i = 0; i < filenames.length; i++){
                stringBuilder.append("File " + (i + 1) + " : " + filenames[i] + "\n");
            }

            if(filenames.length == 0)
                stringBuilder.append("All files are the same!");

            builder.setMessage(stringBuilder.toString());
            builder.setTitle("Files");
            (builder.create()).show();
        }
    }

    /**
     * A method that sends the MD5 checksums of all files in a filestate to the server
     * @param state
     * @param sock
     */

    public void sendIDs(Filestate state, Socket sock){
        try{
            OutputStream outStream = sock.getOutputStream();
            DataOutputStream out = new DataOutputStream(outStream);

            for(MusicFile file: state.getMusicFiles()){
                Message metadata = new Message(file.getID().length, Message.MessageType.NOT_LAST,
                        Message.MessageType.NOT_LAST.getVal(), 0, -1);

                sendMessage(sock, metadata);

                out.write(file.getID());
                out.flush();
            }

            Message last = new Message(0, Message.MessageType.LAST_MESSAGE,  Message.MessageType.LAST_MESSAGE.getVal(),
                    0, -1);
            sendMessage(sock, last);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Static helper method to convert a byte array to an int
     * @param b
     * @return
     */

    //http://stackoverflow.com/questions/5399798/byte-array-and-int-conversion-in-java
    public static int byteArrayToInt(byte[] b)
    {
        int value = 0;
        for (int i = 0; i < 4; i++) {
            int shift = (4 - 1 - i) * 8;
            value += (b[i] & 0x000000FF) << shift;
        }
        return value;
    }

}
