package com.sherncsuk.mymusicmanager;

import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.os.AsyncTask;
import android.os.Build;

import com.sherncsuk.mymusicmanager.DataStructures.Message;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
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


    public void recieveFilenames(Activity activity, Socket sock){
        new FilenameReceiver(activity).execute(sock);
    }


    private class FilenameReceiver extends AsyncTask<Socket, String, String[]> {
        Activity activity;

        public FilenameReceiver(Activity activity){
            this.activity = activity;
        }

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

        @Override
        protected void onPostExecute(String[] filenames) {
            AlertDialog.Builder builder = new AlertDialog.Builder(activity);
            StringBuilder stringBuilder = new StringBuilder();

            for(int i = 0; i < filenames.length; i++){
                stringBuilder.append("File " + (i + 1) + " : " + filenames[i] + "\n");
            }

            builder.setMessage(stringBuilder.toString());
            builder.setTitle("Files");
            (builder.create()).show();
        }
    }

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
