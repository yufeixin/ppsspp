// Copyright (c) 2012- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

class PointerWrap;

#include "Common/CommonTypes.h"

class BlockAllocator
{
public:
	BlockAllocator(int grain = 16) : grain_(grain) {}  // 16 byte granularity by default.
	~BlockAllocator();

	void Init(u32 _rangeStart, u32 _rangeSize, bool suballoc);
	void Shutdown();

	void ListBlocks() const;

	// WARNING: size can be modified upwards!
	u32 Alloc(u32 &size, bool fromTop = false, const char *tag = 0);
	u32 AllocAligned(u32 &size, u32 sizeGrain, u32 grain, bool fromTop = false, const char *tag = 0);
	u32 AllocAt(u32 position, u32 size, const char *tag = 0);

	bool Free(u32 position);
	bool FreeExact(u32 position);
	bool IsBlockFree(u32 position) {
		Block *b = GetBlockFromAddress(position);
		if (b)
			return !b->taken;
		else
			return false;
	}

	u32 GetBlockStartFromAddress(u32 addr) const;
	u32 GetBlockSizeFromAddress(u32 addr) const;
	u32 GetLargestFreeBlockSize() const;
	u32 GetTotalFreeBytes() const;

	const char *GetBlockTag(u32 addr) const;

	void DoState(PointerWrap &p);

private:
	void CheckBlocks() const;

	struct Block {
		Block(u32 _start, u32 _size, bool _taken, Block *_prev, Block *_next);
		void SetAllocated(const char *_tag, bool suballoc);
		void DoState(PointerWrap &p);
		u32 start;
		u32 size;
		bool taken;
		char tag[32];
		Block *prev;
		Block *next;
	};

	Block *bottom_ = nullptr;
	Block *top_ = nullptr;
	u32 rangeStart_ = 0;
	u32 rangeSize_ = 0;

	u32 grain_;
	bool suballoc_ = false;

	void MergeFreeBlocks(Block *fromBlock);
	Block *GetBlockFromAddress(u32 addr);
	const Block *GetBlockFromAddress(u32 addr) const;
	Block *InsertFreeBefore(Block *b, u32 size);
	Block *InsertFreeAfter(Block *b, u32 size);
};
