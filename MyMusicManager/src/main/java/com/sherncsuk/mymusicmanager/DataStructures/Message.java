package com.sherncsuk.mymusicmanager.DataStructures;

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

    private String fileName;
    private MessageType type;
    private boolean lastMessage;
    private int filenameLength;
    private int maxBytes;

    public Message(String fileName, MessageType type, boolean lastMessage,
                   int filenameLength, int maxBytes){
        this.fileName = fileName;
        this.type = type;
        this.lastMessage = lastMessage;
        this.filenameLength = filenameLength;
        this.maxBytes = maxBytes;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public MessageType getType() {
        return type;
    }

    public void setType(MessageType type) {
        this.type = type;
    }

    public boolean isLastMessage() {
        return lastMessage;
    }

    public void setLastMessage(boolean lastMessage) {
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
