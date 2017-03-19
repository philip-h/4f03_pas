/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "append.h"

bool_t
xdr_append_init_params (XDR *xdrs, append_init_params *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->f))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->l))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->m))
				 return FALSE;

		} else {
		IXDR_PUT_LONG(buf, objp->f);
		IXDR_PUT_LONG(buf, objp->l);
		IXDR_PUT_LONG(buf, objp->m);
		}
		 if (!xdr_char (xdrs, &objp->c0))
			 return FALSE;
		 if (!xdr_char (xdrs, &objp->c1))
			 return FALSE;
		 if (!xdr_char (xdrs, &objp->c2))
			 return FALSE;
		 if (!xdr_string (xdrs, &objp->host_verify, ~0))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->f))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->l))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->m))
				 return FALSE;

		} else {
		objp->f = IXDR_GET_LONG(buf);
		objp->l = IXDR_GET_LONG(buf);
		objp->m = IXDR_GET_LONG(buf);
		}
		 if (!xdr_char (xdrs, &objp->c0))
			 return FALSE;
		 if (!xdr_char (xdrs, &objp->c1))
			 return FALSE;
		 if (!xdr_char (xdrs, &objp->c2))
			 return FALSE;
		 if (!xdr_string (xdrs, &objp->host_verify, ~0))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->f))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->l))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->m))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->c0))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->c1))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->c2))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->host_verify, ~0))
		 return FALSE;
	return TRUE;
}
