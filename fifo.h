/*
 * Copyright (c) 2014 Artem Dvinin <artem_dvinin[dot]mail.ru>
 * Site: http://mainloop.ru/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _FIFO_H_
#define _FIFO_H_

//size must be a power of two: 4,8,16,32...128
//To use this code for fifo with size 256 or more,
//it is necessary to change the head and tail type from unsigned char to unsigned short
/* #define FIFO(size)			\
struct {						\
	unsigned char buf[size];	\
	unsigned char tail;			\
	unsigned char head;			\
} */

#define FIFO(size)				\
struct {						\
	unsigned short buf[size];	\
	unsigned short tail;		\
	unsigned short head;		\
}

// the number of elements in the queue
#define FIFO_COUNT(fifo) (fifo.head - fifo.tail)

// size of fifo
#define FIFO_SIZE(fifo) (sizeof(fifo.buf) / sizeof(fifo.buf[0]))

// fifo is filled?
#define FIFO_IS_FULL(fifo) (FIFO_COUNT(fifo) == FIFO_SIZE(fifo))

// fifo is empty?
#define FIFO_IS_EMPTY(fifo) (fifo.tail == fifo.head)

// free space in fifo
#define FIFO_SPACE(fifo) (FIFO_SIZE(fifo) - FIFO_COUNT(fifo))

// put an element in fifo
#define FIFO_PUSH(fifo, byte)							\
{														\
	fifo.buf[fifo.head & (FIFO_SIZE(fifo) - 1)] = byte;	\
	fifo.head++;										\
}

// take the first element of fifo
#define FIFO_FRONT(fifo) (fifo.buf[fifo.tail & (FIFO_SIZE(fifo) - 1)])

// reduce the number of elements in the queue
#define FIFO_POP(fifo)	\
{						\
	fifo.tail++;		\
}

// clear fifo
#define FIFO_FLUSH(fifo)	\
{							\
	fifo.tail=0;			\
	fifo.head=0;			\
}

#endif /* _FIFO_H_ */
