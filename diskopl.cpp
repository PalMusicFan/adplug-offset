/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999, 2000, 2001 Simon Peter, <dn.tlp@gmx.net>, et al.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * diskopl.cpp - Disk Writer OPL, by Simon Peter <dn.tlp@gmx.net>
 */

#include "diskopl.h"

static const unsigned short note_table[12] = {363,385,408,432,458,485,514,544,577,611,647,686};
static const unsigned char op_table[9] = {0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12};

CDiskopl::CDiskopl(std::string filename): old_freq(0.0f), touched(false), del(1)
{
	unsigned short clock = 0xffff;
	f = fopen(filename.c_str(),"wb");
	fwrite("RAWADATA",8,1,f);
	fwrite(&clock,sizeof(clock),1,f);
}

CDiskopl::~CDiskopl()
{
	fclose(f);
}

void CDiskopl::update(CPlayer *p)
{
	unsigned short	clock;
	unsigned int	wait;

	if(p->getrefresh() != old_freq) {
		old_freq = p->getrefresh();
		del = wait = 18.2f / old_freq;
		clock = 1192737/(old_freq*(wait+1));
		fputc(0,f); fputc(2,f);
		fwrite(&clock,2,1,f);
	}

/*	if(touched) {
		if(del) {
			fputc(del,f); fputc(0,f);
			del = 0;
		} else {
			fputc(1,f); fputc(0,f);
		}
		touched = false;
	} else {
		if(del < 255)
			del++;
		else {
			fputc(del,f); fputc(0,f);
			del = 0;
		}
	} */
	fputc(del+1,f); fputc(0,f);
}

void CDiskopl::write(int reg, int val)
{
	diskwrite(reg,val);
	touched = true;
}

void CDiskopl::init()
{
	for (int i=0;i<9;i++) {				// stop instruments
		diskwrite(0xb0 + i,0);				// key off
		diskwrite(0x80 + op_table[i],0xff);	// fastest release
	}
	diskwrite(0xbd,0);	// clear misc. register
}

void CDiskopl::diskwrite(int reg, int val)
{
	fputc(val,f);
	fputc(reg,f);
}
