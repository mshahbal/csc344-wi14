import com.musicg.wave.Wave;
import com.musicg.wave.WaveFileManager;
import java.util.*;
import java.nio.ByteBuffer;

public class Asgn5 {
   public static final int BUFFER_SIZE = 44100;
   public static final int SCREW = 10000;

   public static void main(String[] args) {
      Wave wave;
      Wave newWave;
      byte[] trackData; // data of the music represented in bytes
      byte[] screwed; // slowed down data of the music
      byte temp;
      int endPointer = SCREW; // points to end of sample
      int tracker=0, bpm=0, bufferSize=0;
      String trackName;
      
      
      // GET INFO
      System.out.println("Please type in the name of the track you wish to Chop & Screw: ");
      Scanner in = new Scanner(System.in);
      trackName = in.nextLine();
      
      wave = new Wave(trackName + ".wav");
      trackData = wave.getBytes();
      
      /* made it arbitrarily 3 times larger than trackData */
      screwed = new byte[trackData.length * 3];
      
      System.out.println("Please enter the tempo of the track: ");
      bpm = in.nextInt();
      bufferSize = (BUFFER_SIZE * bpm) / 60;
      
      System.out.println("Thank you! Please wait while we Chop & Screw your track...");
      
      //Quickrun
      /*trackName = "Break the Bank";
      wave = new Wave(trackName + ".wav");
      trackData = wave.getBytes();
      bpm = 80;
      bufferSize = (BUFFER_SIZE * bpm) / 60;*/
      
      
      // CHOP
      for (int i=0; i < wave.size(); i++) {
         
         /* Do this to skip intro of track (commonly 8 bars in rap) */
         if (i > (bufferSize * 32)) {
      
            /* Checks to see if the i var is at the end of the desired beat */
            if (((i/bufferSize) + (bufferSize*8)) % 16 == 0) {
            
               /* As long as we don't go past the end of the track, go two
                  samples ahead and add that sample to the previous one
                  (will be adding to the NEXT sample after i)*/
               for (int j=0; j < bufferSize; j++) {
                  if ((i+j+bufferSize) < trackData.length)
                     trackData[i+j] = trackData[i+j+bufferSize];
               }         
               i+=bufferSize;
            }
         }
      }
      System.out.println("...");
      
      
      // SCREW
      while (endPointer < trackData.length) {
         
         for (int j=0; j < SCREW; j++) {
            // TRIED SCALING THE DATA, made it worse
            /*if (j < 40) {
               int loc = endPointer - (SCREW - j);
               
               if (j % 4 == 0) {
                  byte[] temp2 = {trackData[loc], trackData[loc+1], 
                     trackData[loc+2], trackData[loc+3]};
                     
                  ByteBuffer wrapped = ByteBuffer.wrap(temp2);
                  int sample = wrapped.getInt() * (j/40);
                  
                  ByteBuffer b = ByteBuffer.allocate(4);
                  b.putInt(sample);
                  temp2 = b.array();
                  for (int k=0; k < 4; k++) {
                     screwed[tracker] = temp2[k];
                     tracker++;
                  }
               }
            }
            else if (j > (SCREW - 40)) {
               int loc = endPointer - (SCREW - j);
               
               if (j % 4 == 0) {
                  byte[] temp2 = {trackData[loc], trackData[loc+1], 
                     trackData[loc+2], trackData[loc+3]};
                     
                  ByteBuffer wrapped = ByteBuffer.wrap(temp2);
                  int sample = wrapped.getInt() * (j/40);
                  
                  ByteBuffer b = ByteBuffer.allocate(4);
                  b.putInt(sample);
                  temp2 = b.array();
                  for (int k=0; k < 4; k++) {
                     screwed[tracker] = temp2[k];
                     tracker++;
                  }
               }
            }
            else*/
            
            // TRIED INTERLEAVING BYTES, but got no sound
            /*if (j < 300) {
               first[j] = trackData[endPointer - (SCREW - j)];
            }
            
            if (tracker2 > 0) {
               if (n < 150) {
                  screwed[tracker++] = first[n];
                  screwed[tracker] = last[n];
                  n++;
               }
            }
            else {
               screwed[tracker] = trackData[endPointer - (SCREW - j)];
            }
            
            
            if (j > (SCREW - 300)) {
               last[j - (SCREW - 300)] = trackData[endPointer - (SCREW - j)];
            }*/
            
            screwed[tracker++] = trackData[endPointer - (SCREW - j)];
         }
         /* Advance the sample by sample size * .7 */
         endPointer += SCREW * .7;
      }
      System.out.println("...");
      
      
      // EXPORT
      newWave = new Wave(wave.getWaveHeader(), screwed);
      
      WaveFileManager fileManager = new WaveFileManager(newWave);
      fileManager.saveWaveAsFile(trackName + " - Chopped & Screwed.wav");
      
      System.out.println("Done!");
   }
}