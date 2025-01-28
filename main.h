#include <avr/io.h>
#include "avr.h"
#include "lcd.h"
#include <stdio.h>

float note_freqs[] = {
	440.0, 466.2, 493.9, 523.3, 554.4, 587.3, 622.3, 659.3, 698.5, 740.0,
	784.0, 830.6, 880.0, 932.3, 987.8, 1046.5, 1108.7, 1174.7, 1244.5, 1318.5,
	1396.9, 1480.0, 1568.0, 1661.2
};

float note_periods[] = {
	0.002273, 0.002145, 0.002025, 0.001910, 0.001804, 0.001703, 0.001607, 0.001517,
	0.001432, 0.001351, 0.001276, 0.001204, 0.001136, 0.001073, 0.001012, 0.000956,
	0.000902, 0.000851, 0.000803, 0.000758, 0.000716, 0.000676, 0.000638, 0.000602
};

float note_TH[] = {
	0.001136, 0.001073, 0.001012, 0.000955, 0.000902, 0.000851, 0.000803, 0.000758,
	0.000716, 0.000676, 0.000638, 0.000602, 0.000568, 0.000536, 0.000506, 0.000478,
	0.000451, 0.000426, 0.000402, 0.000379, 0.000358, 0.000338, 0.000319, 0.000301
};

int wait_unit[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int duration_divisors[] = {1, 2, 4, 8}; // Whole, Half, Quarter, Eighth




typedef enum {
	A, AS, B, C, CS, D, DS, E, F, FS, G, GS, Ab, ASb, Bb, Cb, CSb, Db, DSb, Eb, Fb, FSb, Gb, GSb
} Note;

typedef enum {
	W, H, Q, Ei
} Duration;

typedef struct {
	Note note;
	Duration duration;
} PlayingNote;



PlayingNote shooting_stars[] = {
	{DS, W},
	{DS, H},
	{E, H},
	{B, Q},
	{GS, Q}
	
};

PlayingNote anthem[] = {
	{C, Q}, {E, Q}, {G, Q}, {C, H},
	{E, Q}, {G, Q}, {A, Q}, {F, H},
	{E, Q}, {G, Q}, {A, Q}, {B, H},
	{C, Q}, {E, Q}, {G, Q}, {C, H}
	
};

PlayingNote birthday[] = {
	{C, Q}, {C, Ei}, {D, H}, {C, H}, {F, H}, {E, W},
	{C, Q}, {C, Ei}, {D, H}, {C, H}, {G, H}, {F, W},
	{C, Q}, {C, Ei}, {C, H}, {A, H}, {F, H}, {E, H}, {D, W},
	{AS, Q}, {AS, Ei}, {A, H}, {F, H}, {G, H}, {F, W}
	
};


volatile int play_song_flag = 0;  


void play_note(const PlayingNote* note) {
	float period = note_periods[note->note]; 
	float half_period = period / 2.0; 
	int duration_ms = 1000 / duration_divisors[note->duration]; 
	int cycles = duration_ms / (half_period * 1000.0);  

	for (int i = 0; i < cycles; i++) {
		if (play_song_flag == 0) {
			break;  
		}
		SET_BIT(PORTB, 3);
		avr_wait(period * 1000);
		CLR_BIT(PORTB, 3);
		avr_wait(period * 1000);
	}
}

const char keypad[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

int is_pressed(int r, int c) {
	// Set all pins to input with pull-ups disabled to start clean
	DDRC = 0x00;
	PORTC = 0x00;

	// Set the row 'r' as an output and drive it low
	DDRC |= (1 << r);    // Set row pin 'r' as output
	PORTC &= ~(1 << r);  // Set row pin 'r' to LOW

	// Set the column 'c' as an input with pull-up resistor enabled
	DDRC &= ~(1 << (c+4));  // Ensure column pin 'c+4' is set as input
	PORTC |= (1 << (c+4));  // Enable pull-up resistor on column pin 'c+4'

	// Wait a moment for the pin levels to stabilize
	avr_wait(5);

	// Read the column pin to check if the key is pressed
	// Since pull-up resistors are enabled, a pressed key will connect the row to the column, pulling it LOW
	if ((PINC & (1 << (c+4))) == 0) {
		return 1;  // Key is pressed if the column pin is LOW
	}
	return 0;  // No key is pressed
}

int get_key() {
	int i, j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			if (is_pressed(i, j)) {
				return (i * 4) + j+1;
			}
		}
	}
	return 0; // Indicate no key pressed
}

char convert_key(int k) {
	if (k == 0) return '0'; 
	return keypad[(k-1)/4][(k-1)%4];
}


typedef struct {
	PlayingNote* notes;
	int length;
	const char* name;
} Song;

Song songs[] = {
	{birthday, sizeof(birthday) / sizeof(birthday[0]), "Birthday"},
	{shooting_stars, sizeof(shooting_stars) / sizeof(shooting_stars[0]), "Shooting Stars"},
		{anthem, sizeof(anthem) / sizeof(anthem[0]), "Anthem"}
};


void play_song(const Song* song) {
	lcd_clr();
	lcd_pos(0, 0);
	lcd_puts2("Playing:");
	lcd_puts2(song->name);

	for (int i = 0; i < song->length; i++) {
		play_note(&song->notes[i]);
		if (get_key() == (0*4 + 4)) {  
			play_song_flag = 0;  
			lcd_clr();
			lcd_pos(0, 0);
			lcd_puts2("Not playing");
			break;
		}

		if (play_song_flag == 0) {
			break; 
		}
		avr_wait(50);  
	}

	if (play_song_flag) {
		lcd_clr();
		lcd_pos(0, 0);
		lcd_puts2("Not playing");
	}
}


void handle_keypad_input() {
	int int_key = get_key();  
	char key = convert_key(int_key);

	switch (key) {
		case '1':
		// Play birthday song
		if (!play_song_flag) {
			play_song_flag = 1;
			play_song(&songs[0]);
		}
		break;
		case '2':
		// Play shooting stars
		if (!play_song_flag) {
			play_song_flag = 1;
			play_song(&songs[1]);
		}
		break;
		case '3':
		// Play national anthem
		if (!play_song_flag) {
			play_song_flag = 1;
			play_song(&songs[2]);
		}
		break;
		case 'A':
		// Stop playing any song
		play_song_flag = 0;
		lcd_clr();
		lcd_pos(0, 0);
		lcd_puts2("Not playing");
		break;
		default:
		lcd_clr();
		lcd_pos(0, 0);
		lcd_puts2("Not playing");
		break;
	}
	avr_wait(200); 
}


int main(void) {
	DDRB = (1 << PB3);
	PORTB = 0;
	lcd_init();
	lcd_clr();
	lcd_pos(0, 0);
	lcd_puts2("Ready");
	while (1) {
		handle_keypad_input();  
		avr_wait(100);
	}

	return 0;
}
