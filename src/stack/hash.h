#ifndef __HASH_H_
#define __HASH_H_

/*! \file
* \brief Contains hash functions
*/

#include <stdio.h>

#include "types.h"

/************************************************************//**
 * @brief Counts hash by MurmurHash function
 *
 * @param[in] obj object
 * @param[in] size object size
 * @return hash_t object's hash
 *************************************************************/
hash_t MurmurHash (const void* obj, size_t size);

#endif
