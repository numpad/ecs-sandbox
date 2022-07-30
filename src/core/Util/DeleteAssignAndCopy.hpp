#pragma once

#ifndef DELETE_ASSIGN_AND_COPY
	#define DELETE_ASSIGN_AND_COPY(CLASS)                                                                              \
		CLASS(const CLASS&) = delete;                                                                                  \
		CLASS& operator=(const CLASS&) = delete;
#endif
