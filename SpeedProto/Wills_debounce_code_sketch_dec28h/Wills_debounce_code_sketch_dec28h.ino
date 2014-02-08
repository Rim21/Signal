/*******************************************************************************
 * Button management
 *
 ******************************************************************************/

/* uint8_t doDebounce(uint8_t *state, volatile uint8_t *port, uint8_t pin)
 *
 * This function implements the logic for detecting button transitions for
 * arbitrary ports and bits.  The return value is the debounced value the the
 * given pin.
 *
 * The implementation of this function is inspired by the digital filter 
 * presented in ECE573 at Oregon State University.  It is different because
 * I'm using the single 8-bit variable to store all of the state, rather than a
 * 8 bit accumulator and 8 bit flag.  We're using the range from 0x00 -> 0x7F
 * and bit 7 (0x80) as the flag.
 * 
 * The user of this function must provide a static state variable.  This
 * value encodes the state and history of the pin  Failure to maintain the state
 * would cause erratic behavior.  The port can be any of the PINn 
 * memory-mapped input buffers.  Pin must be between 0 and 7.
 *
 * Because these buttons pull to ground, we'll invert the meaning of the edges
 * in software, 1 = yes is much more natural.
 */
static inline uint8_t 
doDebounce(uint8_t *state, volatile uint8_t *port, uint8_t pin)
{
        uint8_t old  =  *state & 0x7F;
        uint8_t flag = (*state & 0x80)? 1 : 0;
        
        // Digital filter part, value = (old * .75) + (new * .25)
        old -= (old >> 2);                                                                // 1 - (1/4) = .75
        old += ((*port) & (1 << pin))? 0x1F : 0x00;                // if bit set, add .25
        
        // Software schmitt trigger
        // Newly detected rising edge
        if ( (flag == 1) && (old > 0x70) ) {
                flag = 0;                
        }
        
        // Newly detected falling edge
        else if ( (flag == 0) && (old < 0x07) ){
                flag = 1;                
        }
        
        // Update the state variable
        *state = (old & 0x7F) | ((flag & 0x01) << 7);
        
        // Return only the pin state
        return flag;
}

static uint8_t states[8];                // Used internally for button debouncing
volatile uint8_t buttons;                // This always contains current button state
volatile uint8_t button_flag;        // Indicates new button data
volatile uint8_t new_button;        // Contains the newest PORTA read
static inline void scan_buttons(void)
{
        uint8_t old_porta;
        uint8_t old_digit;
        
        // Copy the values from the port and digit
        old_digit = PORTB & 0x70;
        old_porta = PORTA;
        
        // Setup PORTA as an input with pullups enabled
        // First, set the digit selector to some unused setting (we'll use 5)
        PORTB = (PORTB & 0x0F) | (5 << 4);
        PORTA = 0xFF;
        DDRA  = 0x00;
        
        // Next, we switch the demultiplexer to the switch port
        PORTB = (PORTB & 0x0F) | (7 << 4);
        
        button_flag = 1;
        new_button = PINA;
        
        // Return the system to an appropriate state
        PORTB = (PORTB & 0x0F) | old_digit;
        PORTA = old_porta;
        DDRA  = 0xFF;
}

void loop_button(void)
{
        uint8_t i, temp;

        if (button_flag) {
                for (i = 0; i < 8; i++) {
                        temp = doDebounce(&(states[i]), &new_button, i);
                        if (temp) {
                                buttons |= 0x01 << i;
                        } else {
                                buttons &= ~(0x01 << i);
                        }
                }
        }
}

uint8_t get_buttons(void)
{
        return buttons;
}

