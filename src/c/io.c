#include "io.h"
#include "mmu.h"

/*
 * Serial port functions (physical and virtual addresses)
 */

#define MU_10_PHYSICAL 0x3F215040
#define MU_10_VIRTUAL 0x3F000000
#define DR (*(uint8_t *)0x3F215040)

int putp(int data) {
	volatile unsigned int *mu10 = (volatile unsigned int *)MU_10_PHYSICAL;
	*mu10 = (unsigned int)(data & 0xFF);
	return data;
}
/*
int putv(int data) {
	volatile unsigned int *mu10 = (volatile unsigned int *)MU_10_VIRTUAL;
	*mu10 = (unsigned int)(data & 0xFF);
	return data;
}
*/

int putv(char data) {
	volatile unsigned int *uart_dr = (volatile unsigned int *)MU_10_VIRTUAL;
	*uart_dr = (unsigned int)data;
	return data;
}

void putu(unsigned int x) {
	char buf[10];
	int i = 0;
	if (x == 0) {
		putp('0');
		return;
	}

	while (x > 0) {
		buf[i++] = '0' + (x % 10);
		x /= 10;
	}

	while (--i >= 0) putp(buf[i]);
}

void puthex(unsigned long long x) {
	for (int i = 60; i >= 0; i -= 4) {
		int digit = (x >> i) & 0xF;
		putp(digit < 10 ? '0' + digit : 'A' + (digit - 10));
	}
}

/*
 * Print functions
 */

#define NULL (void *)0

static func_ptr out_char;
static int do_padding;
static int left_flag;
static int len;
static int num1;
static int num2;
static int pad_char;

size_t strLen(const char *str) {
	unsigned int len = 0;
	while (str[len] != '\0') len++;
	return len;
}

int toLower(int c) {
	if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
	return c;
}

int isDigit(int c) {
	if ((c >= '0') && (c <= '9')) return 1;
	return 0;
}

/*
 * This routine puts pad chars into output buffer
 */

static void padding(const int l_flag) {
	int i;
	if (do_padding && l_flag && (len < num1)) {
		for (i = len; i < num1; i++) out_char(pad_char);
	}
}

/*
 * This routine moves a string to output buffer
 * as directed by paddding and position flags
 */

static void outs(charptr lp) {
	if (lp == NULL) lp = "(null)";
	
	// pad on left if needed
	len = strLen(lp);
	padding(!left_flag);

	// move string to buffer
	while (*lp && num2--) out_char(*lp++);

	// pad on right if needed
	len = strLen(lp);
	padding(left_flag);
}

/*
 * This routine moves a number to the output buffer
 * as directed by the padding and position flags
 */

static void outnum(unsigned long long num, const unsigned base) {
	char outbuf[32];
	int i = 0;
	const char digits[] = "0123456789ABCDEF";

	if (num == 0) {
		out_char('0');
		return;
	}

	while (num > 0 && i < sizeof(outbuf)) {
		outbuf[i++] = digits[num % base];
		num /= base;
	}

	while (--i >= 0) out_char(outbuf[i]);
}


/*
static void outnum(long num, const long base) {
	charptr cp;
	int negative;
	char outbuf[32];
	const char digits[] = "0123456789ABCDEF";

	// check if num is negative
	if (num < 0L && base != 16L) {
		negative = 1;
		num = -num;
	} else negative = 0;

	// build number backwards in outbuf
	cp = outbuf;
	do {
		*cp++ = digits[num % base];
	} while ((num /= base) > 0);
	if (negative) *cp++ = '-';
	*cp-- = 0;

	// move the converted num to buffer
	// add in padding where needed
	len = strLen(outbuf);
	padding(!left_flag);
	while (cp >= outbuf) out_char(*cp--);
	padding(left_flag);
}
*/

/*
 * This routine get a num from the format string
 */

static int getnum(charptr *linep) {
	int n = 0;
	charptr cp = *linep;

	while(isDigit((int)*cp)) n = n * 10 + ((*cp++) - '0');
	*linep = cp;
	return(n);
}

/*
 * This routine operates just like printf/sprintf with
 * the exception of some standard C format control.
 * Other format specifiers can be added easily by
 * following the examples shown for supported formats.
 */

void printp(charptr ctrl, ...) {
	va_list args;
	va_start(args, *ctrl);
	vprintf(putp, ctrl, args);
	va_end(args);
}

void printv(charptr ctrl, ...) {
	va_list args;
	va_start(args, *ctrl);
	vprintf(putv, ctrl, args);
	va_end(args);
}

void vprintf(const func_ptr f_ptr, charptr ctrl, va_list argp) {
	int long_flag, dot_flag;
	char ch;

	out_char = f_ptr;

	for (; *ctrl; ctrl++) {
		// move format string chars to buffer
		// until a format control is found.
		if (*ctrl != '%') {
			out_char(*ctrl);
			continue;
		}

		// initialize all flags for this format
		dot_flag = long_flag = left_flag = do_padding = 0;
		pad_char = ' ';
		num2 = 32767;

try_next:
	ch = *(++ctrl);

	if (isDigit((int)ch)) {
		if (dot_flag) num2 = getnum(&ctrl);
		else {
			if (ch == '0') pad_char = '0';
			num1 = getnum(&ctrl);
			do_padding = 1;
		}
		ctrl--;
		goto try_next;
	}

	switch (toLower((int)ch)) {
		case '%':
			out_char('%');
			continue;
		case '-':
			left_flag = 1;
			break;
		case '.':
			dot_flag = 1;
			break;
		case 'l':
			long_flag = 1;
			break;
		case 'i':
		case 'd':
			if (long_flag || ch == 'D') {
				outnum(va_arg(argp, long), 10L);
				continue;
			} else {
				outnum(va_arg(argp, int), 10L);
				continue;
			}
		case 'x':
		case 'X':
			if (long_flag) outnum(va_arg(argp, unsigned long), 16);
			else outnum((unsigned long)va_arg(argp, unsigned int), 16);
			continue;
		case 's':
			outs(va_arg(argp, charptr));
			continue;
		case 'c':
			out_char(va_arg(argp, int));
			continue;
		case '\\':
			switch (*ctrl) {
				case 'a':
					out_char(0x07);
					break;
				case 'h':
					out_char(0x08);
					break;
				case 'r':
					out_char(0x0D);
					break;
				case 'n':
					out_char(0x0D);
					out_char(0x0A);
					break;
				case 't':
					out_char(0x09);
					break;
				default:
					out_char(*ctrl);
					break;
			}
			ctrl++;
			break;
		default:
			continue;
		}
	goto try_next;
	}
	va_end(argp);
}
