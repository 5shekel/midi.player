// Play a file from the SD card in looping mode, from the SD card.
// Example program to demonstrate the use of the MIDFile library
//
// Hardware required:
//	SD card interface - change SD_SELECT for SPI comms

#include <SPI.h>
#include <SdFat.h> 
#include <MD_MIDIFile.h>

byte byteRead;
int tempo = 78;

#define	USE_MIDI	1

#if USE_MIDI // set up for direct MIDI serial output
  #define DEBUGS(s)
  #define	DEBUG(s, x)
  #define	DEBUGX(s, x)
  #define	SERIAL_RATE	31250
#else // don't use MIDI to allow printing debug statements
  #define DEBUGS(s)     Serial.print(s)
  #define	DEBUG(s, x)	  { Serial.print(F(s)); Serial.print(x); }
  #define	DEBUGX(s, x)	{ Serial.print(F(s)); Serial.print(x, HEX); }
  #define	SERIAL_RATE	57600
#endif // USE_MIDI


#define  SD_SELECT  10
#define	ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
char *loopfile = "katzen.mid";  // simple and short file

SdFat	SD;
MD_MIDIFile SMF;

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
#if USE_MIDI
	if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
	{
		Serial.write(pev->data[0] | pev->channel);
		Serial.write(&pev->data[1], pev->size-1);
	}
	else
		Serial.write(pev->data, pev->size);
#endif
  DEBUG("\nM T", pev->track);
  DEBUG(":  Ch ", pev->channel+1);
  DEBUGS(" Data");
  for (uint8_t i=0; i<pev->size; i++)
  {
	  DEBUGX(" ", pev->data[i]);
  }
}

void setup(void)
{
  int  err;

  Serial.begin(SERIAL_RATE);

  DEBUGS("\n[MidiFile Looper]");

  // Initialize SD
  if (!SD.begin(SD_SELECT, SPI_FULL_SPEED))
  {
    DEBUGS("\nSD init fail!");
    while (true) ;
  }

  // Initialize MIDIFile
  SMF.begin(&SD);
  SMF.setMidiHandler(midiCallback);
  SMF.looping(true);

  dump();
  SMF.setFilename(loopfile);
  err = SMF.load();
  /*
        Serial.println();
        Serial.print("getTimeSignature() ");
        Serial.print(highByte(SMF.getTimeSignature())); Serial.print("/");
        Serial.println(lowByte(SMF.getTimeSignature()));

        Serial.println();
        Serial.print("getTempo() ");
        Serial.print(SMF.getTempo()); Serial.print("/");
        */

  if (err != -1)
  {
    DEBUG("\nSMF load Error ", err);
    while (true);
  }
}

void loop(void)
{
  /*
  if (Serial.available()) {
    //read the most recent byte 
    byteRead = Serial.read();
    if(byteRead == 45 ) //asci -
      tempo -= 10;
    else if(byteRead == 43) //asci +
      tempo += 10;
    SMF.setTempoAdjust(tempo);
  }
  */

	// play the file
	if (!SMF.isEOF())
	{    
    //Serial.print("getTempo(): ");     Serial.println(SMF.getTempo());
		SMF.getNextEvent();
	}
}













void dump(void)
{
  DUMP("\nFile Name:\t", SMF.getFilename());
  DUMP("\nFile format:\t", SMF.getFormat());
  DUMP("\nTracks:\t\t", SMF.getTrackCount());
  DUMP("\nTime division:\t", SMF.getTicksPerQuarterNote());
  DUMP("\nTempo:\t\t", SMF.getTempo());
  DUMP("\nMicrosec/tick:\t", SMF.getTickTime());
  DUMP("\nTime Signature:\t", SMF.getTimeSignature()>>8);
  DUMP("/", SMF.getTimeSignature() & 0xf);
  DUMPS("\n");
 

}