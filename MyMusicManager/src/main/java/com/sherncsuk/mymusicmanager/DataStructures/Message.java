package com.sherncsuk.mymusicmanager.DataStructures;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

/**
 * Author: Ethan Shernan
 * Date: 11/12/13
 * Version: 1.0
 * A data structure to send metadata to the server
 */

public class Message {

    public enum MessageType {
        LEAVE(0), LIST(1), PULL(2), DIFF(3), CAP(4);

        private int val;

        private MessageType(int val){
            this.val = val;
        }

        public int getVal(){
            return val;
        }
    }

    private MessageType type;
    private int numBytes;
    private int lastMessage;
    private int filenameLength;
    private int maxBytes;

    public Message(int numBytes, MessageType type, int lastMessage,
                   int filenameLength, int maxBytes){
        this.numBytes = numBytes;
        this.type = type;
        this.lastMessage = lastMessage;
        this.filenameLength = filenameLength;
        this.maxBytes = maxBytes;
    }

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

    public int getNumBytes() {
        return numBytes;
    }

    public void setNumBytes(int numBytes) {
        this.numBytes = numBytes;
    }

    public MessageType getType() {
        return type;
    }

    public void setType(MessageType type) {
        this.type = type;
    }

    public int isLastMessage() {
        return lastMessage;
    }

    public void setLastMessage(int lastMessage) {
        this.lastMessage = lastMessage;
    }

    public int getFilenameLength() {
        return filenameLength;
    }

    public void setFilenameLength(int filenameLength) {
        this.filenameLength = filenameLength;
    }

    public int getMaxBytes() {
        return maxBytes;
    }

    public void setMaxBytes(int maxBytes) {
        this.maxBytes = maxBytes;
    }

}
