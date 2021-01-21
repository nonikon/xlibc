#ifndef _XCONFIG_H_
#define _XCONFIG_H_

/* cache can decrease memory allocation. node (or block) will
 * be put into cache when it being erased, and next insertion
 * will pop one node (or block) from cache. define 'XARRAY_ENABLE_CACHE=1'
 * to enable it. */
#define XARRAY_ENABLE_CACHE 0

/* cache can decrease memory allocation. node will be put into cache
 * when it being erased, and next insertion will pop one node from
 * cache. define 'XHASH_ENABLE_CACHE=1' to enable it. */
#define XHASH_ENABLE_CACHE 0

/* cache can decrease memory allocation. node will be put into cache
 * when it being erased, and next insertion will pop one node from
 * cache. define 'XLIST_ENABLE_CACHE=1' to enable it. */
#define XLIST_ENABLE_CACHE 0

/* enable xlist_msort interface or not. */
#define XLIST_ENABLE_SORT 1

/* enable xlist_cut_* interface or not. */
#define XLIST_ENABLE_CUT 1

/* cache can decrease memory allocation. node will be put into cache
 * when it being erased, and next insertion will pop one node from
 * cache. define 'XRBT_ENABLE_CACHE=1' to enable it. */
#define XRBT_ENABLE_CACHE 0

#define XSTR_DEFAULT_CAPACITY 32

/* uncomment this line to disable some interface such as 'uitoa'. */
/* #undef XSTR_NO_EXTRA */

#define XVEC_DEFAULT_CAPACITY 16

#endif // _XCONFIG_H_
