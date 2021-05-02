#pragma once

struct CDeletable {
	void *_dummy;
	
	CDeletable()
	 : _dummy{nullptr} {
	}

};
