package com.sherncsuk.mymusicmanager.DataStructures;

import java.io.File;

/**
 * Author: Ethan Shernan
 * Date: 11/12/13
 * Version: 1.0
 * A mapping between filename and hashed ID of a music file
 */
public class MusicFile {
    private String filename;
    private byte[] ID;

    public MusicFile(String filename, File musicFile) {
        this.filename = filename;
        ID = generateID(musicFile);
    }

    private byte[] generateID(File musicFile){
        return null;
    }

    public String getFilename() {
        return filename;
    }

    public void setFilename(String filename) {
        this.filename = filename;
    }

    public byte[] getID() {
        return ID;
    }
}
