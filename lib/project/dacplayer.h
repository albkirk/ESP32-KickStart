//  - - - libraries
#include <math.h>

//  - - -  Constants
#define SPEAKER_PIN 25


//  - - -  Variables  - - -
byte last_Volume = 0;
byte min_ampl=9;        // Max is 8 bits - 1 Byte
byte max_ampl=245;      // Max is 8 bits - 1 Byte



//  - - -  Functions  - - -
// Setup the Timed interrupt
bool player_stop() {
  if (TL_COUNT>=1 || TR_COUNT >=1) {      // Were Touch Buttons pressed?
    TL_COUNT = 0;
    //TL_STATUS = false;
    TR_COUNT = 0;
    //TR_STATUS = false;
    return true;
  }
  else return false;
}

void player_mute() {
    dacWrite(SPEAKER_PIN, 127);
}

void player_fadein(byte amplitude = 127, int sample_rate = 22050) {
    unsigned int delay_interval = ((unsigned long)1000000/sample_rate);
    if (amplitude > 127) {
        for(byte fadein=127; fadein < amplitude; fadein++) {
            dacWrite(SPEAKER_PIN, fadein);
            delayMicroseconds(delay_interval);
        }
    }
    else {
        for(byte fadein=127; fadein > amplitude; fadein--) {
            dacWrite(SPEAKER_PIN, fadein);
            delayMicroseconds(delay_interval);
        }

    }
}

void player_fadeout(byte amplitude = 127, int sample_rate = 22050) {
    unsigned int delay_interval = ((unsigned long)1000000/sample_rate);
    if (amplitude > 127) {
        for(byte fadeout=amplitude; fadeout>=127; fadeout--) {
            dacWrite(SPEAKER_PIN, fadeout);
            delayMicroseconds(delay_interval);
        }
    }
    else {
        for(byte fadeout=amplitude; fadeout<=127; fadeout++) {
            dacWrite(SPEAKER_PIN, fadeout);
            delayMicroseconds(delay_interval);
        }

    }
}


void player_tone(float freq, float tempo, byte ampl=config.Volume, byte bpm=120, uint32_t spl_rate = 22050) {
    TL_COUNT = 0;
    ampl = (byte)ampl/100.0*(max_ampl - min_ampl)/2;     // To convert volume [0-100%] to amplitude [0-128]
    if (ampl > 128) ampl = 128;
    if (ampl < 0) ampl = 0;
    uint32_t delay_intv = ((uint32_t)1000000/spl_rate);
    uint32_t Num_of_samples = (tempo * 60 / bpm * spl_rate);
    float sin_const = 2*PI*freq*delay_intv*0.000001;
    uint32_t n = 0;
    byte sample = 0;
    player_fadein((byte)((ampl * sin(2*PI*freq*n*delay_intv*0.000001))+127), spl_rate);
    for (n = 0; n < Num_of_samples; n++) {
        sample = (byte)( ampl * sin(sin_const*n)) + 127;
        dacWrite(SPEAKER_PIN, sample);
        delayMicroseconds(delay_intv-16);     // 16 --> to compensate the time used to calculate the "sample" value
    }
    player_fadeout((byte)((ampl * sin(2*PI*freq*n*delay_intv*0.000001))+127), spl_rate);
    player_mute();
}

void player_beepup(int ntimes = 2) {
    for (size_t i = 0; i < ntimes; i++) {
        player_tone(A6, 1.0/16);
        player_tone(0, 1.0/16);
        player_tone(B6, 1.0/8);
        player_tone(0, 1.0/2);
        if (player_stop()) i = ntimes;
    };
}

void player_beepdn(int ntimes = 2) {
    for (size_t i = 0; i < ntimes; i++) {
        player_tone(A6, 1.0/8);
        player_tone(0, 1.0/16);
        player_tone(G6, 1.0/16);
        player_tone(0, 1.0/2);
        if (player_stop()) i = ntimes;
    };
}

void player_beep(int ntimes = 2) {
    for (size_t i = 0; i < ntimes; i++) {
        player_tone(A6*2, 1.0/8);
        player_tone(0, 1.0/8);
        player_tone(0, 1.0/4);
        if (player_stop()) i = ntimes;
    };
}

int player_music(const uint8_t* music_data, int spl_rate=22050) {
    TL_COUNT = 0;
    int output = 0;
    uint32_t delay_intv = ((uint32_t)1000000/spl_rate);
    uint32_t Num_of_samples = strlen((char*)music_data)-1;
    Serial.println("music_data lenght: " + String(Num_of_samples));
    if(config.Volume != 0) {
        size_t i = 0;
        player_fadein((byte)((music_data[i]-127)*(config.Volume/100.0)+127), spl_rate);
        for(i=0; i < Num_of_samples; i++) {
            if (player_stop()) {
                output=-1;
                goto stop_music;
            };
            dacWrite(SPEAKER_PIN, (byte)((music_data[i]-127)*(config.Volume/100.0)+127));
            delayMicroseconds(delay_intv-16);
        };
        output=1;
        stop_music:;
        player_fadeout((byte)((music_data[i]-127)*(config.Volume/100.0)+127), spl_rate);
        player_mute();
    };
    return output;
}

int player_playfile(fs::FS &fs, const char * path, int spl_rate=22050) {
  TL_COUNT = 0;
  int output = 0;
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if(!file){
      Serial.println("Failed to open file for reading");
      return output = -2;
  }
  if(config.Volume != 0) {
      uint32_t delay_intv = ((uint32_t)1000000/spl_rate);
      uint32_t Num_of_samples = file.size();
      Serial.println("Music file lenght: " + String(Num_of_samples));
      byte sample = file.read();
      player_fadein((byte)((sample-127)*(config.Volume/100.0)+127), spl_rate);
      for(size_t i=0; i < Num_of_samples-1; i++) {
          if (player_stop()) {
                output=-1;
                goto stop_music;
          };
          sample = file.read();
          dacWrite(SPEAKER_PIN, (byte)((sample-127)*(config.Volume/100.0)+127));
          delayMicroseconds(delay_intv-20);
      };
      output=1;
      stop_music:;
      player_fadeout((byte)((sample-127)*(config.Volume/100.0)+127), spl_rate);
      player_mute();
  };
  file.close();
  return output;
}


void player_play (byte idx = 0) {
  // sounds[] = {"Birds", "Ocean", "Crickets", "music box", "Waterfall", "Startup", "WakeUp", "Cucu"};
  switch (idx) {
        case 0 :
            for (size_t i = 0; i < 3; i++) {
                //if (player_music(Nightingale) == -1) i = 3;
                if (player_playfile(SD, "/Nightingale.raw") == -1) i = 3;
            }
            break;
        case 1 :
            for (size_t i = 0; i < 3; i++) {
                //if (player_music(Ocean) == -1) i = 3;
                if (player_playfile(SD, "/Ocean.raw") == -1) i = 3;
            }
            break;
        case 2 :
            for (size_t i = 0; i < 5; i++) {
                //if (player_music(Crickets) == -1) i = 5;
                if (player_playfile(SD, "/Cricket.raw") == -1) i = 5;
            }
            break;
        case 3 :
            for (size_t i = 0; i < 3; i++) {
                if (player_music(MusicBox) == -1) i = 3;
                //if (player_playfile(SD, "/Music-box.raw") == -1) i = 3;
            }
            break;
        case 4 :
            player_beepdn(60);
            break;
        case 5 :
            player_playfile(SD, "/1KHz_30sec.raw");
            //player_music(startup_sound);
            break;
        case 6 :
            player_beepup(60);
            break;
        case 7 :
            for (size_t i = 0; i < 5; i++) {
                if (player_music(Cuckoo) == -1) i = 5;
                delay(600);
            }
            break;
        default :
            player_beep(6);
  }



}

void player_setup() {
    player_mute();
    //player_beep(3);
    //player_music(startup_sound, 22500);
    /*for (size_t i = 0; i < 7962; i++) {
      Serial.print(String(cuckoowav[i]) + ", ");
      if(i%16 == 0) Serial.println("");
    }
    */

}


void player_loop() {
    /*if (last_Volume != config.Volume) {
        myDFPlayer.Volume(config.Volume);  //Set volume value [0~100%].
    }*/
    /*
    if(A_STATUS == true) player_tone(C4, 1);
    if(B_STATUS == true) player_tone(D4, 1);
    if(C_STATUS == true) player_tone(E4, 1);
    */
}
