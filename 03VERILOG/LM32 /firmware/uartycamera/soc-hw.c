#include "soc-hw.h"

uart_t  *uart0  = (uart_t *)   0x20000000;
timer_t *timer0 = (timer_t *)  0x30000000;
gpio_t  *gpio0  = (gpio_t *)   0x70000000;
spi_t   *spi0   = (spi_t *)    0x50000000;
camera_t   *camera0   = (camera_t *)    0x60000000;
pwm_t  *pwm	= (pwm_t *)    0x40000000;

isr_ptr_t isr_table[32];
//pwm
uint32_t pwm_read(){	//escribe en el pin
	return pwm -> d_out;
}
void pwm_rd(uint8_t vrd){
pwm -> rd = vrd;
}
void pwm_addr(uint32_t vaddr){
pwm -> addr = vaddr;
}
void pwm_wr(uint8_t vwr){
pwm -> wr = vwr;
}
void pwm_din(uint32_t vd_in){
pwm -> d_in = vd_in;
}


//camera

void tomarfoto(int a){
camera0->resetwr=1;
camera0->takepicture=a;
}

uint32_t leerfoto(int b){
camera0->leer=b;
camera0->resetrd=1;
return camera0->dout;
}


/*
void pwm_wr (int pwm_sel, int T, int D ){
	if (pwm_sel==0) {
		pwm -> addr = 0x00;
		pwm -> wr = 1;
		pwm -> rd = 0;
		pwm -> d_in = 1;
		msleep(5);
		pwm -> addr = 0x04;
		pwm -> wr = 1;
		pwm -> rd = 0;
		pwm -> d_in = T;
		msleep(5);
		pwm -> addr = 0x08;
		pwm -> wr = 1;
		pwm -> rd = 0;
		pwm -> d_in = D;
		}
	else{
		pwm -> rd = 1;}
		
}
*/


/***************************************************************************
 * GPIO
 */

void gpio_config_dir(uint32_t vdir){	//configira la direccion del pin
	gpio0 -> dir = vdir;
}

void gpio_write(uint32_t vpins){	//escribe en el pin
	gpio0 -> wr = vpins;
}

uint32_t gpio_read(){	//escribe en el pin
	return gpio0 -> rd;
}


/***************************************************************************
 * IRQ handling
 */
void isr_null()
{
}

void irq_handler(uint32_t pending)
{
	int i;

	for(i=0; i<32; i++) {
		if (pending & 0x01) (*isr_table[i])();
		pending >>= 1;
	}
}

void isr_init()
{
	int i;
	for(i=0; i<32; i++)
		isr_table[i] = &isr_null;
}

void isr_register(int irq, isr_ptr_t isr)
{
	isr_table[irq] = isr;
}

void isr_unregister(int irq)
{
	isr_table[irq] = &isr_null;
}
void tic_isr();
/***************************************************************************
 * TIMER Functions
 */
void msleep(uint32_t msec)
{
	uint32_t tcr;

	// Use timer0.1
	timer0->compare1 = (FCPU/1000)*msec;
	timer0->counter1 = 0;
	timer0->tcr1 = TIMER_EN;

	do {
		//halt();
 		tcr = timer0->tcr1;
 	} while ( ! (tcr & TIMER_TRIG) );
}

void nsleep(uint32_t nsec)
{
	uint32_t tcr;

	// Use timer0.1
	timer0->compare1 = (FCPU/1000000)*nsec;
	timer0->counter1 = 0;
	timer0->tcr1 = TIMER_EN;

	do {
		//halt();
 		tcr = timer0->tcr1;
 	} while ( ! (tcr & TIMER_TRIG) );
}


uint32_t tic_msec;

void tic_isr()
{
	tic_msec++;
	timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;
}

void tic_init()
{
	tic_msec = 0;

	// Setup timer0.0
	timer0->compare0 = (FCPU/10000);
	timer0->counter0 = 0;
	timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;

	isr_register(1, &tic_isr);
}


// **************************************************************************
 // * UART Functions
 
void uart_init()
{
	//uart0->ier = 0x00;  // Interrupt Enable Register
	//uart0->lcr = 0x03;  // Line Control Register:    8N1
	//uart0->mcr = 0x00;  // Modem Control Register

	// Setup Divisor register (Fclk / Baud)
	//uart0->div = (FCPU/(57600*16));
}

char uart_getchar()
{   
	while (! (uart0->ucr & UART_DR)) ;
	return uart0->rxtx;
}

void uart_putchar(char c)
{
	while (uart0->ucr & UART_BUSY) ;
	uart0->rxtx = c;
}

void uart_putstr(char *str)
{
	char *c = str;
	while(*c) {
		uart_putchar(*c);
		c++;
	}
}



