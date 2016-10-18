-------------------------------------------------------------------------------

The 'lin_drv.h' file was customized for ATmega32/64/M1/C1 in this package.

-------------------------------------------------------------------------------

To customize the 'lin_drv.h' file for ATtiny167, replace:

#define   LIN_PORT_IN      PIND     - by -    #define   LIN_PORT_IN      PINA
#define   LIN_PORT_DIR     DDRD     - by -    #define   LIN_PORT_DIR     DDRA
#define   LIN_PORT_OUT     PORTD    - by -    #define   LIN_PORT_OUT     PORTA
#define   LIN_INPUT_PIN    4        - by -    #define   LIN_INPUT_PIN    0
#define   LIN_OUTPUT_PIN   3        - by -    #define   LIN_OUTPUT_PIN   1

 (c.f. AVR286 Application Note, § 3.2)
-------------------------------------------------------------------------------
