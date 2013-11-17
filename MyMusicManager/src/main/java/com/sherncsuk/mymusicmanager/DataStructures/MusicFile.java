package com.sherncsuk.mymusicmanager.DataStructures;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * Author: Ethan Shernan
 * Date: 11/12/13
 * Version: 1.0
 * A mapping between filename and hashed ID of a music file
 */
public class MusicFile {
    private String filename;
    private byte[] ID;
    private int fileSize;

    public MusicFile(String filename, File musicFile) {
        this.filename = filename;
        ID = generateID(musicFile);
    }

    /**
     * Generates the MD5 hashed checksum of the musicfile
     * @param musicFile
     * @return digest
     */

    private byte[] generateID(File musicFile){
        MessageDigest md = null;

        try{
             md = MessageDigest.getInstance("MD5");
        } catch (NoSuchAlgorithmException e){
            e.printStackTrace();
        }

        try {
            InputStream in = new FileInputStream(musicFile);
            DigestInputStream dis = new DigestInputStream(in, md);
            int currByte = dis.read();

            while(currByte != -1){
                currByte = dis.read();
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return md.digest();
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

    public int getFileSize() {
        return fileSize;
    }

    public void setFileSize(int fileSize) {
        this.fileSize = fileSize;
    }
}
