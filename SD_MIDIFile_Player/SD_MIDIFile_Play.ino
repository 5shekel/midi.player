// Test playing a succession of MIDI files from the SD card.

#include <SdFat.h> 
#include <MD_MIDIFile.h>

#define	USE_MIDI	1
#define deadbug 0

#if USE_MIDI // set up for direct MIDI serial output

	#define	DEBUG(x)
	#define	DEBUGX(x)
	#define	SERIAL_RATE	31250

#else // don't use MIDI to allow printing debug statements

	#define	DEBUG(x)	Serial.print(x)
	#define	DEBUGX(x)	Serial.print(x, HEX)
	#define	SERIAL_RATE	57600

#endif // USE_MIDI

#define  SD_SELECT  10

// LED definitions for user indicators
#define	READY_LED		  7	// when finished
#define SMF_ERROR_LED	6	// SMF error
#define SD_ERROR_LED	5	// SD error
#define	BEAT_LED		  6	// toggles to the 'beat'

#define	WAIT_DELAY	20	// ms

#define	ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
char *tuneList[] = 
{
	"katzen.mid",  // simplest and shortest file
	//"2001.mid",
};

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
  DEBUG("\nM T");
  DEBUG(pev->track);
  DEBUG(":  Ch ");
  DEBUG(pev->channel+1);
  DEBUG(" Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
	DEBUGX(pev->data[i]);
    DEBUG(' ');
  }
}


void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
	midi_event	ev;

	// All sound off
	// When All Sound Off is received all oscillators will turn off, and their volume
	// envelopes are set to zero as soon as possible.
	ev.size = 0;
	ev.data[ev.size++] = 0xb0;
	ev.data[ev.size++] = 120;
	ev.data[ev.size++] = 0;

	for (ev.channel = 0; ev.channel < 16; ev.channel++)
		midiCallback(&ev);
}

void setup(void)
{
  // Set up LED pins
  pinMode(READY_LED, OUTPUT);
  pinMode(SD_ERROR_LED, OUTPUT);
  pinMode(SMF_ERROR_LED, OUTPUT);

  Serial.begin(SERIAL_RATE);

  DEBUG("\n[MidiFile Play List]");

  // Initialize SD
  if (!SD.begin(SD_SELECT, SPI_FULL_SPEED))
  {
    DEBUG("\nSD init fail!");
    digitalWrite(SD_ERROR_LED, HIGH);
    while (true) ;
  }

  // Initialize MIDIFile
  SMF.begin(&SD);
  SMF.setMidiHandler(midiCallback);
  digitalWrite(READY_LED, HIGH);
}

void tickMetronome(void)
// flash a LED to the beat
{
	static uint32_t	lastBeatTime = 0;
	static boolean	inBeat = false;
	uint16_t	beatTime;

	beatTime = 60000/SMF.getTempo();		// msec/beat = ((60sec/min)*(1000 ms/sec))/(beats/min)
	if (!inBeat)
	{
		if ((millis() - lastBeatTime) >= beatTime)
		{
			lastBeatTime = millis();
			digitalWrite(BEAT_LED, HIGH);
			inBeat = true;
		}
	}
	else
	{
		if ((millis() - lastBeatTime) >= 100)	// keep the flash on for 100ms only
		{
			digitalWrite(BEAT_LED, LOW);
			inBeat = false;
		}
	}

}

void loop(void)
{
    int  err;
	
	for (uint8_t i=0; i<ARRAY_SIZE(tuneList); i++)
	{
	  // reset LEDs
	  digitalWrite(READY_LED, LOW);
	  digitalWrite(SD_ERROR_LED, LOW);

	  // use the next file name and play it
    DEBUG("\nFile: ");
    DEBUG(tuneList[i]);
	  SMF.setFilename(tuneList[i]);
	  	
	  SMF.setTimeSignature(1,1);
	  if(deadbug){
	  	Serial.println();
	  	Serial.print("post getTimeSignature() ");
	  	Serial.print(highByte(SMF.getTimeSignature())); Serial.print("/");
	  	Serial.println(lowByte(SMF.getTimeSignature()));
	  }

	  err = SMF.load();
	  if (err != -1)
	  {
		DEBUG("\nSMF load Error ");
		DEBUG(err);
		digitalWrite(SMF_ERROR_LED, HIGH);
		delay(WAIT_DELAY);
	  }
	  else
	  {
	  	SMF.setTimeSignature(1,1);
	  	if(deadbug){	
		  	Serial.println();
		  	Serial.print("getTimeSignature() ");
		  	Serial.print(highByte(SMF.getTimeSignature())); Serial.print("/");
		  	Serial.println(lowByte(SMF.getTimeSignature()));
			//while(1){}
	  	}
		// play the file
		while (!SMF.isEOF())
		{
			if (SMF.getNextEvent())
			tickMetronome();
		}

		// done with this one
		SMF.close();
		midiSilence();

		// signal finish LED with a dignified pause
		digitalWrite(READY_LED, HIGH);
		delay(WAIT_DELAY);
	  }
	}
}

