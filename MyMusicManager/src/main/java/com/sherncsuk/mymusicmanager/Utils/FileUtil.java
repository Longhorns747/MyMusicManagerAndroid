package com.sherncsuk.mymusicmanager.Utils;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.os.Environment;

import com.sherncsuk.mymusicmanager.DataStructures.Filestate;
import com.sherncsuk.mymusicmanager.DataStructures.MusicFile;

import java.io.File;
import java.util.ArrayList;

/**
 * Author: Ethan Shernan
 * Date: 11/22/13
 * Version: 1.0
 */
public class FileUtil {
    /**
     * A helper method to scan the directory with our music files and update the filestate
     * @param currContext
     * @return currentFileState
     */

    @TargetApi(Build.VERSION_CODES.FROYO)
    public Filestate updateFiles(Context currContext){
        String state = Environment.getExternalStorageState();

        //Check to make sure the external storage is writable
        if (!Environment.MEDIA_MOUNTED.equals(state)) {
            // We can read and write the media
            return null;
        }

        File currDirectory = currContext.getExternalFilesDir(Environment.DIRECTORY_MUSIC);
        File[] fileList = currDirectory.listFiles();
        ArrayList<MusicFile> musicFiles = new ArrayList<MusicFile>();

        for(File file: fileList){
            String extension = "";

            int i = file.getName().lastIndexOf('.');
            if (i > 0) {
                extension = file.getName().substring(i + 1);
            }

            if(extension.equals("mp3")){
                musicFiles.add(new MusicFile(file.getName(), file));
            }
        }

        return new Filestate(musicFiles.size(), musicFiles.toArray(new MusicFile[musicFiles.size()]));
    }
}
