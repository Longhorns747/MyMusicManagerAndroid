package com.sherncsuk.mymusicmanager.DataStructures;

/**
 * Author: Ethan Shernan
 * Date: 11/12/13
 * Version: 1.0
 * A data structure to send metadata to the server
 */

public class Message {

    private MessageType type;
    private int numBytes;
    private int lastMessage;
    private int filenameLength;
    private int maxBytes;
    public static final int NUM_MESSAGE_FIELDS = 5;

    public Message(int numBytes, MessageType type, int lastMessage,
                   int filenameLength, int maxBytes){
        this.numBytes = numBytes;
        this.type = type;
        this.lastMessage = lastMessage;
        this.filenameLength = filenameLength;
        this.maxBytes = maxBytes;
    }

    public Message(){}

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

    public enum MessageType {
        LEAVE(0), LIST(1), PULL(2), DIFF(3), CAP(4), LAST_MESSAGE(1), NOT_LAST(0);

        private int val;

        private MessageType(int val){
            this.val = val;
        }

        public int getVal(){
            return val;
        }

        public static MessageType getType(int val){
            switch(val){
                case 0:
                    return LEAVE;
                case 1:
                    return LIST;
                case 2:
                    return PULL;
                case 3:
                    return DIFF;
                case 4:
                    return CAP;
            }

            return null;
        }
    }

}
