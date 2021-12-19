#ifndef _RTEGLCHECK_
#define _RTEGLCHECK_

namespace RTE {
#define glCheck(expr) do{expr; RTE::glCheckError(__FILE__, __LINE__, #expr); }while(false)

	void glCheckError(const char* file, unsigned int line, const char* expr);
}

#endif
