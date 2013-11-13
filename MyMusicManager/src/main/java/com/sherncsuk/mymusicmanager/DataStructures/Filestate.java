package com.sherncsuk.mymusicmanager.DataStructures;

/**
 * Author: Ethan Shernan
 * Date: 11/12/13
 * Version: 1.0
 */

public class Filestate {
    private int numFiles;
    private MusicFile[] musicFiles;

    public Filestate(int numFiles, MusicFile[] musicFiles) {
        this.numFiles = numFiles;
        this.musicFiles = musicFiles;
    }

    public MusicFile[] getMusicFiles() {
        return musicFiles;
    }

    public void setMusicFiles(MusicFile[] musicFiles) {
        this.musicFiles = musicFiles;
    }

    public int getNumFiles() {
        return numFiles;
    }

    public void setNumFiles(int numFiles) {
        this.numFiles = numFiles;
    }

}
