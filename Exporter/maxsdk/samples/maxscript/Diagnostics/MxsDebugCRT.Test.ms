(
	CRTCorruptHeap 4
	local result = CRTCheckMemory()
	assert (result == false) message:"This should have corrupted the heap"
)