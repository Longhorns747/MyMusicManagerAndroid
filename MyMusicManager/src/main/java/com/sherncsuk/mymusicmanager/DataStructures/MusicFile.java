package com.sherncsuk.mymusicmanager.DataStructures;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
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
        try {
            InputStream in = new FileInputStream(musicFile);
            int filesize = 0;

            //Seems as though when I make the buffer bigger, it goes faster :O
            if(musicFile.length() < Integer.MAX_VALUE)
                filesize = (int)musicFile.length(); //Let's make it as big as the file!
            else
                filesize = Integer.MAX_VALUE;

            byte[] buffer = new byte[filesize];
            MessageDigest complete = MessageDigest.getInstance("MD5");
            int numRead;

            do {
                numRead = in.read(buffer);
                if (numRead > 0) {
                    complete.update(buffer, 0, numRead);
                }
            } while (numRead != -1);

            in.close();
            return complete.digest();

        } catch (NoSuchAlgorithmException e){
            e.printStackTrace();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

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

    public int getFileSize() {
        return fileSize;
    }

    public void setFileSize(int fileSize) {
        this.fileSize = fileSize;
    }
}
