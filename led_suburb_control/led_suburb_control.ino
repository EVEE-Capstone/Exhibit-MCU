#include <FastLED.h>
// MAX MEMORY: 540 pixels

// STRIP DEFINITIONS

#define PIXEL_OFF 0

#define ERROR_R 25
#define ERROR_G 0
#define ERROR_B 0

#define HOUSE_R 255
#define HOUSE_G 230
#define HOUSE_B 0

#define FLOW_R 0
#define FLOW_G 155
#define FLOW_B 60

#define CHARGE_R 0
#define CHARGE_G 255
#define CHARGE_B 0

// Main Neighborhood Line
#define STRIP_0 0
#define STRIP_0_PIN 2
#define STRIP_0_LENGTH 239
CRGB strip0[STRIP_0_LENGTH];

// North Branch? 
#define STRIP_1 1
#define STRIP_1_PIN 3
#define STRIP_1_LENGTH 58
CRGB strip1[STRIP_1_LENGTH];

// South Branch?
#define STRIP_2 2
#define STRIP_2_PIN 4
#define STRIP_2_LENGTH 21
CRGB strip2[STRIP_2_LENGTH];

// Unknown?
#define STRIP_3 3
#define STRIP_3_PIN 5
#define STRIP_3_LENGTH 11
CRGB strip3[STRIP_3_LENGTH];

// Microgrid House?
#define STRIP_4 4
#define STRIP_4_PIN 6
#define STRIP_4_LENGTH 14
CRGB strip4[STRIP_4_LENGTH];

// House?
#define STRIP_5 5
#define STRIP_5_PIN 7
#define STRIP_5_LENGTH 14
CRGB strip5[STRIP_5_LENGTH];

// Unknown?
#define STRIP_6 6
#define STRIP_6_PIN 8
#define STRIP_6_LENGTH 44
CRGB strip6[STRIP_6_LENGTH];

// House D
#define STRIP_7 7
#define STRIP_7_PIN 9
#define STRIP_7_LENGTH 7
CRGB strip7[STRIP_7_LENGTH];

#define NUM_STRIPS 8


#define OFF 0
#define OUTAGE 1
#define INFLOW 2
#define OUTFLOW 3
#define HOUSE 4
#define SYSERROR 5
#define HOUSE_CHARGE_LIGHT 6
#define STATION_CHARGE_LIGHT 7
#define GARAGE_CHARGE_LIGHT 8

#define EXHIBIT_OFF 0
#define REGULAR_GRID 1
#define POWER_OUTAGE 2 // everything actually off til car arrives at house
#define NANO_GRID 3
#define MICRO_GRID 4
#define ERROR_STATE 5
#define HOUSE_CHARGE 6
#define GARAGE_CHARGE 7
#define STATION_CHARGE 8
#define OUTAGE_CHARGE 9 // car arrives -> light up house and what not

// ****************** Animation Definitions ******************

#define STRIP_EYE 4
#define STRIP_GAP 8

#define ANIM_LOOPS 12
#define PULSE_ANIM_LOOPS 36

// used to control light frequency
#define ANIM_FREQ 25 // is a delay in ms between frames

// used to lower the frequency of error light blinking from every 12 ms to every 24
bool loop_flipper = 0;
int pulse_anim_frame = 0;

// setup() function -- runs once at startup --------------------------------

void setup() {
  Serial.begin(9600);
  
  FastLED.addLeds<WS2812, STRIP_0_PIN, GRB>(strip0, STRIP_0_LENGTH);
  FastLED.addLeds<WS2812, STRIP_1_PIN, GRB>(strip1, STRIP_1_LENGTH);
  FastLED.addLeds<WS2812, STRIP_2_PIN, GRB>(strip2, STRIP_2_LENGTH);
  FastLED.addLeds<WS2812, STRIP_3_PIN, GRB>(strip3, STRIP_3_LENGTH);
  FastLED.addLeds<WS2812, STRIP_4_PIN, GRB>(strip4, STRIP_4_LENGTH);
  FastLED.addLeds<WS2812, STRIP_5_PIN, GRB>(strip5, STRIP_5_LENGTH);
  FastLED.addLeds<WS2812, STRIP_6_PIN, GRB>(strip6, STRIP_6_LENGTH);
  FastLED.addLeds<WS2812, STRIP_7_PIN, GRB>(strip7, STRIP_7_LENGTH);
}


// loop() function -- runs repeatedly as long as board is on ---------------

int exhibit_state = EXHIBIT_OFF;
char data_rx;
void loop() {
  if(Serial.available()){
    data_rx = Serial.read();
    if(data_rx == '0')
      exhibit_state = EXHIBIT_OFF;
    if(data_rx == '1')
      exhibit_state = REGULAR_GRID;
    if(data_rx == '2')
      exhibit_state = POWER_OUTAGE;
    if(data_rx == '3')
      exhibit_state = NANO_GRID;
    if(data_rx == '4')
      exhibit_state = MICRO_GRID;
    if(data_rx == '5')
      exhibit_state = ERROR_STATE;
    if(data_rx == '6')
      exhibit_state = HOUSE_CHARGE;
    if(data_rx == '7')
      exhibit_state = GARAGE_CHARGE;
    if(data_rx == '8')
      exhibit_state = STATION_CHARGE;
    if(data_rx == '9')
      exhibit_state = OUTAGE_CHARGE;
  }

  int strip_assignments[NUM_STRIPS];
  // assign city branch and house branch one of the following
      // 0: OFF
      // 1: OUTAGE
      // 2: INFLOW
      // 3: OUTFLOW
      // 4: HOUSE
      // 5: SYSERROR
  if(exhibit_state == EXHIBIT_OFF) {
    // all strips off
    for(int strip_num = 0; strip_num < NUM_STRIPS; strip_num++) {
      strip_assignments[strip_num] = OFF;
    }
  }
  else if(exhibit_state == REGULAR_GRID) {    
    strip_assignments[STRIP_0] = OUTFLOW; // Main Line
    strip_assignments[STRIP_1] = OUTFLOW; // Branch to House C, D
    strip_assignments[STRIP_2] = OUTFLOW; // Branch to House B, A
    strip_assignments[STRIP_3] = INFLOW; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = HOUSE; // House B, C Internal
    strip_assignments[STRIP_5] = HOUSE; // House A Internal
    strip_assignments[STRIP_6] = INFLOW; // House A -> House B line
    strip_assignments[STRIP_7] = HOUSE; // House D Internal
  }
  else if(exhibit_state == POWER_OUTAGE) {
    // everything on house side turns off until car arrives to supply power to house
    strip_assignments[STRIP_0] = OFF;
    strip_assignments[STRIP_1] = OFF;
    strip_assignments[STRIP_2] = OFF;
    strip_assignments[STRIP_3] = OFF;
    strip_assignments[STRIP_4] = OFF;
    strip_assignments[STRIP_5] = OFF;
    strip_assignments[STRIP_6] = OFF;
    strip_assignments[STRIP_7] = OFF; 
  }
  else if(exhibit_state == NANO_GRID) {
    strip_assignments[STRIP_0] = OFF; // Main Line
    strip_assignments[STRIP_1] = OFF; // Branch
    strip_assignments[STRIP_2] = OFF; // Branch
    strip_assignments[STRIP_3] = OFF; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = OFF; // House B, C, D Internal
    strip_assignments[STRIP_5] = HOUSE; // House A Internal
    strip_assignments[STRIP_6] = OFF; // House A -> House B line
    strip_assignments[STRIP_7] = OFF; // House D Internal
  }
  else if(exhibit_state == MICRO_GRID) {
    strip_assignments[STRIP_0] = OFF; // Main Line
    strip_assignments[STRIP_1] = OFF; // Branch
    strip_assignments[STRIP_2] = OFF; // Branch
    strip_assignments[STRIP_3] = INFLOW; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = HOUSE; // House B, C, D Internal
    strip_assignments[STRIP_5] = HOUSE; // House A Internal
    strip_assignments[STRIP_6] = INFLOW; // House A -> House B line
    strip_assignments[STRIP_7] = OFF; // House D Internal
  }
  else if(exhibit_state == HOUSE_CHARGE) {
    // like regular grid, except house garage has charging animation
    strip_assignments[STRIP_0] = OUTFLOW; // Main Line
    strip_assignments[STRIP_1] = OUTFLOW; // Branch to House C, D
    strip_assignments[STRIP_2] = OUTFLOW; // Branch to House B, A
    strip_assignments[STRIP_3] = INFLOW; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = HOUSE; // House B, C Internal
    strip_assignments[STRIP_5] = HOUSE_CHARGE_LIGHT; // House A Internal
    strip_assignments[STRIP_6] = INFLOW; // House A -> House B line
    strip_assignments[STRIP_7] = HOUSE; // House D Internal
  }
  else if(exhibit_state == STATION_CHARGE) {
    // like regular grid, except station has charging animation
    strip_assignments[STRIP_0] = OUTFLOW; // Main Line
    strip_assignments[STRIP_1] = OUTFLOW; // Branch to House C, D
    strip_assignments[STRIP_2] = OUTFLOW; // Branch to House B, A
    strip_assignments[STRIP_3] = INFLOW; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = HOUSE; // House B, C Internal
    strip_assignments[STRIP_5] = HOUSE; // House A Internal
    strip_assignments[STRIP_6] = INFLOW; // House A -> House B line
    strip_assignments[STRIP_7] = HOUSE; // House D Internal
    
  }
  else if(exhibit_state == GARAGE_CHARGE) {
    // like regular grid, except garage has charging animation
    strip_assignments[STRIP_0] = OUTFLOW; // Main Line
    strip_assignments[STRIP_1] = OUTFLOW; // Branch to House C, D
    strip_assignments[STRIP_2] = OUTFLOW; // Branch to House B, A
    strip_assignments[STRIP_3] = INFLOW; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = HOUSE; // House B, C Internal
    strip_assignments[STRIP_5] = HOUSE; // House A Internal
    strip_assignments[STRIP_6] = INFLOW; // House A -> House B line
    strip_assignments[STRIP_7] = HOUSE; // House D Internal
  }
  else if(exhibit_state == OUTAGE_CHARGE) {
    // like regular grid, except house garage has charging animation
    strip_assignments[STRIP_0] = OFF; // Main Line
    strip_assignments[STRIP_1] = OFF; // Branch to House C, D
    strip_assignments[STRIP_2] = OFF; // Branch to House B, A
    strip_assignments[STRIP_3] = INFLOW; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = HOUSE; // House B, C Internal
    strip_assignments[STRIP_5] = HOUSE_CHARGE_LIGHT; // House A Internal
    strip_assignments[STRIP_6] = INFLOW; // House A -> House B line
    strip_assignments[STRIP_7] = OFF; // House D Internal
  }
  else {
    // all strips error
    strip_assignments[STRIP_0] = SYSERROR; // Main Line
    strip_assignments[STRIP_1] = SYSERROR; // Branch
    strip_assignments[STRIP_2] = SYSERROR; // Branch
    strip_assignments[STRIP_3] = SYSERROR; // HOUSE B -> House C Line
    strip_assignments[STRIP_4] = SYSERROR; // House B, C, D Internal
    strip_assignments[STRIP_5] = SYSERROR; // House A Internal
    strip_assignments[STRIP_6] = SYSERROR; // House A -> House B line
    strip_assignments[STRIP_7] = SYSERROR; // House D Internal
  }
  
  display_strips(strip_assignments);
}

// function to set pixels on all strips
// runs at each iteration of main loop (after receiving uart transmission)
void display_strips(int * strip_pattern) {
  // each strip-branch system can exist in 4 states
  // 0: OFF
  // 1: OUTAGE
  // 2: INFLOW
  // 3: OUTFLOW
  // 4: HOUSE
  // 5: SYSERROR

  // outer loop determines pattern flow / changes -> outerloop needs to last as long as it takes to fill the strips w/ constant pattern
  for(int anim_frame = 0; anim_frame < ANIM_LOOPS; anim_frame++) {

    pulse_anim_frame++;
    if(pulse_anim_frame > PULSE_ANIM_LOOPS)
      pulse_anim_frame = 0;

    // ***** CHARGE PATTERN **********
    if(strip_pattern[STRIP_5] == HOUSE_CHARGE_LIGHT) {
      house_charge_pattern(STRIP_5_LENGTH, STRIP_5, pulse_anim_frame);
    }
    // ********************************
    
    // ***** TURN OFF PATTERN *********
    if(strip_pattern[0] == OFF) {
        turn_off_pattern(STRIP_0_LENGTH, STRIP_0);
    }
    if(strip_pattern[1] == OFF) {
        turn_off_pattern(STRIP_1_LENGTH, STRIP_1);
    }
    if(strip_pattern[2] == OFF) {
        turn_off_pattern(STRIP_2_LENGTH, STRIP_2);
    }
    if(strip_pattern[3] == OFF) {
        turn_off_pattern(STRIP_3_LENGTH, STRIP_3);
    }
    if(strip_pattern[4] == OFF) {
        turn_off_pattern(STRIP_4_LENGTH, STRIP_4);
    }
    if(strip_pattern[5] == OFF) {
        turn_off_pattern(STRIP_5_LENGTH, STRIP_5);
    }
    if(strip_pattern[6] == OFF) {
        turn_off_pattern(STRIP_6_LENGTH, STRIP_6);
    }
    if(strip_pattern[7] == OFF) {
        turn_off_pattern(STRIP_7_LENGTH, STRIP_7);
    }
    // ********************************
    
    // ***** HOUSE LIGHTS PATTERN *****
    if(strip_pattern[0] == HOUSE) {
        house_light_pattern(STRIP_0_LENGTH, STRIP_0);
    }
    if(strip_pattern[1] == HOUSE) {
        house_light_pattern(STRIP_1_LENGTH, STRIP_1);
    }
    if(strip_pattern[2] == HOUSE) {
        house_light_pattern(STRIP_2_LENGTH, STRIP_2);
    }
    if(strip_pattern[3] == HOUSE) {
        house_light_pattern(STRIP_3_LENGTH, STRIP_3);
    }
    if(strip_pattern[4] == HOUSE) {
        house_light_pattern(STRIP_4_LENGTH, STRIP_4);
    }
    if(strip_pattern[5] == HOUSE) {
        house_light_pattern(STRIP_5_LENGTH, STRIP_5);
    }
    if(strip_pattern[6] == HOUSE) {
        house_light_pattern(STRIP_6_LENGTH, STRIP_6);
    }
    if(strip_pattern[7] == HOUSE) {
        house_light_pattern(STRIP_7_LENGTH, STRIP_7);
    }
    // ********************************
    
    // ***** ERROR PATTERN *****
    if(strip_pattern[0] == SYSERROR) {
        error_pattern(STRIP_0_LENGTH, STRIP_0, loop_flipper);
    }
    if(strip_pattern[1] == SYSERROR) {
        error_pattern(STRIP_1_LENGTH, STRIP_1, loop_flipper);
    }
    if(strip_pattern[2] == SYSERROR) {
        error_pattern(STRIP_2_LENGTH, STRIP_2, loop_flipper);
    }
    if(strip_pattern[3] == SYSERROR) {
        error_pattern(STRIP_3_LENGTH, STRIP_3, loop_flipper);
    }
    if(strip_pattern[4] == SYSERROR) {
        error_pattern(STRIP_4_LENGTH, STRIP_4, loop_flipper);
    }
    if(strip_pattern[5] == SYSERROR) {
        error_pattern(STRIP_5_LENGTH, STRIP_5, loop_flipper);
    }
    if(strip_pattern[6] == SYSERROR) {
        error_pattern(STRIP_6_LENGTH, STRIP_6, loop_flipper);
    }
    if(strip_pattern[7] == SYSERROR) {
        error_pattern(STRIP_7_LENGTH, STRIP_7, loop_flipper);
    }
    // ********************************
    
    // ***** OUT FLOW PATTERN *****
    if(strip_pattern[0] == OUTFLOW) {
        outflow_pattern(STRIP_0_LENGTH, STRIP_0, anim_frame);
    }
    if(strip_pattern[1] == OUTFLOW) {
        outflow_pattern(STRIP_1_LENGTH, STRIP_1, anim_frame);
    }
    if(strip_pattern[2] == OUTFLOW) {
        outflow_pattern(STRIP_2_LENGTH, STRIP_2, anim_frame);
    }
    if(strip_pattern[3] == OUTFLOW) {
        outflow_pattern(STRIP_3_LENGTH, STRIP_3, anim_frame);
    }
    if(strip_pattern[4] == OUTFLOW) {
        outflow_pattern(STRIP_4_LENGTH, STRIP_4, anim_frame);
    }
    if(strip_pattern[5] == OUTFLOW) {
        outflow_pattern(STRIP_5_LENGTH, STRIP_5, anim_frame);
    }
    if(strip_pattern[6] == OUTFLOW) {
        outflow_pattern(STRIP_6_LENGTH, STRIP_6, anim_frame);
    }
    if(strip_pattern[7] == OUTFLOW) {
        outflow_pattern(STRIP_7_LENGTH, STRIP_7, anim_frame);
    }
    // ***** ***** *****
    
    // ***** INFLOW PATTERN *****
    if(strip_pattern[0] == INFLOW) {
        inflow_pattern(STRIP_0_LENGTH, STRIP_0, anim_frame);
    }
    if(strip_pattern[1] == INFLOW) {
        inflow_pattern(STRIP_1_LENGTH, STRIP_1, anim_frame);
    }
    if(strip_pattern[2] == INFLOW) {
        inflow_pattern(STRIP_2_LENGTH, STRIP_2, anim_frame);
    }
    if(strip_pattern[3] == INFLOW) {
        inflow_pattern(STRIP_3_LENGTH, STRIP_3, anim_frame);
    }
    if(strip_pattern[4] == INFLOW) {
        inflow_pattern(STRIP_4_LENGTH, STRIP_4, anim_frame);
    }
    if(strip_pattern[5] == INFLOW) {
        inflow_pattern(STRIP_5_LENGTH, STRIP_5, anim_frame);
    }
    if(strip_pattern[6] == INFLOW) {
        inflow_pattern(STRIP_6_LENGTH, STRIP_6, anim_frame);
    }
    if(strip_pattern[7] == INFLOW) {
        inflow_pattern(STRIP_7_LENGTH, STRIP_7, anim_frame);
    }
    // ***** ***** *****

    FastLED.show();
    delay(ANIM_FREQ);
  }

  loop_flipper = !loop_flipper;
}

void turn_off_pattern(int strip_length, int strip_num) {
  int off_pixel_index = 0;
  while(off_pixel_index < strip_length) {
    setPixel(off_pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
    off_pixel_index++;
  }
}

void house_light_pattern(int strip_length, int strip_num) {
  int house_pixel_index = 0;
  while(house_pixel_index < strip_length) {
    setPixel(house_pixel_index, strip_num, HOUSE_R, HOUSE_G, HOUSE_B);
    house_pixel_index++;
  }
}

void error_pattern(int strip_length, int strip_num, int loop_state) {
  int error_pixel_index = 0;
  while(error_pixel_index < strip_length) {
    if(loop_state) {
      setPixel(error_pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
      error_pixel_index++;
    }
    else {
      setPixel(error_pixel_index, strip_num, ERROR_R, ERROR_G, ERROR_B);
      error_pixel_index++;
    }
  }
}

void inflow_pattern(int strip_length, int strip_num, int anim_frame) {
  int pixel_index = 0; 
  if(anim_frame < ANIM_LOOPS - STRIP_EYE + 1) {
    // display gap or partial gap
    for(int a = 0; a < STRIP_GAP - anim_frame; a++) {
      if(pixel_index < strip_length) {
        setPixel(pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
        pixel_index++;
      }
    }
  }
  else {
    // display eye
    for(int a = 0; a < STRIP_EYE + STRIP_GAP - anim_frame; a++) {
      if(pixel_index < strip_length) {
        setPixel(pixel_index, strip_num, FLOW_R, FLOW_G, FLOW_B);
        pixel_index++;
      }
    }
    // then a full set of gaps
    for(int a = 0; a < STRIP_GAP; a++) {
      if(pixel_index < strip_length) {
        setPixel(pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
        pixel_index++;
      }
    }
  }
  while(pixel_index < strip_length) {
    // then fill rest of strip with alternating eyes/gaps
    for(int eye_i = 0; eye_i < STRIP_EYE; eye_i++) {
      if(pixel_index < strip_length) {
        setPixel(pixel_index, strip_num, FLOW_R, FLOW_G, FLOW_B);
        pixel_index++;
      }
    }
    for(int gap_i = 0; gap_i < STRIP_GAP; gap_i++) {
      if(pixel_index < strip_length) {
        setPixel(pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
        pixel_index++;
      }
    }
  }
}

void outflow_pattern(int strip_length, int strip_num, int anim_frame) {
  int flow_pixel_index = 0; 
  if(anim_frame < ANIM_LOOPS - STRIP_EYE + 1) {
    // display gap
    for(int a = 0; a < anim_frame; a++) {
      if(flow_pixel_index < strip_length) {
        setPixel(flow_pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
        flow_pixel_index++;
      }
    }
  }
  else {
    // display eye
    for(int a = 0; a < anim_frame-STRIP_GAP; a++) {
      if(flow_pixel_index < strip_length) {
        setPixel(flow_pixel_index, strip_num, FLOW_R, FLOW_G, FLOW_B);
        flow_pixel_index++;
      }
    }
    // then a full set of gaps
    for(int a = 0; a < STRIP_GAP; a++) {
      if(flow_pixel_index < strip_length) {
        setPixel(flow_pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
        flow_pixel_index++;
      }
    }
  }
  while(flow_pixel_index < strip_length) {
    
    for(int eye_i = 0; eye_i < STRIP_EYE; eye_i++) {
      if(flow_pixel_index < strip_length) {
        setPixel(flow_pixel_index, strip_num, FLOW_R, FLOW_G, FLOW_B);
        flow_pixel_index++;
      }
    }
    for(int gap_i = 0; gap_i < STRIP_GAP; gap_i++) {
      if(flow_pixel_index < strip_length) {
        setPixel(flow_pixel_index, strip_num, PIXEL_OFF, PIXEL_OFF, PIXEL_OFF);
        flow_pixel_index++;
      }
    }
  }
}

void house_charge_pattern(int strip_length, int strip_num, int anim_frame) {
  // want first 7 to be regular house lights
  int pixel_index = 0;
  for(int i = 0; i < 7; i++) {
    setPixel(pixel_index, strip_num, HOUSE_R, HOUSE_G, HOUSE_B);
    pixel_index++;
  }

  // want last 7 to be charge pulse
  // frames 1, 2, 3, 4, 5, 6, 7, ..., 36
  // want low brightness -> high brightness
  // want 0 -> 18 = 0 -> 255 and 19 -> 36 = 255 -> 0
  
  for(int i = 0; i < 7; i++) {
    if(anim_frame < 19)
      setPixel(pixel_index, strip_num, CHARGE_R, anim_frame*14, CHARGE_B);  
    else
      setPixel(pixel_index, strip_num, CHARGE_R, -15*anim_frame + 540, CHARGE_B);
 
    pixel_index++;
  }
}

void setPixel(int pixel_index, int strip_num, int r, int g, int b) {
  if(strip_num == STRIP_0) {
    strip0[pixel_index] = CRGB(r, g, b);
  }
  if(strip_num == STRIP_1) {
    strip1[pixel_index] = CRGB(r, g, b);
  }
  if(strip_num == STRIP_2) {
    strip2[pixel_index] = CRGB(r, g, b);
  }
  if(strip_num == STRIP_3) {
    strip3[pixel_index] = CRGB(r, g, b);
  }
  if(strip_num == STRIP_4) {
    strip4[pixel_index] = CRGB(r, g, b);
  }
  if(strip_num == STRIP_5) {
    strip5[pixel_index] = CRGB(r, g, b);
  }
  if(strip_num == STRIP_6) {
    strip6[pixel_index] = CRGB(r, g, b);
  }
  if(strip_num == STRIP_7) {
    strip7[pixel_index] = CRGB(r, g, b);
  }
}
